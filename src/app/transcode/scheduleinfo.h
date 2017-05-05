/**
   @file scheduleinfo.h
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
   - 2009/12/27 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SCHEDULEINFO_H
#define KLK_SCHEDULEINFO_H

#include <string>
#include <gst/gst.h>

namespace klk
{
    namespace trans
    {
        /** @defgroup grTransScheduledPlayback Scheduled playback

            There is an info about scheduled playback functionality available
            at the mediaserver. The info consists of 2 part

            - Start time is set in the form very similar to crontab(5)
            - Duration - the duration itself

            @ingroup grTrans

            @{
        */

        /**
           @brief The crontab file format parser

             From crontab(5)

              field                  allowed values
              -----                    --------------
              minute              0-59
              hour                  0-23
              day of month   1-31
              month               1-12 (or names, see below)
              day of week     0-7 (0 or 7 is Sun, or use names)

           A   field   may   be   an   asterisk  (*),  which  always  stands  for
          ``first-last''.
        */
        class CrontabParser
        {
            friend class TestScheduleInfo;
        public:
            /**
               Constructor

               @param[in] info - the crontab info
            */
            CrontabParser(const std::string& info);

            /**
               Destructor
            */
            virtual ~CrontabParser(){}
        protected:
            /**
               Is the time match to be starting processing

               @param[in] checktime - is the time to be checked.
               If the arg is NULL the localtime_r(3) will be taken

               @return
               - true - the time is match and processing should be started
               - true - the time is not match and processing
               should not be started
            */
            bool isMatch(const struct tm* checktime = NULL) const;
        private:
            std::string m_minute; ///< minute field from crontab(5)
            std::string m_hour; ///< hour field from crontab(5)
            std::string m_day_of_month; ///< day of month field from crontab(5)
            std::string m_month; ///< month field from crontab(5)
            std::string m_day_of_week; ///< day of week field from crontab(5)

            /**
               Inits from crontab(5) info

               @param[in] info - the init info

               @exception klk::Exception if the info arg has an incorrect format
            */
            void init(const std::string& info);

            /**
               Checks the string pattern to the numerical value

               @param[in] pattern - the pattern to be used
               @param[in] val - the value to be checked

               @return
               - true - the val parameter does match the pattern
               - false - the val parameter does match the pattern
            */
            static bool check(const std::string& pattern, int val);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            CrontabParser(const CrontabParser& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            CrontabParser& operator=(const CrontabParser& value);
        };

        /**
           @brief The schedule info container

           The class keeps info about scheduled playback
        */
        class ScheduleInfo : public CrontabParser
        {
        public:
            /**
               Constructor

               @param[in] start - the startup info in the form of equal
               to crontab(5)
               @param[in] duration - the duration of the task
            */
            ScheduleInfo(const std::string& start, time_t duration);

            /**
               Destructor
            */
            virtual ~ScheduleInfo(){}

            /**
               Is the task expired

               @return
               - true - the task was expired
               - false - the task has not been expired yet
            */
            bool isExperied() const;

            /**
               Should be started or not

               @return
               - true - the task should be started
               - false - the task has not to be started
               (started already or does not match the shceduled
               play back settings)
            */
            bool needStart() const;
        protected:
            /**
               Retrives actual duration

               @return the stream duration in nanoseconds
            */
            virtual GstClockTime getActualDuration() const = 0;

            /**
               Retruns a counter how many times it was started

               @return the counter
            */
            virtual u_int getRunningCount() const throw()  = 0;
        private:
            const bool m_once; ///< should be played only one time (true/false)?
            const GstClockTime m_duration; ///< duration for the task
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ScheduleInfo(const ScheduleInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ScheduleInfo& operator=(const ScheduleInfo& value);
        };

        /** @} */
    }
}

#endif //KLK_SCHEDULEINFO_H
