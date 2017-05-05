/**
   @file outcmd.h
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
   - 2009/03/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_OUTCMD_H
#define KLK_OUTCMD_H

#include "cli.h"
#include "basecmd.h"

namespace klk
{
    namespace http
    {
        /**
           The ID for output HTTP info command show

           @ingroup grHTTP
        */
        const std::string OUT_SHOW_COMMAND_ID=
            "bc0935e9-7de8-4e31-a734-351f258fe00b";

        /**
           The name for output HTTP info command show

           @ingroup grHTTP
        */
        const std::string OUT_SHOW_COMMAND_NAME=
            "output show";

        /**
           @brief The output info show command

           Command to show output info

           @ingroup grHTTP
        */
        class OutputShowCommand  : public cli::Command
        {
        public:
            /**
               Constructor
            */
            OutputShowCommand();

            /**
               Destructor
            */
            virtual ~OutputShowCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return OUT_SHOW_COMMAND_ID;
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
               @copydoc cli::ICommand::getCompletion
            */
            virtual const cli::ParameterVector
                getCompletion(const cli::ParameterVector& setparams);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            OutputShowCommand(const OutputShowCommand& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            OutputShowCommand& operator=(const OutputShowCommand& value);
        };


        /**
           The ID for output HTTP info command set

           @ingroup grHTTP
        */
        const std::string OUT_SET_COMMAND_ID=
            "028ed85b-c376-42f4-80cf-abbe954ba257";

        /**
           The name for output HTTP info command set

           @ingroup grHTTP
        */
        const std::string OUT_SET_COMMAND_NAME=
            "output set";

        /**
           @brief The output info set command

           Command to set output info

           @ingroup grHTTP
        */
        class OutputSetCommand  : public AddCommand
        {
        public:
            /**
               Constructor
            */
            OutputSetCommand();

            /**
               Destructor
            */
            virtual ~OutputSetCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return OUT_SET_COMMAND_ID;
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
               @copydoc cli::ICommand::getCompletion
            */
            virtual const cli::ParameterVector
                getCompletion(const cli::ParameterVector& setparams);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            OutputSetCommand(const OutputSetCommand& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            OutputSetCommand& operator=(const OutputSetCommand& value);
        };
    }
}

#endif //KLK_OUTCMD_H
