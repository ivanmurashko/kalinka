/**
   @file launcher.h
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
   - 2010/07/11 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_LAUNCHER_H
#define KLK_LAUNCHER_H

#include <string>

#include "ifactory.h"

namespace klk
{
    namespace launcher
    {
        /** @defgroup grLauncher
            @brief Application launcher

            The application launcher is used to start mediaserver applications
            such as @ref grTrans "Transcode" outside
            @ref grServer "the main server application"

            @ingroup grApp

            @{
        */

        /**
           @brief The launcher class

           Start the application with the specified name
        */
        class Launcher
        {
        public:
            /**
               Constructor

               @param[in] factory - the launcher factory that is used
               to retrive necessary objects
               @param[in] module_id - the application module id to
               be started
            */
            Launcher(IFactory* factory, const std::string& module_id);

            /// Destructor
            virtual ~Launcher(){}

            /**
               @brief starts the application

               @exception klk::Exception
             */
            void start();

            /// stops the launcher
            void stop();
        private:
            IFactory* m_factory; ///< factory instance
            const std::string m_module_id; ///< the application module id

            /**
               @brief Checks RPC

               The methods checks that RPC is available. It checks connections
               to all necessary adapters: messages, modules, resources and dev

               @see @ref grAdapterModuleObjects "Adapter module objects"
             */
            void checkRPC();
        private:
            /// Fake copy constructor
            Launcher(const Launcher&);

            /// Fake assigment operator
            Launcher& operator=(const Launcher&);
        };

        /** @} */
    }
}

#endif //KLK_LAUNCHER_H
