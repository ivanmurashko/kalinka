/**
   @file scancommand.h
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
   - 2008/11/15 created by ipp (Ivan Murashko)
   - 2009/01/17 renamed to scancommand.h by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SCANCOMMAND_H
#define KLK_SCANCOMMAND_H

#include "cli.h"

namespace klk
{
    namespace dvb
    {
        namespace scan
        {
            /**
               Scan start command id
            */
            const std::string SCAN_START_COMMAND_ID =
                "eaa8ce2d-60ed-4bc4-bf19-944b863f374a";

            /**
               Scan stop command id
            */
            const std::string SCAN_STOP_COMMAND_ID =
                "654539c3-94b2-49e9-808a-0ca8129b33da";

            /**
               @brief Scan start command

               CLI command that starts scan process
            */
            class ScanStart : public cli::Command
            {
            public:
                /**
                   Constructor
                */
                ScanStart();

                /**
                   Destructor
                */
                virtual ~ScanStart(){}
            private:
                /**
                   Gets ID for CLI processor message's id

                   @return the message
                */
                virtual const std::string getMessageID() const throw()
                {
                    return SCAN_START_COMMAND_ID;
                }

                /**
                   Process the command

                   @param[in] params - the input parameters

                   @return the result of processing in the form of string
                   to be sent back to the CLI client

                   @exception klk::Exception
                */
                virtual const std::string process(
                    const cli::ParameterVector& params);

                /**
                   Gets the command usage description

                   @return the command's usage description
                */
                virtual const std::string getUsage() const;

                /**
                   @copydoc cli::ICommand::getCompletion
                */
                virtual const cli::ParameterVector
                    getCompletion(const cli::ParameterVector& setparams);

                /**
                   Retrive dev from parameter

                   @param[in] source_name - the source_name as parameter

                   @return the dev
                */
                const IDevPtr getDev(const std::string& source_name);
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                ScanStart(const ScanStart& value);


                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                ScanStart& operator=(const ScanStart& value);
            };

            /**
               @brief Scan stop command

               CLI command that stops scan process
            */
            class ScanStop : public cli::Command
            {
            public:
                /**
                   Constructor
                */
                ScanStop();

                /**
                   Destructor
                */
                virtual ~ScanStop(){}
            private:
                /**
                   Gets ID for CLI processor message's id

                   @return the message
                */
                virtual const std::string getMessageID() const throw()
                {
                    return SCAN_STOP_COMMAND_ID;
                }

                /**
                   Process the command

                   @param[in] params - the input parameters

                   @return the result of processing in the form of string
                   to be sent back to the CLI client

                   @exception klk::Exception
                */
                virtual const std::string process(
                    const cli::ParameterVector& params);

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
                ScanStop(const ScanStop& value);


                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                ScanStop& operator=(const ScanStop& value);
            };
        }
    }
}

#endif //KLK_SCANCOMMAND_H
