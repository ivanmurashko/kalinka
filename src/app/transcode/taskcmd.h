/**
   @file taskcmd.h
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
   - 2009/11/15 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TASKCMD_H
#define KLK_TASKCMD_H

#include "cmd.h"

namespace klk
{
    namespace trans
    {

        /** @addtogroup grTransCLI

            Task related CLI commands

            @{
        */

        /**
           Task add command id
        */
        const std::string TASKADD_COMMAND_ID =
            "734cd0d2-5fa4-4a9f-bbf5-36828892fbc3";

        /**
           @brief The task add command

           The command adds a task
        */
        class TaskAddCommand : public BaseCommand
        {
        public:
            /**
               Constructor
            */
            TaskAddCommand();

            /**
               Destructor
            */
            virtual ~TaskAddCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return TASKADD_COMMAND_ID;
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

            /**
               Retrives a list with possible video sizes names

               @return the list
            */
            const StringList getVideoSizeList();

            /**
               Retrives a list with possible video qualities names

               @return the list
            */
            const StringList getVideoQualityList();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TaskAddCommand& operator=(const TaskAddCommand& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TaskAddCommand(const TaskAddCommand& value);
        };

        /**
           Task delete command id
        */
        const std::string TASKDEL_COMMAND_ID =
            "f1746342-f142-4f3a-a2f8-7521532d5e75";

        /**
           @brief The task del command

           The command dels a task
        */
        class TaskDelCommand : public BaseCommand
        {
        public:
            /**
               Constructor
            */
            TaskDelCommand();

            /**
               Destructor
            */
            virtual ~TaskDelCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return TASKDEL_COMMAND_ID;
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

            /**
               Deletes a task with specified parameters

               @param[in] name - the task name

               @exception klk::Exception if there was any trouble
               with @ref grDB DB
            */
            void delTask(const std::string& name);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TaskDelCommand& operator=(const TaskDelCommand& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TaskDelCommand(const TaskDelCommand& value);
        };

        /**
           Task show command id
        */
        const std::string TASKSHOW_COMMAND_ID =
            "2c37fda3-028c-4279-a254-97b90fe4e455";

        /**
           @brief The task show command

           The command shows a task
        */
        class TaskShowCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            TaskShowCommand();

            /**
               Destructor
            */
            virtual ~TaskShowCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return TASKSHOW_COMMAND_ID;
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
            TaskShowCommand& operator=(const TaskShowCommand& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TaskShowCommand(const TaskShowCommand& value);
        };

        /** @} */
    }
}

#endif //KLK_TASKCMD_H
