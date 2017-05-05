/**
   @file client.h
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
   - 2007/06/30 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CLIENT_H
#define KLK_CLIENT_H

#include <string>
#include <list>

#include "common.h"
#include "clidriver.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "histedit.h"
#ifdef __cplusplus
}
#endif

#include "ifactory.h"

namespace klk
{
    namespace cli
    {
        /** @defgroup grClient CLI client appl. for Kalinka Mediaserver

            Mediaserver CLI (command-line interface) client description
            @{
        */
        /**
           @brief The client application

           The client application
        */
        class Client
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            explicit Client(IFactory* factory);

            /**
               Destructor
            */
            virtual ~Client();

            /**
               Starts the application

               @exception klk::Exception
            */
            void start();

            /**
               Stops the application
            */
            void stop();

            /**
               Processes the start command

               @param[in] fd - the file descriptor for output messages
               @param[in] argc - TBD
               @param[in] argv - TBD

               @exception @ref klk::Exception
            */
            void handleStart(int fd, int argc, char *argv[]);

            /**
               Processes the stop command

               @param[in] fd - the file descriptor for output messages
               @param[in] argc - TBD
               @param[in] argv - TBD

               @exception @ref klk::Exception
            */
            void handleStop(int fd, int argc, char *argv[]);
        private:
            IFactory* const m_factory; ///< factory object
            Tokenizer *m_tok; ///< a clie object
            History *m_hist; ///< a cli object

            /**
               Tests connection

               @return
               - @ref klk::OK - the server respond is OK
               - @ref klk::ERROR - the server does not respond
            */
            klk::Result testConnection();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Client(const Client& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Client& operator=(const Client& value);
        };
        /** @} */
    }
}

#endif //KLK_CLIENT_H
