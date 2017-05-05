/**
   @file service.h
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/11/13 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SERVICE_H
#define KLK_SERVICE_H

#include <string>

#include "moduledb.h"
#include "appinfo.h"
#include "modulescheduler.h"

namespace klk
{
    namespace srv
    {
        /**
           @brief The system class implementation

           The system info holder class. The module provides info
           about running modules and applications.

           It starts applications in separate processes and monitors
           their states. If an application got a signal
           (SIGSEGV, SIGABRT, SIGBUS) it will
           be restarted.

           For the application monitor a thread pool is created. One thread
           for an application.

           @ingroup grSysModule
        */
        class Service : public ModuleWithDB
        {
            friend class TestService;
        public:
            /**
               Constructor

               @param[in] factory the module factory
            */
            Service(IFactory *factory);

            /**
               Destructor
            */
            virtual ~Service();

            /**
               Gets total CPU usage

               @return the value
            */
            const double getTotalCPUUsage();

            /**
               Gets total Memory usage

               @return the value
            */
            const long getTotalMemUsage() const;

            /**
               Retrives application info by its module uuid

               @param[in] mod_id - the application module uuid to be used for
               the info retrival

               @return the pointer to the info contatiner

               @note used in @ref grSysModuleCLI "CLI" and @ref  grSysModuleTest "utests"
            */
            const AppInfoPtr getAppInfo(const std::string& mod_id) const;
        private:
            AppInfoStorage m_apps; ///< applications
            Usage m_global_usage; ///< global usage container
            double m_cpu_usage; ///< last stored CPU usage

            /**
               Do some actions after main loop
            */
            virtual void postMainLoop() throw();

            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw();

            /// @copydoc klk::IModule::registerProcessors()
            virtual void registerProcessors();

            /**
               Process changes at the DB
               For the system module it just starts applications

               @param[in] msg - the input message

               @exception @ref klk::Exception
            */
            virtual void processDB(const IMessagePtr& msg);

            /**
               @copydoc IModule::getType
            */
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Do some actions before main loop

               @exception @ref klk::Exception
            */
            virtual void preMainLoop();

            /**
               Stops applications for the daemon
            */
            void stopApplications();

            /**
               Gets application list

               @return the application list

               @exception klk::Exception
            */
            AppInfoList getApplications();

            /**
               Starts an application

               @param[in] info - the info about application to be loaded
            */
            void startApplication(const AppInfoPtr& info) throw();

            /**
               Stops an application

               @param[in] info - the info about application to be stopped
            */
            void stopApplication(const AppInfoPtr& info) throw();

            /**
               Checks is the module loaded or not

               @param[in] in - the input message
               @param[out] out - the output message

               @exception @ref klk::Exception
            */
            void checkModule(const IMessagePtr& in,
                             const IMessagePtr& out);

            /// Updates global CPU usage
            void updateCPUUsage();

            /**
               @brief Main application thread loop

               The application is started inside the thread.

               @param[in] info - the application info container
             */
            void doApplicationMonitor(const AppInfoPtr& info);

            /**
               Updates application cpu usage with info gotten in the message

               @param[in] in - the input message with CPU usage info

               @see klk::launcher::Module::sendCPUUsageInfo()

               @exception @ref klk::Exception
            */
            void updateCPUUsage4App(const IMessagePtr& in);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Service(const Service& value);

            /**
               Assignment opearator
            */
            Service& operator=(const Service&);
        };

        /**
           Smart pointer
        */
        typedef boost::shared_ptr<Service> ServicePtr;
    }
}

#endif //KLK_SERVICE_H

