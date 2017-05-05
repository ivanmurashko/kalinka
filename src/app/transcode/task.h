/**
   @file task.h
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

#ifndef KLK_TASK_H
#define KLK_TASK_H

#include "transinfo.h"
#include "ifactory.h"
#include "gst/gstthread.h"

#include "media.h"

namespace klk
{
    namespace trans
    {
        class Transcode; //FIXME!!! ticket #200 (need for src init)

        /**
           @brief The wrapper for klk::trans::TaskInfo

           The wrapper incapsulates task initialization/deinitialization
           operations and provides an access to necessary Task operation

           @ingroup grTransInfo
        */
        class Task
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory instance
               @param[in] module - the klk::trans::Transcode module instance
               @param[in] task - the task info holder that
               will be stored at the wrapper

               FIXME!!! the module param is used for task initialization
               may be it will be possible to remove it from here. If we will
               have a possibility to send sync messages directly from any place

               see ticket #200
            */
            Task(IFactory* factory, Transcode* module,
                 const TaskInfoPtr& task);

            /**
               Destructor
            */
            ~Task();

            /**
               Retrives the task uuid

               @return the task's uuid
            */
            const std::string getUUID(){return m_task_info->getUUID();}

            /**
               @brief Retrives the task's source uuid

               All tasks combined into groups all task in a group
               have common source. Thus we can identify a group
               by the source uuid

               @return the task's uuid
            */
            const std::string getSourceUUID()
            {
                return m_task_info->getSource()->getUUID();
            }

            /**
               Retrives source for klk::trans::Pipepile initialization

               @return the pointer to the source for the task
            */
            const SourceInfoPtr getSource(){return m_task_info->getSource();}

            /**
               Retrives media type (for destination)

               @return the type uuid
            */
            const std::string getMediaType() const
            {
                if (m_task_info->getSource()->getMediaType() ==
                     m_task_info->getDestination()->getMediaType())
                {
                    // no need to transcode because source and destination are same
                    return klk::media::EMPTY;
                }

                if ( m_task_info->getDestination()->getMediaType() == klk::media::EMPTY )
                {
                    // no need to recode
                    return klk::media::EMPTY;
                }

                if ( m_task_info->getDestination()->getMediaType() == klk::media::AUTO )
                {
                    // no need to recode
                    return klk::media::EMPTY;
                }

                return m_task_info->getDestination()->getMediaType();
            }

            /**
               Retrives video quality info

               @return a pointer to the info
            */
            const quality::VideoPtr getVideoQuality() const
            {
                return m_task_info->getVideoQuality();
            }


            /**
               Retrives destination element

               @return the pointer to the destination GST element
            */
            GstElement* getDestinationElement()
            {
                return m_task_info->getDestination()->getElement();
            }

            /**
               Updates running time

               There is a time in which the task was in PLAYING state
            */
            void updateRunningTime();

            /**
               Retrives the branch (pipeline for transcoding)
               relevant for the task

               @exception klk::Exception

               @return the smart pointer to gst element  gst element
            */
            gst::Element getBranch();

            /**
               Sets the tee's branch for the task

               @param[in] branch - the value to be set
            */
            void setBranch(GstElement* branch) throw();

            /**
               Sets branch in the paused state

               @exception klk::Exception
            */
            void setPause();
        private:
            mutable Mutex m_lock; ///< mutext for thread sync
            IFactory* m_factory; ///< factory instance
            const TaskInfoPtr m_task_info; ///< the task info holder
            GstElement* m_branch; ///< tee's branch for the task
            /**
               running time before the GST element was set into play state
               it should be combined with getRunningTime()
               to get actual duration
            */
            GstClockTime m_running_time;

            /**
               Retrives actual task duration

               @return the time in ns since the task startup
            */
            GstClockTime getActualDuration();

            /**
               Retrives running time since the branch element
               was last time set to PAUSED

               @param[in] branch - the branch element

               @return the running time
            */
            static GstClockTime getRunningTime(GstElement* branch);

            /**
               Do the base resources deinitalization
            */
            void deinit() throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Task(const Task& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Task& operator=(const Task& value);
        };

        /**
           The task infor wrapper smart pointer

           @ingroup grTransInfo
        */
        typedef boost::shared_ptr<Task> TaskPtr;

    }
}

#endif //KLK_TASK_H
