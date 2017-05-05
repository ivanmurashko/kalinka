/**
   @file cmd.h
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
   - 2009/12/19 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CMD_H
#define KLK_CMD_H

#include "cli.h"

namespace klk
{
    namespace httpsrc
    {

        /** @defgroup grHTTPSrcCLI HTTP  module CLI

            @ingroup grHTTPSrc

            CLI commands for http  module

            @{
        */

        /**
           @brief Base CLI command for HTTP source

           It defines several common procedures that are used by
           different CLI related classes
        */
        class BaseCommand : public cli::Command
        {
        public:
            /**
               Constructor

               @param[in] name - the command's name
               @param[in] summary - the summary for the command
               @param[in] usage - the usage for the command
            */
            BaseCommand(const std::string& name,
                        const std::string& summary,
                        const std::string& usage) : cli::Command(name, summary, usage)
            {
            }

            /**
               Destructor
            */
            virtual ~BaseCommand(){}
        protected:
            /**
               Retrives a list with source names which possible to delete

               @return the list
            */
            const StringList getSourceNameList();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            BaseCommand(const BaseCommand& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            BaseCommand& operator=(const BaseCommand& value);
        };

        /**
            add command id
        */
        const std::string ADD_COMMAND_ID = "c688869e-aed6-449a-a7da-47bcf4fa84aa";

        /**
           @brief The  add command

           The command adds a
        */
        class AddCommand : public BaseCommand
        {
        public:
            /**
               Constructor
            */
            AddCommand();

            /**
               Destructor
            */
            virtual ~AddCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return ADD_COMMAND_ID;
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
               Is the command require module be loaded or not

               @return
               - true - the module should be loaded
            */
            virtual bool isRequireModule() const throw() {return false;}

            /**
               @copydoc cli::ICommand::getCompletion
            */
            virtual const cli::ParameterVector
                getCompletion(const cli::ParameterVector& setparams);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            AddCommand& operator=(const AddCommand& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            AddCommand(const AddCommand& value);
        };

        /**
            delete command id
        */
        const std::string DEL_COMMAND_ID = "e838f9cb-520f-4d9d-9c11-a0f5383984ca";

        /**
           @brief The  del command

           The command dels a
        */
        class DelCommand : public BaseCommand
        {
        public:
            /**
               Constructor
            */
            DelCommand();

            /**
               Destructor
            */
            virtual ~DelCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return DEL_COMMAND_ID;
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
               Is the command require module be loaded or not

               @return
               - true - the module should be loaded
            */
            virtual bool isRequireModule() const throw() {return false;}

            /**
               @copydoc cli::ICommand::getCompletion
            */
            virtual const cli::ParameterVector
                getCompletion(const cli::ParameterVector& setparams);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            DelCommand& operator=(const DelCommand& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            DelCommand(const DelCommand& value);
        };

        /**
            show command id
        */
        const std::string SHOW_COMMAND_ID = "1d3ba2ec-f97f-45d0-b4ce-2829f1477a60";

        /**
           @brief The  show command

           The command shows a
        */
        class ShowCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            ShowCommand();

            /**
               Destructor
            */
            virtual ~ShowCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return SHOW_COMMAND_ID;
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
               Is the command require module be loaded or not

               @return
               - true - the module should be loaded
            */
            virtual bool isRequireModule() const throw() {return false;}

            /**
               @copydoc cli::ICommand::getCompletion
            */
            virtual const cli::ParameterVector
                getCompletion(const cli::ParameterVector& setparams);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ShowCommand& operator=(const ShowCommand& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ShowCommand(const ShowCommand& value);
        };

        /** @} */
    }
}

#endif //KLK_CMD_H
