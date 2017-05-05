/**
   @file appinfo.h
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

#ifndef KLK_APPINFO_H
#define KLK_APPINFO_H

#include <unistd.h>

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "thread.h"

namespace klk
{
    namespace srv
    {
        /** @addtogroup grSysModule
            @{
        */

        /// Thread smart pointer
        typedef boost::shared_ptr<boost::thread> ThreadPtr;

        /**
           @brief Application info container

           Application info container
        */
        class AppInfo
        {
        public:
            /**
               Constructor

               @param[in] app - the application uuid
               @param[in] mod - the module uuid
               @param[in] appname - the application name
            */
            AppInfo(const std::string& app,
                    const std::string& mod,
                    const std::string& appname);

            /**
               Destructor
            */
            virtual ~AppInfo(){}

            /**
               Retrive application uuid

               @return the uuid
            */
            const std::string getAppUUID() const throw(){return m_app;}

            /**
               Retrive module uuid

               @return the uuid
            */
            const std::string getModUUID() const throw(){return m_mod;}

            /**
               Retrives application name

               @return the application name
            */
            const std::string getName() const throw(){return m_appname;}

            /**
               Retrives the application process pid

               @return the pid
            */
            pid_t getPid() const throw();

            /**
               Sets the application process pid

               @param[in] pid - the pid to be set
            */
            void setPid(pid_t pid);

            /**
               Sets thread that monitors the application

               @param[in] thread - the thread to be set
            */
            void setThread(const ThreadPtr& thread);

            /// @return the thread that monitors the application
            const ThreadPtr getThread() const
            {
                Locker lock(&m_mutex);
                return m_thread;
            }

            /// @return the application uptime in seconds
            time_t  getUptime() const;

            /**
               Sets CPU usage

               @param[in] cpu_usage - the info to be set
            */
            void setCPUUsage(const  std::string& cpu_usage);

            /// @return cpu usage info
            const std::string getCPUUsage() const;
        private:
            mutable Mutex m_mutex; ///< mutex for locking
            const std::string m_app; ///< application uuid
            const std::string m_mod; ///< module uuid
            const std::string m_appname; ///< the application name
            pid_t m_pid; ///< the application pid
            ThreadPtr m_thread; ///< the thread that monitors the application
            time_t m_start_time; ///< the application start time
            std::string m_cpu_usage; ///< cpu usage info
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            AppInfo(const AppInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            AppInfo& operator=(const AppInfo& value);
        };

        /// Application info smart pointer
        typedef boost::shared_ptr<AppInfo> AppInfoPtr;

        /// Application list
        typedef std::list<AppInfoPtr> AppInfoList;

        /**
           @brief AppInfo holder

           There is a thread-safe list with application info
        */
        class AppInfoStorage
        {
        public:
            /// Constructor
            AppInfoStorage();

            /// Destructor
            virtual ~AppInfoStorage(){}

            /**
               Adds an info to the list

               @param[in] info - the info to be added
            */
            void add(const AppInfoPtr& info);

            /**
               Removes an info from the list

               @param[in] info - the info to be removed
            */
            void remove(const AppInfoPtr& info);

            /**
               @brief Retrives missing application

               It returns a list with application present at the storage
               but missing at the list specified at the argument

               @param[in] list - the list to be compared

               @return the result list
            */
            const AppInfoList getMissing(const AppInfoList& list) const;

            /// @return all applications from the storage
            const AppInfoList getAll() const
            {
                Locker lock(&m_mutex);
                return m_storage;
            }

            /**
               Find by app info

               @param[in] info - the info to be search

               @return
               - true - the app was found in the list
               - false - the app was not found in the list
            */
            bool find(const AppInfoPtr& info) const;

            /**
               @return the size of the storage

               @note FIXME!!! used in utests only
            */
            size_t size() const
            {
                Locker lock(&m_mutex);
                return m_storage.size();
            }
        private:
            mutable Mutex m_mutex; ///< mutex for locking
            AppInfoList m_storage; ///< the storage
        private:
            /// Fake copy constructor
            AppInfoStorage(const AppInfoStorage&);

            /// Fake assigment operator
            AppInfoStorage& operator=(const AppInfoStorage&);
        };

        /** @} */
    }
}

#endif //KLK_APPINFO_H
