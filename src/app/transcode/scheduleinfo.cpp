/**
   @file scheduleinfo.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>

#include <map>

#include <boost/lexical_cast.hpp>
#include <boost/assert.hpp>

#include "scheduleinfo.h"
#include "exception.h"
#include "defines.h"
#include "utils.h"

using namespace klk;
using namespace klk::trans;

//
// CrontabParser class
//

// Constructor
CrontabParser::CrontabParser(const std::string& info)
{
    if (info != schedule::REBOOT)
    {
        /**
           Aliases map
        */
        typedef std::map<std::string, std::string> AliasMap;

/*
              string         meaning
              ------         -------
              @reboot        Run once, at startup.
              @yearly        Run once a year, "0 0 1 1 *".
              @annually      (same as @yearly)
              @monthly       Run once a month, "0 0 1 * *".
              @weekly        Run once a week, "0 0 * * 0".
              @daily         Run once a day, "0 0 * * *".
              @midnight      (same as @daily)
              @hourly        Run once an hour, "0 * * * *".
*/

        AliasMap ALIASES;
        ALIASES[schedule::ALWAYS] = "* * * * *";
        ALIASES[schedule::YEARLY] = "0 0 1 1 *";
        ALIASES[schedule::ANNUALLY] = ALIASES[schedule::YEARLY];
        ALIASES[schedule::MONTHLY] = "0 0 1 * *";
        ALIASES[schedule::WEEKLY] = "0 0 * * 0";
        ALIASES[schedule::DAILY] = "0 0 * * *";
        ALIASES[schedule::MIDNIGHT] = ALIASES[schedule::DAILY];
        ALIASES[schedule::HOURLY] = "0 * * * *";

        AliasMap::iterator find = ALIASES.find(info);
        if (find != ALIASES.end())
        {
            // init from alias
            init(find->second);
        }
        else
        {
            // not an alias
            init(info);
        }
    }
}

// Inits from crontab(5) info
void CrontabParser::init(const std::string& info)
{
    std::vector<std::string> tmpcontainer = base::Utils::split(info, " \t\r\n");
    if (tmpcontainer.size() != 5)
    {
        throw Exception(__FILE__, __LINE__, "Incorrect format for crontab: %s. "
                        "There are should be 5 fields.", info.c_str());
    }
    m_minute             = tmpcontainer[0];
    if (m_minute != "*")
    {
        // it should be a number 0-59
        int minute = boost::lexical_cast<int>(m_minute);
        if (minute < 0 || minute > 59)
        {
            throw Exception(__FILE__, __LINE__, "Invalid minute field: " + m_minute +
                ". Should be a number in the following interval [0-59] or asterisk (*)");
        }
    }

    m_hour                 = tmpcontainer[1];
    if (m_hour != "*")
    {
        // it should be a number 0-23
        int hour = boost::lexical_cast<int>(m_hour);
        if (hour < 0 || hour > 23)
        {
            throw Exception(__FILE__, __LINE__, "Invalid hour field: " + m_hour +
                ". Should be a number in the following interval [0-23]  or asterisk (*)");
        }
    }

    m_day_of_month = tmpcontainer[2];
    if (m_day_of_month != "*")
    {
        // it should be a number 0-23
        int day_of_month = boost::lexical_cast<int>(m_day_of_month);
        if (day_of_month < 1 || day_of_month > 31)
        {
            throw Exception(__FILE__, __LINE__, "Invalid day_of_month field: " +
                            m_day_of_month +
                            ". Should be a number in the following interval [1-31] or asterisk (*)");
        }
    }

    m_month              = tmpcontainer[3];
    if (m_month != "*")
    {
        // it should be a number 0-23
        int month = boost::lexical_cast<int>(m_month);
        if (month < 1 || month > 12)
        {
            throw Exception(__FILE__, __LINE__, "Invalid month field: " + m_month +
                ". Should be a number in the following interval [1-12] or asterisk (*)");
        }
    }

    m_day_of_week   = tmpcontainer[4];
    if (m_day_of_week != "*")
    {
        // it should be a number 0-23
        int day_of_week = boost::lexical_cast<int>(m_day_of_week);
        if (day_of_week < 0 || day_of_week > 7)
        {
            throw Exception(__FILE__, __LINE__, "Invalid day_of_week field: " +
                            m_day_of_week +
                ". Should be a number in the following interval [0-7]  or asterisk (*)");
        }
    }
}


// Is the time match to be starting processing
bool CrontabParser::isMatch(const struct tm* checktime) const
{
    struct tm tmp;
    if (checktime == NULL)
    {
        time_t now = time(NULL);
        checktime = localtime_r(&now, &tmp);
    }
    BOOST_ASSERT(checktime);

    // check it
    if (!check(m_minute, checktime->tm_min))
        return false; /// minute does not match
    if (!check(m_hour, checktime->tm_hour))
        return false; /// minute does not match
    if (!check(m_day_of_month, checktime->tm_mday))
        return false; /// minute does not match
    if (!check(m_month, checktime->tm_mon))
        return false; /// minute does not match
    if (!check(m_day_of_week, checktime->tm_wday))
        return false; /// minute does not match

    // all is match
    return true;
}

// Checks the string pattern to the numerical value
bool CrontabParser::check(const std::string& pattern, int val)
{
    if (pattern == "*")
        return true; // match everything

    try
    {
        if (boost::lexical_cast<int>(pattern) == val)
            return true;
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Incorrect pattern at crontab: %s", pattern.c_str());
    }

    // FIXME!!! add checks for
    // 1,2,4,6
    // 1-7,8
    // etc.

    return false;
}


//
// ScheduleInfo class
//

// Constructor
ScheduleInfo::ScheduleInfo(const std::string& start, time_t duration) :
    CrontabParser(start),
    m_once(start == schedule::REBOOT),
    m_duration(duration * 1000000000ULL /*in nanoseconds*/)
{
}

// Is the task expired
bool ScheduleInfo::isExperied() const
{
    if (m_duration && m_duration < getActualDuration())
    {
        // expired
        return true;
    }

    // not expired yet
    return false;
}

// Should be started or not
bool ScheduleInfo::needStart() const
{
    // processing only one start
    if (m_once)
    {
        if ((getRunningCount() > 0) ||
            (getActualDuration() > 0))
        {
            // the task has been already started
            // or it's playing now
            // we should not start it again
            return false;
        }

        // we need to start it
        return true;
    }

    // processing schedule playback

    // if match and not started yet
    if (isMatch() && (getActualDuration() == 0))
        return true;

    return false;
}
