/**
   @file resourcecommand.h
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
   - 2009/01/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_RESOURCECOMMAND_H
#define KLK_RESOURCECOMMAND_H

#include "cli.h"
#include "stringlist.h"

namespace klk
{
    namespace dvb
    {
        /**
           DVB sources set command id
        */
        const std::string
            SETSOURCE_COMMAND_ID("a712a22f-c0f6-4b64-a1a4-5a398f8617a3");

        /**
           Resources command name
        */
        const std::string
            SETSOURCE_COMMAND_NAME("set source");

        /**
           @brief Sets DVB card source

           Sets DVB card source
        */
        class SetSourceCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            SetSourceCommand();

            /**
               Destructor
            */
            virtual ~SetSourceCommand(){}
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return SETSOURCE_COMMAND_ID;
            }

            /**
               Process the command

               @param[in] params - the input parameters

               @return the result of processing in the form of string
               to be sent back to the CLI client

               @exception @ref klk::Exception
            */
            virtual const std::string
                process(const cli::ParameterVector& params);

            /**
               Is the command require module be loaded or not

               @return
               - false - the module can be unloaded
            */
            virtual bool isRequireModule() const throw() {return false;}

            /**
               @copydoc cli::ICommand::getCompletion
            */
            virtual const cli::ParameterVector
                getCompletion(const cli::ParameterVector& setparams);

            /**
               Retrives a list with possible source names for the specified dev

               @param[in] dev_name - the dev name
            */
            const StringList getSourceNames(const std::string& dev_name);

            /**
               Retrives a list with possible dev names
            */
            const StringList getDevNames();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SetSourceCommand(const SetSourceCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SetSourceCommand& operator=(const SetSourceCommand& value);
        };
    }
}

#endif //KLK_RESOURCECOMMAND_H
