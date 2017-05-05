/**
   @file incmd.h
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
   - 2009/03/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_INCMD_H
#define KLK_INCMD_H

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
        const std::string IN_SHOW_COMMAND_ID=
            "ad890717-4dfc-465e-88eb-055504bdb5b0";

        /**
           The name for output HTTP info command show

           @ingroup grHTTP
        */
        const std::string IN_SHOW_COMMAND_NAME=
            "input show";

        /**
           @brief The output info show command

           Command to show output info

           @ingroup grHTTP
        */
        class InputShowCommand  : public cli::Command
        {
        public:
            /**
               Constructor
            */
            InputShowCommand();

            /**
               Destructor
            */
            virtual ~InputShowCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return IN_SHOW_COMMAND_ID;
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
            InputShowCommand(const InputShowCommand& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InputShowCommand& operator=(const InputShowCommand& value);
        };

        /**
           The ID for output HTTP info command set

           @ingroup grHTTP
        */
        const std::string IN_ADD_COMMAND_ID=
            "16125ef4-df51-48ce-81c8-a012198ccb5d";

        /**
           The name for output HTTP info command set

           @ingroup grHTTP
        */
        const std::string IN_ADD_COMMAND_NAME=
            "input add";

        /**
           @brief The output info set command

           Command to set output info

           @ingroup grHTTP
        */
        class InputAddCommand  : public AddCommand
        {
        public:
            /**
               Constructor
            */
            InputAddCommand();

            /**
               Destructor
            */
            virtual ~InputAddCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return IN_ADD_COMMAND_ID;
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
               Checks the path parameter

               @param[in] path - the value to be checked

               @exception klk::Exception
            */
            void checkPath(const std::string& path);

            /**
               Checks the type parameter

               @param[in] type - the value to be checked

               @exception klk::Exception
            */
            void checkMediaType(const std::string& type);

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
            InputAddCommand(const InputAddCommand& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InputAddCommand& operator=(const InputAddCommand& value);
        };

        /**
           The ID for input HTTP info command delete

           @ingroup grHTTP
        */
        const std::string IN_DEL_COMMAND_ID=
            "9c9280e6-363d-4502-ac3e-f69f31b9ec08";

        /**
           The name for output HTTP info command delete

           @ingroup grHTTP
        */
        const std::string IN_DEL_COMMAND_NAME=
            "input del";

        /**
           @brief The output info set command

           Command to set output info

           @ingroup grHTTP
        */
        class InputDelCommand  : public cli::Command
        {
        public:
            /**
               Constructor
            */
            InputDelCommand();

            /**
               Destructor
            */
            virtual ~InputDelCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return IN_DEL_COMMAND_ID;
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
            InputDelCommand(const InputDelCommand& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InputDelCommand& operator=(const InputDelCommand& value);
        };
    }
}

#endif //KLK_INCMD_H
