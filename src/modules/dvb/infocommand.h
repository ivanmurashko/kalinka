/**
   @file infocommand.h
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
   - 2009/01/17 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_INFOCOMMAND_H
#define KLK_INFOCOMMAND_H

#include "basecommand.h"

namespace klk
{
    namespace dvb
    {
        /**
           Info command id
        */
        const std::string INFO_COMMAND_ID =
            "f9d3721a-48fc-4d3f-835b-c232c27290e7";

        /**
           Info command name
        */
        const std::string INFONAME = "status";

        /**
           @brief Retrives info about the mediaserver

           Retrives info about the mediaserver for CLI

           Lists all dvb cards and status information.
           When adapter name is given, the selected adapter is shown.
           all lists all available adapters.
        */
        class InfoCommand : public BaseCommand
        {
        public:
            /**
               Constructor
            */
            InfoCommand();

            /**
               Destructor
            */
            virtual ~InfoCommand(){}

        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return INFO_COMMAND_ID;
            }

            /**
               Process the command

               @param[in] params - the input parameters

               @return the result of processing in the form of string
               to be sent back to the CLI client

               @exception @ref Result
            */
            virtual const std::string process(
                const cli::ParameterVector& params);

            /**
               Retrives info about the specified dev

               @param[in] name - the dev's name
               @param[out] result - the result collector


               @exception @ref klk::Exception
            */
            void getInfo(const std::string& name, std::string* result);

            /**
               Retrives specified param info for a dev

               @param[in] param - the dev's parameter
               @param[in] dev - the dev

               @return the info about the param (it's value)

               @exception @ref klk::Exception
            */
            const std::string getParamInfo(const std::string& param,
                                           const IDevPtr& dev) const;

            /**
               @copydoc cli::ICommand::getCompletion
            */
            virtual const cli::ParameterVector
                getCompletion(const cli::ParameterVector& setparams);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            InfoCommand(const InfoCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InfoCommand& operator=(const InfoCommand& value);
        };
    }
}

#endif //KLK_INFOCOMMAND_H
