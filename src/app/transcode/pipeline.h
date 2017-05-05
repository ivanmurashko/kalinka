/**
   @file pipeline.h
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

#ifndef KLK_PIPELINE_H
#define KLK_PIPELINE_H

#include <list>

#include "transinfo.h"
#include "task.h"
#include "ipipeline.h"
#include "gst/gstthread.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief The pipeline

           The pipeline combines several klk::trans::Task that are
           executed in one gst thread

           The typical pipeline example is the following

           gst-launch filesrc location=test/test.avi ! tee name=t \
           t. ! queue ! filesink location=file.avi \
           t. ! queue ! decodebin name=d \
           ffmux_flv name=mux ! filesink location=file.flv \
           d. ! ffmpegcolorspace ! "video/x-raw-yuv",format=\(fourcc\)I420 ! \
           videoscale ! "video/x-raw-yuv",width=320,height=240 ! \
           ffenc_flv bitrate=600000 ! identity sync=TRUE ! mux. \
           d. ! queue ! audioconvert ! audioresample ! \
           audio/x-raw-int,rate=44100 ! lame ! mp3parse ! mux.

           @ingroup grTrans
        */
        class Pipeline : public gst::Thread, public IPipeline
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory to get all necessary objects
               @param[in] processor - the processor handler
               @param[in] source - the source info
            */
            Pipeline(IFactory* factory, const gst::IProcessorPtr& processor,
                     const SourceInfoPtr& source);

            /**
               Destructor
            */
            virtual ~Pipeline();

            /**
               @copydoc klk::transIPipeline::play
            */
            virtual void play();

            /**
               @copydoc klk::transIPipeline::pause
            */
            virtual void pause();

            /**
               Adds a task

               @param[in] task - the task to be added

               @exception klk::Exception
            */
            void addTask(const TaskPtr& task);

            /**
               Deletes a task

               @param[in] uuid - the uuid of task to be deleted

               @exception klk::Exception
            */
            void delTask(const std::string& uuid);

            /**
               Is there any tasks

               @return
               - true
               -false
            */
            bool empty() const throw();

            /**
               Checks is the source match the specified uuid

               @param[in] uuid - the uuid to be matched

               @return
               - true the source match
               - false the source does not match
            */
            bool sourceMatch(const std::string& uuid) const
            {
                return (m_source->getUUID() == uuid);
            }
        private:
            /**
               Tasks storage
            */
            typedef std::list<TaskPtr> Storage;

            IFactory* const m_factory; ///< factory
            IBranchFactoryPtr m_branch_factory; ///< branch factory
            SourceInfoPtr m_source; ///< source
            mutable Mutex m_storage_lock; ///< storage lock
            Storage m_storage; ///< storage for tasks
            GstElement* m_tee; ///< tee element

            /**
               Do init before startup

               @exception klk::Exception - the init failed
            */
            virtual void init();

            /**
               Do cleanup
            */
            virtual void clean() throw();

            /**
               Initializes a task for streaming

               @param[in] task - the task to be initialized

               @exception klk::Exception
            */
            void initTask(const TaskPtr& task);

            /**
               Process stage event

               It updates running times
            */
            virtual void processStageChange();

            /**
               Finds a task

               @param[in] uuid - the uuid of task to be found

               @note not thread safe method

               @return the iterator in the storage
            */
            Storage::iterator findTask(const std::string& uuid);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Pipeline(const Pipeline& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Pipeline& operator=(const Pipeline& value);
        };
    }
}

#endif //KLK_PIPELINE_H
