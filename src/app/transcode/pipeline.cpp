/**
   @file pipeline.cpp
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
   - 2009/09/26 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "pipeline.h"
#include "exception.h"
#include "commontraps.h"
#include "branchfactory.h"
#include "mod/infocontainer.h"

using namespace klk;
using namespace klk::trans;

//
// Pipeline class
//

// gst-launch filesrc location=test/test.avi ! tee name=t
// t. ! queue ! filesink location=file.avi
// t. ! queue ! decodebin name=d
//   ffmux_flv name=mux ! filesink location=file.flv
//   d. ! ffmpegcolorspace ! "video/x-raw-yuv",format=\(fourcc\)I420 !
//   videoscale ! "video/x-raw-yuv",width=320,height=240 !
//   ffenc_flv bitrate=600000 ! identity sync=TRUE ! mux.
//   d. ! queue ! audioconvert ! audioresample !
//   audio/x-raw-int,rate=44100 ! lame ! mp3parse ! mux.

// Constructor
Pipeline::Pipeline(IFactory *factory, const gst::IProcessorPtr& processor,
                   const SourceInfoPtr& source) :
    gst::Thread(processor, source->getUUID()),
    m_factory(factory), m_branch_factory(),
    m_source(source),
    m_storage_lock(), m_storage(), m_tee(NULL)
{
    BOOST_ASSERT(m_factory);
    BOOST_ASSERT(m_source);
}

// Destructor
Pipeline::~Pipeline()
{
}

// Do init before startup
void Pipeline::init()
{
    try
    {
        // init main loop and other common GST elements
        gst::Thread::init();

        // there should be at least one task added before
        // now init pipeline
        // gst-launch filesrc location=test/test.avi ! tee name=t

        // necessary plugins
        if (m_tee != NULL)
        {
            throw Exception(__FILE__, __LINE__,
                            "Pipeline should be initialized only one time");
        }
        const char* TEE = "tee";
        m_tee = gst_element_factory_make (TEE, NULL);
        if (m_tee == NULL)
        {
            klk_log(KLKLOG_ERROR, "%s GStreamer plugin missing", TEE);
            m_factory->getSNMP()->sendTrap(snmp::GSTPLUGINMISSING,
                                           TEE);
            throw Exception(__FILE__, __LINE__,
                            std::string(TEE) +
                            " GStreamer plugin missing");
        }
        BOOST_ASSERT(m_tee);

        // source element
        GstElement* source_element = m_source->getElement();
        BOOST_ASSERT(source_element);

        // link the elements
        //
        // do links
        gst::Element pipeline(getPipeline());
        gst_bin_add_many (GST_BIN(pipeline.getElement()),
                          source_element, m_tee, NULL);
        if (!gst_element_link(source_element, m_tee))
        {
            throw Exception(__FILE__, __LINE__,
                            "gst_element_link() was failed");
        }

        // init branch factory
        BOOST_ASSERT(m_branch_factory == NULL);
        m_branch_factory =
            IBranchFactoryPtr(new BranchFactory(m_factory, this));

        // init tasks
        //
        // We should start all tasks (barnches) in one time
        // it's necessary to get valid utest results
        // also it's necessary to prevent very fast processing in the following
        // situation
        // source - file
        // first sink - a file with same media
        // second sink - a network with another media
        // as well as first sink does not require any decoding it will convert
        // all data very fast and the data will be lost for the second sink
        //
        Locker lock(&m_storage_lock);
        // there should be at least one task here
        BOOST_ASSERT(m_storage.empty() == false);
        std::for_each(m_storage.begin(), m_storage.end(),
                      boost::bind(&Pipeline::initTask, this, _1));

    }
    catch(...)
    {
        // we should do cleanup
        clean();
        throw;
    }
}

// Do cleanup
void Pipeline::clean() throw()
{
    if (m_tee)
    {
        //gst_object_unref (m_tee);
        m_tee = NULL;
    }

    {
        Locker lock(&m_storage_lock);

        // deinit all tasks that has not been deinitialized
        // the deinitialization will be called at klk::trans::Task destructor
        m_storage.clear();
    }

    // deinit source
    m_source->deinit();

    // clear branch factory
    m_branch_factory.reset();

    gst::Thread::clean();
}

// Finds a task
Pipeline::Storage::iterator Pipeline::findTask(const std::string& uuid)
{
    return std::find_if(
        m_storage.begin(), m_storage.end(),
        boost::bind(mod::FindInfoByUUID<Task>(), _1, uuid));
}


// Adds a task
void Pipeline::addTask(const TaskPtr& task)
{
    BOOST_ASSERT(task);

    // the source should be same for all tasks
    BOOST_ASSERT(m_source->getUUID() == task->getSourceUUID());

    Locker lock(&m_storage_lock);
    // check that we don't have the task added already
    Storage::iterator find = findTask(task->getUUID());
    if (find != m_storage.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "The task has been already added. UUID: " +
                        task->getUUID());
    }

    // add the task
    m_storage.push_back(task);
}

// Initializes a task for streaming
void Pipeline::initTask(const TaskPtr& task)
{
    //assert(GST_IS_BIN(m_pipeline));
    BOOST_ASSERT(task);

    // the source should be same for all tasks
    BOOST_ASSERT(m_source->getUUID() == task->getSourceUUID());

    // add the element
    BOOST_ASSERT(m_branch_factory); // check initialization
    GstElement *branch = m_branch_factory->createBranch(task);
    BOOST_ASSERT(branch);

    // add the branch to pipeline
    gst::Element pipeline(getPipeline());
    if (!gst_bin_add (GST_BIN (pipeline.getElement()), branch))
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_bin_add() was failed");
    }

    // link the branch with tee
    GstPad *link_pad_src = gst_element_get_request_pad(m_tee, "src%d");
    BOOST_ASSERT(link_pad_src);
    GstPad *link_pad_sink = gst_element_get_pad(branch, "sink");
    BOOST_ASSERT(link_pad_sink);
    if (gst_pad_link (link_pad_src, link_pad_sink) != GST_PAD_LINK_OK)
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_pad_link() was failed");
    }
    gst_object_unref(link_pad_src);
    gst_object_unref(link_pad_sink);

    // init task for future stops
    task->setBranch(branch);
}


// Deletes a task
void Pipeline::delTask(const std::string& uuid)
{
    Locker lock(&m_storage_lock);
    // check that we don't have the task added already
    Storage::iterator find = findTask(uuid);
    if (find != m_storage.end())
    {
        TaskPtr task = (*find);
        // update running time before element state change
        task->updateRunningTime();
        gst::Element branch(task->getBranch());
        if (gst_element_set_state (GST_ELEMENT(branch.getElement()),
                                   GST_STATE_NULL) == GST_STATE_CHANGE_FAILURE)
        {
            throw Exception(__FILE__, __LINE__,
                            "gst_element_set_state() was failed");
        }
        // FIXME!!! add unlink
        //gst_element_unlink (m_tee, branch.getElement());
        gst::Element pipeline(getPipeline());
        if (!gst_bin_remove (GST_BIN (pipeline.getElement()), branch.getElement()))
        {
            throw Exception(__FILE__, __LINE__,
                            "gst_bin_remove() was failed");
        }

        // remove the task
        m_storage.erase(find);
    }
}

// Is there any tasks
bool Pipeline::empty() const  throw()
{
    Locker lock(&m_storage_lock);
    return m_storage.empty();
}

// Pauses the pipeline
void Pipeline::pause()
{
    // move it to paused state
    pausePipeline();
}

// Starts playing the pipeline
void Pipeline::play()
{
    // move pipeline to PLAYING state
    playPipeline();
}

//  Process stage event
void Pipeline::processStageChange()
{
    Locker lock(&m_storage_lock);
    // update running time counter for all running tasks
    std::for_each(m_storage.begin(), m_storage.end(),
                  boost::bind(&Task::updateRunningTime, _1));
}
