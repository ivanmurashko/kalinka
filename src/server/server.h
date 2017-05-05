/**
   @file server.h
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
   - 2008/07/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SERVER_H
#define KLK_SERVER_H

#include "thread.h"
#include "ifactory.h"

namespace klk
{
    namespace server
    {
        /** @defgroup grServer Mediaserver appl.

            Mediaserver daemon description
            @{
        */
        /**
           @brief Mediaserver main class

           Mediaserver main class
        */
        class ServerDaemon
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory for getting all objects
            */
            explicit ServerDaemon(IFactory* factory);

            /**
               Destructor
            */
            virtual ~ServerDaemon();

            /**
               Starts the server

               Perform main loop

               @exception klk::Exception
            */
            void start();
        private:
            IFactory * const m_factory; ///< modules factory
            int m_pid; ///< daemon pid
            int m_lock; ///< lock fd

            /**
               Do startup

               @exception klk::Exception
            */
            void doStartup();

            /**
               Perform cleaning actions
            */
            void doStop() throw();


            /**
               Locks the daemon

               @exception klk::Exception
            */
            void lockDaemon();

            /**
               Unlocks the daemon
            */
            void unlockDaemon() throw();

            /**
               Starts modules for the daemon

               @exception klk::Exception
            */
            void startModules();

            /**
               Stops modules for the daemon
            */
            void stopModules() throw();
        private:
            /**
               Constructor
            */
            ServerDaemon();

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ServerDaemon(const ServerDaemon& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ServerDaemon& operator=(const ServerDaemon& value);
        };
        /** @} */
    }
}

#endif //KLK_SERVER_H
