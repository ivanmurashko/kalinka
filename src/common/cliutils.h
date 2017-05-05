/**
   @file cliutils.h
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
   - 2009/05/03 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CLIUTILS_H
#define KLK_CLIUTILS_H

#include <string>

#include "imessage.h"
#include "icli.h"

namespace klk
{
    namespace cli
    {
        /**
           CLI command mode

           @ingroup grCLI
        */
        typedef enum
        {
            UNKNOWN = 0,
            PROCESS = 1,
            GETCOMPLETION = 2
        } CommandMode;

        /**
           @brief CLI utils

           Utils

           @ingroup grCLI
        */
        class Utils
        {
        public:
            /**
               Constructor
            */
            Utils(){}

            /**
               Destructor
            */
            ~Utils(){}

            /**
               Fills the message with parameters for processing

               @param[in] msg - the message to be filled
               @param[in] params - the parameters to be used

               @exception klk::Exception
            */
            static void setProcessParams(const IMessagePtr& msg,
                                         const ParameterVector& params);

            /**
               Retrives parameters from the message

               @param[in] msg - the message with the requested info

               @return the parameters list

               @exception klk::Exception
            */
            static const ParameterVector
                getProcessParams(const IMessagePtr& msg);

            /**
               Retrives parameters vector from data string

               retrive vector with command names from command

               "cm1 cmd2 cmd3 " +> "cm1" "cmd2" "cmd3"

               @param[in] data - the data string to be splitted

               @return the commands
            */
            static const ParameterVector getCommands(const std::string& data);

            /**
               Sets completion info for sending

               @param[in] msg - the message to be filled
               @param[in] setparams - the parameters that have been already set
            */
            static void setCompletionRequest(const IMessagePtr& msg,
                                             const ParameterVector& setparams);

            /**
               Retrives completion parameters (that have been already set)
               from a message

               @param[in] msg - the message

               @return the parameters that have been already set
            */
            static const ParameterVector
                getCompletionRequest(const IMessagePtr& msg);

            /**
               Retrives completion info from a message

               @param[in] msg - the message
               @param[in] params - the data to be set

               @return the completions
            */
            static void
                setCompletionResponse(const IMessagePtr& msg,
                                      const ParameterVector& params);

            /**
               Sets completion info to a message

               @param[in] msg - the message

               @return the completions
            */
            static const ParameterVector
                getCompletionResponse(const IMessagePtr& msg);


            /**
               Retrives command mode from the message

               @param[in] msg - the message

               @return the mode
            */
            static const CommandMode getCommandMode(const IMessagePtr& msg);

        private:
            /**
               Fills the message with parameters

               @param[in] msg - the message to be filled
               @param[in] params - the params to be used

               @exception klk::Exception
            */
            static void setParams(const IMessagePtr& msg,
                                  const ParameterVector& params);

            /**
               Retrives parameters from the message

               @param[in] msg - the message with the requested info

               @return the parameters list

               @exception klk::Exception
            */
            static const ParameterVector getParams(const IMessagePtr& msg);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Utils& operator=(const Utils& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Utils(const Utils& value);
        };
    }
}

#endif //KLK_CLIUTILS_H
