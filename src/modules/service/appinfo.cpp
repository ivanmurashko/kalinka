/**
   @file appinfo.cpp
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
   - 2009/03/17 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "appinfo.h"
#include "exception.h"

using namespace klk;
using namespace klk::srv;

//
// AppInfo class
//

// Constructor
AppInfo::AppInfo(const std::string& app,
                 const std::string& mod,
                 const std::string& appname) :
    m_mutex(), m_app(app), m_mod(mod), m_appname(appname), m_pid(0),
    m_thread(), m_start_time(0), m_cpu_usage("0.0%")
{
}

//
// Application finders
//

// Retrives the application process pid
pid_t AppInfo::getPid() const throw()
{
    Locker lock(&m_mutex);
    return m_pid;
}

// Sets the application process pid
void AppInfo::setPid(pid_t pid)
{
    Locker lock(&m_mutex);
    m_pid = pid;
    m_start_time = time(NULL);
}

// Sets thread that monitors the application
void AppInfo::setThread(const ThreadPtr& thread)
{
    Locker lock(&m_mutex);
    m_thread = thread;
}

// Gets uptime inseconds
time_t AppInfo::getUptime() const
{
    Locker lock(&m_mutex);
    return m_start_time > 0 ? (time(NULL) - m_start_time) : 0;
}

// Sets CPU usage
void AppInfo::setCPUUsage(const  std::string& cpu_usage)
{
    Locker lock(&m_mutex);
    m_cpu_usage = cpu_usage;
}

/// @return cpu usage info
const std::string AppInfo::getCPUUsage() const
{
    Locker lock(&m_mutex);
    return m_cpu_usage;
}

//
// AppInfoStorage class
//

/**
   Functor for search equal application

   Application are equal if their UUIDs are equal
*/
struct IsAppEqual
{
    /**
       Compare func
    */
    bool operator()(const AppInfoPtr& info1, const AppInfoPtr& info2){
        BOOST_ASSERT(info1);
        BOOST_ASSERT(info2);
        return (info1->getAppUUID() == info2->getAppUUID());
    }

    /**
       Compare func for UUID in the string for

       used in our utests
    */
    bool operator()(const AppInfoPtr& info, const std::string& uuid){
        BOOST_ASSERT(info);
        return (info->getAppUUID() == uuid);
    }
};

/// Constructor
AppInfoStorage::AppInfoStorage() : m_mutex(), m_storage()
{
}

// Adds an info to the list
void AppInfoStorage::add(const AppInfoPtr& info)
{
    BOOST_ASSERT(info);
    Locker lock(&m_mutex);

    // check for unique
    AppInfoList::iterator find = std::find_if(
        m_storage.begin(), m_storage.end(),
        boost::bind<bool>(IsAppEqual(), _1, info));
    BOOST_ASSERT(find == m_storage.end());

    m_storage.push_back(info);
}

// Removes an info from the list
void AppInfoStorage::remove(const AppInfoPtr& info)
{
    BOOST_ASSERT(info);
    Locker lock(&m_mutex);

    AppInfoList::iterator find = std::find_if(
        m_storage.begin(), m_storage.end(),
        boost::bind<bool>(IsAppEqual(), _1, info));

    if(find != m_storage.end())
    {
        m_storage.erase(find);
    }
}

// Find by app info
bool AppInfoStorage::find(const AppInfoPtr& info) const
{
    Locker lock(&m_mutex);

    AppInfoList::const_iterator find = std::find_if(
        m_storage.begin(), m_storage.end(),
        boost::bind<bool>(IsAppEqual(), _1, info));

    return (find != m_storage.end());
}

// Retrives missing application
// It returns a list with application present at the storage
// but missing at the list specified at the argument
const AppInfoList AppInfoStorage::getMissing(const AppInfoList& list) const
{
    // FIXME!!! use code from klk::mod::InfoContainer::getDel

    AppInfoList result;
    Locker lock(&m_mutex);
    for (AppInfoList::const_iterator item = m_storage.begin();
         item != m_storage.end(); item++)
    {
        AppInfoList::const_iterator find = std::find_if(
            list.begin(), list.end(),
            boost::bind<bool>(IsAppEqual(), _1, *item));
        if (find == list.end())
            result.push_back(*item);
    }

    return result;
}
