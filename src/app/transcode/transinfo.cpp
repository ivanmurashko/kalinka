/**
   @file transinfo.cpp
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
   - 2009/04/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "transinfo.h"
#include "exception.h"
#include "commontraps.h"


using namespace klk;
using namespace klk::trans;

//
// SourceInfo class
//

// Constructor
SourceInfo::SourceInfo(const std::string& uuid, const std::string& name,
                       const std::string& media_type) :
    mod::Info(uuid, name), m_lock(), m_element(NULL), m_direction(UNKNOWN),
    m_media_type(media_type)
{
}

// Sets direction
void SourceInfo::setDirection(const Direction direction)
{
    BOOST_ASSERT(direction != UNKNOWN);
    m_direction.setValue(direction);
}

// Retrives the element
GstElement* SourceInfo::getElement()
{
    Locker lock(&m_lock);
    if (m_element == NULL)
    {
        throw Exception(__FILE__, __LINE__,
                        "Trying to retrive a non existent element");
    }
    return m_element;
}

// Inits element
void SourceInfo::initElement(const std::string& name, IFactory* factory)
{
    BOOST_ASSERT(name.empty() == false);

    Locker lock(&m_lock);
    BOOST_ASSERT(m_element == NULL);
    m_element = gst_element_factory_make(name.c_str(), NULL);
    if (m_element == NULL)
    {
        BOOST_ASSERT(factory);
        factory->getSNMP()->sendTrap(snmp::GSTPLUGINMISSING, name);
        throw Exception(__FILE__, __LINE__, name +
                        " GStreamer plugin missing");
    }
}

// Do nothing if the elemen has been initialized
// before and has SOURCE direction and calls
// klk::trans::SourceInfo::initInternal in other cases
void SourceInfo::init(Transcode* module)
{
    Locker lock(&m_lock);
    if (getDirection() == SOURCE && m_element != NULL)
    {
        // do nothing the source element initialization
        // can be requested several times
        return;
    }
    initInternal(module);
}

// DeInits the file element
void SourceInfo::deinit() throw()
{
    Locker lock(&m_lock);
    m_element = NULL;
}

// Retrives the direction
const Direction SourceInfo::getDirection() const throw()
{
    return m_direction.getValue();
}

//
// TaskInfo class
//

// Constructor
TaskInfo::TaskInfo(const std::string& uuid, const std::string& name,
                   const SourceInfoPtr& source,
                   const SourceInfoPtr& destination,
                   const quality::VideoPtr& vquality,
                   const std::string& start,
                   time_t duration) :
    mod::Info(uuid, name), ScheduleInfo(start, duration),
    m_source(source), m_destination(destination),
    m_vquality(vquality),
    m_running_time(0ULL),
    m_running_count(0), m_get_duration(DurationCallbackDefault())
{
    BOOST_ASSERT(m_source);
    m_source->setDirection(SOURCE);
    BOOST_ASSERT(m_destination);
    m_destination->setDirection(SINK);
}

// Destructor
TaskInfo::~TaskInfo()
{
}

// Was the task started at least one time or not
u_int TaskInfo::getRunningCount() const throw()
{
    Locker lock(&m_lock);
    return m_running_count;
}

// Retrives source info
const SourceInfoPtr TaskInfo::getSource() const throw()
{
    Locker lock(&m_lock);
    return m_source;
}

// Retrives destination info
const SourceInfoPtr TaskInfo::getDestination() const throw()
{
    Locker lock(&m_lock);
    return m_destination;
}

// Retrives actual duration
GstClockTime TaskInfo::getActualDuration() const
{
    Locker lock(&m_lock);
    return m_get_duration();
}

// It adds running time value. The method is called when a task
// was terminated and accepts the duration of the task
void TaskInfo::addRunningTime(GstClockTime duration) throw()
{
    Locker lock(&m_lock);
    if (duration)
    {
        m_running_time += duration;
        // there was a successful run thus update the running count
        m_running_count++;
    }
}

// Retrives total duration for the task
GstClockTime TaskInfo::getRunningTime() const
{
    Locker lock(&m_lock);
    return m_running_time + m_get_duration();
}

// Sets duration callback
Result TaskInfo::setDurationCallBack(DurationCallback callback)
{
    Locker lock(&m_lock);
/*
    if (m_get_duration == callback)
    {
        klk_log(KLKLOG_ERROR, "Trying to set the same callback "
                "for get duration info");
        return ERROR;
    }

    if (!(DurationCallbackDefault() == m_get_duration) &&
        !(DurationCallbackDefault() == callback))
    {
        klk_log(KLKLOG_ERROR, "Trying to set the callback "
                "that has been locked");
        return ERROR;
    }
*/
    // all is ok
    // FIXME!!! add checking
    m_get_duration = callback;

    return OK;
}
