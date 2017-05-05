/**
   @file stationaddcmd.h
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
   - 2009/02/13 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STATIONADDCMD_H
#define KLK_STATIONADDCMD_H

#include "cli.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {
            /**
               Station add command id
            */
            const std::string STATION_ADD_COMMAND_ID =
                "b4d35105-8bd6-4bc5-af31-7cb1dd932261";

            /**
               Station add command name
            */
            const std::string STATION_ADD_COMMAND_NAME = "station add";

            /**
               @brief station add command

               (application streamer) station add [channel_name]
               [route_name] ([priority])

               The station name is set to the channel name
            */
            class StationAddCommand : public cli::Command
            {
            public:
                /**
                   Constructor
                */
                StationAddCommand();

                /**
                   Destructor
                */
                virtual ~StationAddCommand(){}
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
                    return STATION_ADD_COMMAND_ID;
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
                   Retrives possible channel names (not assigned channels)

                   @return the list
                */
                const StringList getNotAssignedChannels();

                /**
                   Retrives possible route names (not assigned routes)

                   @return the list
                */
                const StringList getNotAssignedRoutes();
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                StationAddCommand(const StationAddCommand& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                StationAddCommand& operator=(const StationAddCommand& value);
            };
        }
    }
}

#endif //KLK_STATIONADDCMD_H
