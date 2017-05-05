/**
   @file sourcecmd.h
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
   - 2009/11/06 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SOURCECMD_H
#define KLK_SOURCECMD_H

#include "cmd.h"

namespace klk
{
    namespace trans
    {

        /** @addtogroup grTransCLI

            Source related CLI commands

            @{
        */

        /**
           Source add command id
        */
        const std::string SOURCEADD_COMMAND_ID =
            "3c0bff25-410e-467b-b497-b2e2a28f5620";

        /**
           @brief The source add command

           The command adds a source
        */
        class SourceAddCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            SourceAddCommand();

            /**
               Destructor
            */
            virtual ~SourceAddCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return SOURCEADD_COMMAND_ID;
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
               Retrives a list with possible source types names

               @return the list
            */
            const StringList getSourceTypeNameList();

            /**
               Retrives a list with possible source names
               for the specified type

               @param[in] type_name - the specified source type name

               @return the list
            */
            const StringList getSourceNameList(const std::string& type_name);

            /**
               Adds a source with specified parameters

               @param[in] type - the source type name
               @param[in] name - the source name
               @param[in] media_type_name - the media type name

               @return the assigned source name
               (will construct from type and name arguments)

               @exception klk::Exception if there was any trouble
               with @ref grDB DB
            */
            const std::string addSource(const std::string& type,
                                        const std::string& name,
                                        const std::string& media_type_name);

            /**
               Retrives a list with possible media types names

               @return the list
            */
            const StringList getMediaTypeList();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SourceAddCommand& operator=(const SourceAddCommand& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SourceAddCommand(const SourceAddCommand& value);
        };

        /**
           Source delete command id
        */
        const std::string SOURCEDEL_COMMAND_ID =
            "a6593cdb-c713-4654-82a4-3f4944eec572";

        /**
           @brief The source del command

           The command dels a source
        */
        class SourceDelCommand : public BaseCommand
        {
        public:
            /**
               Constructor
            */
            SourceDelCommand();

            /**
               Destructor
            */
            virtual ~SourceDelCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return SOURCEDEL_COMMAND_ID;
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
               Deletes a source with specified parameters

               @param[in] name - the source name

               @exception klk::Exception if there was any trouble
               with @ref grDB DB
            */
            void delSource(const std::string& name);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SourceDelCommand& operator=(const SourceDelCommand& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SourceDelCommand(const SourceDelCommand& value);
        };

        /**
           Source show command id
        */
        const std::string SOURCESHOW_COMMAND_ID =
            "7fe70f4e-8113-42e9-a12b-30f6405c91b9";

        /**
           @brief The source show command

           The command shows a source
        */
        class SourceShowCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            SourceShowCommand();

            /**
               Destructor
            */
            virtual ~SourceShowCommand();
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return SOURCESHOW_COMMAND_ID;
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
            SourceShowCommand& operator=(const SourceShowCommand& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SourceShowCommand(const SourceShowCommand& value);
        };

        /** @} */
    }
}


#endif //KLK_SOURCECMD_H
