/**
   @file showcmd.h
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
   - 2009/07/05 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SHOWCMD_H
#define KLK_SHOWCMD_H

#include "cli.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {
            /**
               Station show command id
            */
            const std::string STATION_SHOW_COMMAND_ID =
                "039b0f41-fd58-40c2-ad63-ded078131d74";

            /**
               Station show command name
            */
            const std::string STATION_SHOW_COMMAND_NAME = "station show";

            /**
               @brief station show command

               (application streamer) station show

               The station name is set to the channel name
            */
            class StationShowCommand : public cli::Command
            {
            public:
                /**
                   Constructor
                */
                StationShowCommand();

                /**
                   Destructor
                */
                virtual ~StationShowCommand(){}
            private:
                /**
                   Is the command require module be loaded or not

                   @return
                   - false - the module can be unloaded
                */
                virtual bool isRequireModule() const throw() {return false;}

                /**
                   Gets ID for CLI processor message's id

                   @return the message
                */
                virtual const std::string getMessageID() const throw()
                {
                    return STATION_SHOW_COMMAND_ID;
                }

                /**
                   Process the command

                   @param[in] params - the input parameters

                   @return the result of processing in the form of string
                   to be sent back to the CLI client

                   @exception @ref Result
                */
                virtual const std::string
                    process(const cli::ParameterVector& params);

                /**
                   @copydoc cli::ICommand::getCompletion
                */
                virtual const cli::ParameterVector
                    getCompletion(const cli::ParameterVector& setparams);

                /**
                   Retrives used host name by it's uuid

                   @param[in] uuid - the host's uuid

                   @return the host name
                */
                const std::string getUsedHostName(const std::string& uuid);
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                StationShowCommand(const StationShowCommand& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                StationShowCommand& operator=(const StationShowCommand& value);
            };

            /**
               NotAssigned show command id
            */
            const std::string NOT_ASSIGNED_SHOW_COMMAND_ID =
                "e3dd4738-c094-44e7-bd10-96f868987b7es";

            /**
               NotAssigned show command name
            */
            const std::string NOT_ASSIGNED_SHOW_COMMAND_NAME = "notassigned show";

            /**
               @brief Shows not assigned channels

               (application streamer) not assigned show

               The station name is set to the channel name
            */
            class NotAssignedShowCommand : public cli::Command
            {
            public:
                /**
                   Constructor
                */
                NotAssignedShowCommand();

                /**
                   Destructor
                */
                virtual ~NotAssignedShowCommand(){}
            private:
                /**
                   Is the command require module be loaded or not

                   @return
                   - false - the module can be unloaded
                */
                virtual bool isRequireModule() const throw() {return false;}

                /**
                   Gets ID for CLI processor message's id

                   @return the message
                */
                virtual const std::string getMessageID() const throw()
                {
                    return NOT_ASSIGNED_SHOW_COMMAND_ID;
                }

                /**
                   Process the command

                   @param[in] params - the input parameters

                   @return the result of processing in the form of string
                   to be sent back to the CLI client

                   @exception @ref Result
                */
                virtual const std::string
                    process(const cli::ParameterVector& params);

                /**
                   @copydoc cli::ICommand::getCompletion
                */
                virtual const cli::ParameterVector
                    getCompletion(const cli::ParameterVector& setparams);

                /**
                   Retrives used host name by it's uuid

                   @param[in] uuid - the host's uuid

                   @return the host name
                */
                const std::string getUsedHostName(const std::string& uuid);
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                NotAssignedShowCommand(const NotAssignedShowCommand& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                NotAssignedShowCommand& operator=(const NotAssignedShowCommand& value);
            };
        }
    }
}

#endif //KLK_SHOWCMD_H
