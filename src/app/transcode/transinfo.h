/**
   @file transinfo.h
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

#ifndef KLK_TRANSINFO_H
#define KLK_TRANSINFO_H

#include <gst/gst.h>

#include <boost/function/function0.hpp>

#include "ifactory.h"
#include "mod/info.h"
#include "thread.h"
#include "scheduleinfo.h"
#include "quality.h"

namespace klk
{
    namespace trans
    {
        class Transcode; //FIXME!!! see ticket #200

        /** @defgroup grTransInfo Transcode application (info)

            The Transcode application (info) storage

            @ingroup grTrans

            @{
        */

        /**
           The data flow direction
        */
        typedef enum
        {
            UNKNOWN = 0,
            SOURCE = 1,
            SINK = 2
        } Direction;

        /**
           @brief Transcode application info about a source

           Source info holder
        */
        class SourceInfo : public mod::Info
        {
        public:
            /**
               Constructor

               @param[in] uuid - the info uuid from DB
               @param[in] name - the info name from DB
               @param[in] media_type - the source media type for instance
               klk::media::MPEGTS
            */
            SourceInfo(const std::string& uuid,
                       const std::string& name,
                       const std::string& media_type);

            /// Destructor
            virtual ~SourceInfo(){}

            /**
               Retrives the media type

               @return the type
            */
            const std::string getMediaType() const throw(){return m_media_type;}

            /**
               Sets direction

               @param[in] direction - the value to be set

               @exception klk::Exception
            */
            void setDirection(const Direction direction);

            /**
               @brief Inits the gst element assosiated with the source

               @param[in] module - the klk::trans::Transcode module instance

               Do nothing if the elemen has been initialized
               before and has SOURCE direction and calls
               klk::trans::SourceInfo::initInternal in other cases

               @exception klk::Exception if the source
               has been initialized before and there is not a SOURCE direction
            */
            void init(Transcode* module);

            /**
               Deinits the gst element assosiated with the source

               @exception klk::Exception
            */
            virtual void deinit() throw();

            /**
               Retrives the element

               @return the pointer to element

               @exception klk::Exception
            */
            GstElement* getElement();

            /**
               Inits element

               @param[in] name - the element's name
               @param[in] factory - the klk::IFactory instance

               @exception klk::Exception
            */
            void initElement(const std::string& name, IFactory* factory);
        protected:
            mutable Mutex m_lock; ///< lock for the element
            GstElement* m_element; ///< the gst element

            /**
               Retrives the direction

               @return the value
            */
            const Direction getDirection() const throw();

            /**
               Inits the gst element assosiated with the source

               @param[in] module - the klk::trans::Transcode module instance

               @exception klk::Exception if the source
               has been initialized before
            */
            virtual void initInternal(Transcode* module) = 0;
        private:
            SafeValue<Direction> m_direction; ///< direction
            const std::string m_media_type; ///< media type
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SourceInfo(const SourceInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SourceInfo& operator=(const SourceInfo& value);
        };

        /**
           Source info smart pointer
        */
        typedef boost::shared_ptr<SourceInfo> SourceInfoPtr;

        /**
           The callback functor to retrive task duration info
        */
        typedef boost::function0<GstClockTime> DurationCallback;

        /**
           Default get duration callback.
        */
        struct DurationCallbackDefault
        {
            /**
               The functor body

               @return 0 as a marker that no
               any task assosiated with the caller
            */
            GstClockTime operator()(void){return 0ULL;}
        };

        /**
           @brief Task info

           Info storage about a task
        */
        class TaskInfo : public mod::Info, public ScheduleInfo
        {
            friend class TestTranscode;
            friend class TestSchedulePlay;
        public:
            /**
               Constructor

               @param[in] uuid - the info uuid from DB
               @param[in] name - the info name from DB
               @param[in] source - the source pointer
               @param[in] destination - the destination pointer
               @param[in] vquality - video quality info
               @param[in] start - the startup info in the form of equal
               to crontab(5)
               @param[in] duration - the duration of the task

               @note FIXME!!! the constructor has a lot of parameters
            */
            TaskInfo(const std::string& uuid, const std::string& name,
                     const SourceInfoPtr& source,
                     const SourceInfoPtr& destination,
                     const quality::VideoPtr& vquality,
                     const std::string& start,
                     time_t duration);

            /**
               Destructor
            */
            virtual ~TaskInfo();

            /**
               Retrives source info

               @return the pointer to the source info
            */
            const SourceInfoPtr getSource() const throw();

            /**
               Retrives destination info

               @return the pointer to the destination info
            */
            const SourceInfoPtr getDestination() const throw();

            /**
               @copydoc klk::trans::ScheduleInfo::getActualDuration
            */
            virtual GstClockTime getActualDuration() const;

            /**
               @copydoc klk::trans::ScheduleInfo::getRunningCount
            */
            virtual u_int getRunningCount() const throw();

            /**
               It adds running time value. The method is called when a task
               was terminated and accepts the duration of the task

               The method should not throw exception because it is called by
               klk::trans::Task::~Task
            */
            void addRunningTime(GstClockTime duration) throw();

            /**
               Retrives total duration for the task include
               finished and actual duration

               @return the time
            */
            GstClockTime getRunningTime() const;

            /**
               Sets duration callback

               @param[in] callback - the callback to be set

               @return
               - klk::OK the callback was set
               - klk::ERROR there was an error while setting the callback

               @note it should not throw exceptions because is called from a
               destructor
            */
            Result setDurationCallBack(DurationCallback callback);

            /**
               @return video quality info
            */
            const quality::VideoPtr getVideoQuality() const
            {
                return m_vquality;
            }
        private:
            const SourceInfoPtr m_source; ///< source
            const SourceInfoPtr m_destination; ///< destination

            const quality::VideoPtr m_vquality; ///< video quality info

            GstClockTime m_running_time; ///< total time for finished tasks
            u_int m_running_count; ///< how many times it was started
            DurationCallback m_get_duration; ///< get duration callback
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TaskInfo(const TaskInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TaskInfo& operator=(const TaskInfo& value);
        };

        /**
           TaskInfo smart pointer
        */
        typedef boost::shared_ptr<TaskInfo> TaskInfoPtr;

        /**
           TaskInfo list
        */
        typedef std::list<TaskInfoPtr> TaskInfoList;

        /** @} */
    }
}

#endif //KLK_TRANSINFO_H
