/**
   @file transscheduler.h
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

#ifndef KLK_TRANSSCHEDULER_H
#define KLK_TRANSSCHEDULER_H

#include <map>
#include <string>

#include "pipeline.h"
#include "scheduler.h"
#include "media.h"


namespace klk
{
    namespace trans
    {
        /**
           @brief Container for klk::transPipeline objects

           Container for klk::transPipeline objects

           @ingroup grTrans
        */
        class Scheduler : public base::Scheduler
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            Scheduler(IFactory* factory);

            /**
               Destructor
            */
            virtual ~Scheduler();

            /**
               Processes stop event

               @param[in] event - the event info holder
            */
            void processStopEvent(const gst::IEventPtr& event);

            /**
               Pause pipelines for the specified tasks

               @param[in] tasks - the tasks to be paused
            */
            void pausePipelines(const TaskInfoList& tasks) throw();

            /**
               Play pipelines for the specified tasks

               @param[in] tasks - the tasks to be played
            */
            void playPipelines(const TaskInfoList& tasks) throw();

            /**
               Adds a task

               @param[in] task - the task to be added

               @exception klk::Exception
            */
            void addTask(const TaskPtr& task);

            /**
               Deletes a task

               @param[in] task_uuid - the uuid of task to be deleted

               @exception klk::Exception
            */
            void delTask(const TaskInfoPtr& task);

            /**
               Inits scheduler before startup

               @exception klk::Exception
            */
            void init();

            /**
               @copydoc klk::base::Scheduler::stop
            */
            virtual void stop() throw();
        private:
            /**
               Pipeline smart pointer
            */
            typedef boost::shared_ptr<Pipeline> PipelinePtr;

            /**
               Pipelines storage
            */
            typedef std::map<std::string, PipelinePtr> Storage;

            /**
               Pipelines storage
            */
            typedef std::list<PipelinePtr> PipelineList;

            IFactory* const m_factory; ///< the factory
            Storage m_pipelines; ///< pipelines
            mutable Mutex m_storage_lock; ///< storage lock
            gst::IProcessorPtr m_processor; ///< processor thread

            /**
               Retrives pipelines for specified tasks

               @param[in] tasks - the tasks

               @return the pipelines list
            */
            const PipelineList
                getPipelines(const TaskInfoList& tasks) const throw();

            /**
               Stops pipeline by its uuid
            */
            void stopPipeline(const std::string& uuid);

            /**
               Retrives a pipeline by its uuid

               @param[in] uuid - the pipeline's uuid

               @return the pipeline pointer or NULL
               if there is no such pipeline
            */
            const PipelinePtr getPipeline(const std::string& uuid) const;

            /**
               Play the specified pipeline. It will move it to PLAYING state
               if the pipeline's thread has been started or start the thread.

               @param[in]pipeline - the pipeline to be played
            */
            void playPipeline(const PipelinePtr& pipeline) throw();

            /**
               Pauses all task in the specified pipeline

               @param[in]pipeline - the pipeline to be plaused

               @note will be executed only if the pipeline
               has been started in a separate thread
            */
            void pausePipeline(const PipelinePtr& pipeline) throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Scheduler(const Scheduler& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Scheduler& operator=(const Scheduler& value);
        };
    }
}

#endif //KLK_TRANSSCHEDULER_H
