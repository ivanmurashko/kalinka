/**
   @file transscheduler.cpp
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
   - 2009/10/03 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>
#include <boost/assert.hpp>

#include "transscheduler.h"
#include "processor.h"
#include "log.h"

using namespace klk;
using namespace klk::trans;

//
// Scheduler class
//

// Constructor
Scheduler::Scheduler(IFactory* factory) :
    base::Scheduler(), m_factory(factory), m_pipelines(), m_storage_lock(),
    m_processor()
{
    BOOST_ASSERT(m_factory);
}

// Destructor
Scheduler::~Scheduler()
{
    // we should stop all actively running threads in other case we will have
    // threads that are used data that are cleared at the destructor
    KLKASSERT(m_pipelines.empty());
}

// Inits scheduler before startup
void Scheduler::init()
{
    ProcessorThreadPtr processor(new ProcessorThread(this));
    // start the thread
    startThread(processor);
    // set it as processor
    Locker lock(&m_storage_lock);
    m_processor = processor;
}

// Adds a task
void Scheduler::addTask(const TaskPtr& task)
{
    const std::string uuid = task->getSourceUUID();
    PipelinePtr pipeline;
    {
        Locker lock(&m_storage_lock);
        Storage::iterator find = m_pipelines.find(uuid);
        if (find == m_pipelines.end())
        {
            pipeline = PipelinePtr(new Pipeline(m_factory, m_processor,
                                                task->getSource()));
            m_pipelines.insert(Storage::value_type(uuid, pipeline));
        }
        else
        {
            pipeline = find->second;
        }
    }
    BOOST_ASSERT(pipeline);
    pipeline->addTask(task);
}

// Deletes a task
void Scheduler::delTask(const TaskInfoPtr& task)
{
    PipelinePtr pipeline;
    const std::string
        pipeline_uuid = task->getSource()->getUUID(),
        task_uuid = task->getUUID();
    {
        Locker lock(&m_storage_lock);
        Storage::iterator find = m_pipelines.find(pipeline_uuid);
        if (find == m_pipelines.end())
        {
            // nothing todo else
            return;
        }
        else
        {
            pipeline = find->second;
        }
    }
    BOOST_ASSERT(pipeline);
    pipeline->delTask(task_uuid);
    if (pipeline->empty())
    {
        stopPipeline(pipeline_uuid);
    }
}

// Pause pipelines for the specified tasks
void Scheduler::pausePipelines(const TaskInfoList& tasks) throw()
{
    PipelineList list = getPipelines(tasks);
    std::for_each(list.begin(), list.end(),
                  boost::bind(&Scheduler::pausePipeline, this, _1));
}

// Play pipelines for the specified tasks
void Scheduler::playPipelines(const TaskInfoList& tasks) throw()
{
    PipelineList list = getPipelines(tasks);
    std::for_each(list.begin(), list.end(),
                  boost::bind(&Scheduler::playPipeline, this, _1));
}

// Play the specified pipeline. It will move it to PLAYING state
// if the pipeline's thread has been started or start the thread.
void Scheduler::playPipeline(const PipelinePtr& pipeline) throw()
{
    try
    {
        if (isStarted(pipeline))
        {
            // move pipeline to PLAYING state
            pipeline->play();
        }
        else
        {
            // start the thread
            startThread(pipeline);
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Failed to play a pipeline");
    }
}

// Pauses all task in the specified pipeline
void Scheduler::pausePipeline(const PipelinePtr& pipeline) throw()
{
    try
    {
        if (isStarted(pipeline))
        {
            // move pipeline to PLAYING state
            pipeline->pause();
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Failed to pause a pipeline");
    }
}


// Retrives pipelines for specified tasks
const Scheduler::PipelineList
Scheduler::getPipelines(const TaskInfoList& tasks) const throw()
{
    PipelineList list;
    try
    {
        for (TaskInfoList::const_iterator i = tasks.begin();
             i != tasks.end(); i++)
        {
            const std::string uuid = (*i)->getSource()->getUUID();
            Locker lock(&m_storage_lock);
            Storage::const_iterator find1 = m_pipelines.find(uuid);
            if (find1 != m_pipelines.end())
            {
                // check it at the result list
                PipelineList::iterator find2 =
                    std::find_if(
                        list.begin(), list.end(),
                        boost::bind<bool>(&Pipeline::sourceMatch, _1, uuid));
                if (find2 == list.end())
                {
                    list.push_back(find1->second);
                }
            }
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Error while retriving pipelines list");
    }

    return list;
}

// Stops pipeline by its uuid
void Scheduler::stopPipeline(const std::string& uuid)
{
    PipelinePtr pipeline = getPipeline(uuid);
    if (pipeline)
    {
        // stop thread
        stopThread(pipeline);
        // erase it from the storage
        Locker lock(&m_storage_lock);
        m_pipelines.erase(uuid);
    }
}

// Processes stop event
void Scheduler::processStopEvent(const gst::IEventPtr& event)
{
    klk_log(KLKLOG_DEBUG, "Processing stop event at transcode application scheduler");
    BOOST_ASSERT(event);
    stopPipeline(event->getThreadUUID());
}

// Retrives a pipeline by its uuid
const Scheduler::PipelinePtr
Scheduler::getPipeline(const std::string& uuid) const
{
    PipelinePtr pipeline;
    Locker lock(&m_storage_lock);
    Storage::const_iterator find = m_pipelines.find(uuid);
    if (find != m_pipelines.end())
    {
        pipeline = find->second;
    }
    return pipeline;
}

// stops all threads
void Scheduler::stop() throw()
{
    // stop all running pipelines
    {
        Locker lock(&m_storage_lock);
        for (Storage::iterator pipeline = m_pipelines.begin();
             pipeline != m_pipelines.end(); pipeline++)
        {
            try
            {
                stopThread(pipeline->second);
            }
            catch(...)
            {
                klk_log(KLKLOG_ERROR, "There was an error while stopping a GST pipeline");
            }

        }
        m_pipelines.clear();
    }

    base::Scheduler::stop();
}

