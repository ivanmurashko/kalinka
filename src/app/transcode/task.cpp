/**
   @file task.cpp
   @brief This file is part of Kalinka mediaserver.
   @author Ivan Murashko <ivan.murashko@gmail.com>

   Copyright (c) 2007-2012 Kalinka Team

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   CHANGE HISTORY

   @date
   - 2010/01/27 created by ipp (Ivan Murashko)
   - 2010/01/28 renamed taskwrapper -> task by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "task.h"
#include "trans.h"
#include "exception.h"
#include "traps.h"


using namespace klk;
using namespace klk::trans;

//
// Task class
//

// Constructor
Task::Task(IFactory* factory, Transcode* module, const TaskInfoPtr& task) :
    m_lock(),
    m_factory(factory), m_task_info(task),
    m_branch(NULL), m_running_time(0ULL)
{
    BOOST_ASSERT(m_factory);
    BOOST_ASSERT(m_task_info);
    BOOST_ASSERT(module);

    // do the task initialization

    // http://www.parashift.com/c++-faq-lite/exceptions.html#faq-17.4
    // ...
    // If a constructor throws an exception, the object's destructor is not run.
    // ...
    try
    {
        // always init destination
        m_task_info->getDestination()->init(module);

        Result rc = m_task_info->setDurationCallBack(
            boost::bind(&Task::getActualDuration, this));
        if (rc != OK)
        {
            throw Exception(__FILE__, __LINE__,
                            "Setting duration callback was failed");
        }

        // we init source only one time (there can be one source for
        //several tasks)
        m_task_info->getSource()->init(module);
    }
    catch(...)
    {
        klk_log(KLKLOG_DEBUG, "Task '%s' initialization was failed",
                m_task_info->getName().c_str());

        // there was an exception in the constructor
        // we should clear the allocated resources
        deinit();
        throw;
    }
}

// Destructor
Task::~Task()
{
    // base deinitialization
    deinit();

    // update statistic

    // update stat info at the task info holder
    m_task_info->addRunningTime(m_running_time);
    // error processing
    if (!m_running_time)
    {
        // the task was failed
        m_factory->getSNMP()->sendTrap(trap::TASKFAILED, getUUID());
    }
    // clear running time info
    klk_log(KLKLOG_DEBUG, "Task '%s' was deinitialized. Running time: %.3f s",
            m_task_info->getName().c_str(), m_running_time * 1.0e-9);
}

// Do the base resources deinitalization
void Task::deinit() throw()
{
    // clear duration callback
    m_task_info->setDurationCallBack(DurationCallbackDefault());

    // do the task deinitialization
    m_task_info->getDestination()->deinit();
    // clear branch. it will also recalculate running time
    setBranch(NULL);
}

// Sets branch in the paused state
void Task::setPause()
{
    // update running time
    {
        Locker lock(&m_lock);
        m_running_time += getRunningTime(m_branch);
    }

    gst::Element branch(getBranch());
    if (gst_element_set_state (GST_ELEMENT(branch.getElement()),
                               GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE)
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_set_state() was failed");
    }
}

// Creates the tee's branch for the task
void Task::setBranch(GstElement* branch) throw()
{
    Locker lock(&m_lock);
    if (branch == m_branch)
        return; // nothing to do
    if (branch == NULL)
    {
        KLKASSERT(m_branch != NULL);
        gst_object_unref(GST_OBJECT(m_branch));
        m_branch = NULL;
    }
    else
    {
        m_branch = branch;
        gst_object_ref(GST_OBJECT(m_branch));
    }
}

// Retrives the branch (pipeline for transcoding)
// relevant for the task
gst::Element Task::getBranch()
{
    Locker lock(&m_lock);
    if (m_branch == NULL)
    {
        // should be set before the call
        throw Exception(__FILE__, __LINE__,
                        "Branch has not been initialized");
    }
    if (!GST_IS_ELEMENT(m_branch))
    {
        throw Exception(__FILE__, __LINE__,
                        "Transcode branch is not a GstElement");
    }

    return gst::Element(m_branch);
}

// Retrives running time since the branch element
// was last time set to PAUSED
GstClockTime Task::getRunningTime(GstElement* branch)
{
    GstClockTime result = 0ULL;
    if (branch)
    {
        GstClock *clock = gst_element_get_clock(branch);
        if (clock)
        {
            result = gst_clock_get_time(clock) -
                gst_element_get_base_time(branch);
            gst_object_unref(clock);
        }
    }
    return result;
}

// Updates running time
void Task::updateRunningTime()
{
    Locker lock(&m_lock);
    if (m_branch)
    {
        m_running_time += getRunningTime(m_branch);
    }
}

// Retrives actual task duration
GstClockTime Task::getActualDuration()
{
    Locker lock(&m_lock);
    return m_running_time + getRunningTime(m_branch);
}
