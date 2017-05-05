/**
   @file installer.h
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
   - 2009/03/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_INSTALLER_H
#define KLK_INSTALLER_H

#include <string>

#include "ifactory.h"

namespace klk
{
    namespace inst
    {

        /** @defgroup grInstall Installer appl. for Kalinka Mediaserver

            Mediaserver installer. There is a simple application that is used to to installation.
            The installation includes the following steps:
            - Setup config data for configuration file located at klk::dir::CFG
            - Detect resources
            - Set adapter host name. The @ref grAdapter "adapter module" is used to provide
            an access to the system managment via @ref grCLI "CLI"

            @ingroup grApp

            @{
        */

        /**
           @brief The installer class

           The installer
        */
        class Installer
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            Installer(IFactory* factory);

            /**
               Destructor
            */
            virtual ~Installer();

            /**
               Processes interactive option

               @exception klk::Exception
            */
            void processInteractive();

            /**
               Sets an option

               @param[in] name - the option name
               @param[in] value - the option value

               @exception klk::Exception
            */
            void setOption(const std::string& name, const std::string& value);

            /**
               Detects resources

               @exception klk::Exception
            */
            void detectResources();
        private:
            IFactory* const m_factory; ///< the factory

            /**
               Recreates DB

               @exception klk::Exception
            */
            void recreateDB();

            /**
               Asks a question

               @param[in] prompt - the prompt

               @return
               - true - the answer was yes
               - false - the answer was no

               @exception klk::Exception
            */
            bool ask(const std::string& prompt);

            /**
               Gets a value

               @param[in] prompt - the prompt

               @return the value

               @exception klk::Exception
            */
            const std::string get(const std::string& prompt);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Installer(const Installer& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Installer& operator=(const Installer& value);
        };

        /** @} */

    }
}

#endif //KLK_INSTALLER_H
