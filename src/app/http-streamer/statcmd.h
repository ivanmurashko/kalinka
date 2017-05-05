/**
   @file statcmd.h
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
   - 2009/04/25 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STATCMD_H
#define KLK_STATCMD_H

#include "cli.h"
#include "inthread.h"

namespace klk
{
    namespace http
    {
        /**
           The ID for stat HTTP command

           @ingroup grHTTP
        */
        const std::string STAT_COMMAND_ID =
            "4dfed298-6a79-4c9a-87ee-2a0880650f88";

        /**
           The name for output HTTP info command show

           @ingroup grHTTP
        */
        const std::string STAT_COMMAND_NAME =
            "stat show";

        /**
           The name for output HTTP info command show input

           @ingroup grHTTP
        */
        const std::string STAT_COMMAND_PARAM_INPUT =
            "input";

        /**
           The name for output HTTP outfo command show output

           @ingroup grHTTP
        */
        const std::string STAT_COMMAND_PARAM_OUTPUT =
            "output";


        /**
           @brief The output info show command

           Command to show output info

           @ingroup grHTTP
        */
        class StatCommand  : public cli::Command
        {
        public:
            /**
               Constructor
            */
            StatCommand();

            /**
               Destructor
            */
            virtual ~StatCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return STAT_COMMAND_ID;
            }

            /**
               Process the command

               @param[in] params - the input parameters

               @return the result of processing in the form of string
               to be sent back to the CLI client

               @exception @ref klk::Exception
            */
            virtual const std::string process(
                const cli::ParameterVector& params);

            /**
               Is the command require module be loaded or not

               @return
               - true - the module should be loaded
            */
            virtual bool isRequireModule() const throw() {return true;}

            /**
               Gets addr str for display

               @param[in] info - the input info

               @return the addr
            */
            const std::string getAddr(const InputInfoPtr& info) const;

            /**
               Gets broken package count for display

               @param[in] info - the input info

               @return the addr
            */
            const std::string getBrokenCount(const InputInfoPtr& info) const;

            /**
               Gets input rate for display

               @param[in] info - the input info

               @return the rate
            */
            const std::string getInputRate(const InputInfoPtr& info) const;

            /**
               Gets output rate for display

               @param[in] info - the input info

               @return the rate
            */
            const std::string getOutputRate(const InputInfoPtr& info) const;

            /**
               Gets connection count for display

               @param[in] info - the input info

               @return the count
            */
            const std::string
                getConnectionCount(const InputInfoPtr& info) const;


            /**
               Retrives input thread pointer by info

               @param[in] info - the input info

               @return pointer to the thread
            */
            const InThreadPtr getInThread(const InputInfoPtr& info) const;


            /**
               Retrives input info

               @return the info
            */
            const std::string getInputInfo() const;

            /**
               Retrives output info

               @return the info
            */
            const std::string getOutputInfo() const;

            /**
               Retrives rate string from double

               @return the string
            */
            const std::string getRate(const double rate) const;

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
            StatCommand(const StatCommand& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            StatCommand& operator=(const StatCommand& value);
        };
    }
}

#endif //KLK_STATCMD_H
