/**
   @file clicommands.h
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
   - 2008/11/14 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CLICOMMANDS_H
#define KLK_CLICOMMANDS_H

#include "cli.h"
#include "defines.h"

namespace klk
{
    namespace srv
    {
        /** @defgroup grSysModuleCLI Service module CLI

            @brief CLI commands for service module

            There are several CLI commands processed by the service module

            @ingroup grSysModule
            @{
        */


        /**
           Modules info command id
        */
        const std::string INFO_MOD_COMMAND_ID =
            "e029f17f-93ec-4007-9b71-3f7e87e5733d";

        /**
           Application info command id
        */
        const std::string INFO_APP_COMMAND_ID =
            "9f865e48-5379-4dc9-9efd-0fec68687106";

        /**
           Module info command name
        */
        const std::string MODINFONAME = "module status";

        /**
           Load module command name
        */
        const std::string LOADMODNAME = "module start";

        /**
           Load application command name
        */
        const std::string LOADAPPNAME = "application start";


        /**
           @brief Retrives info about the mediaserver

           Retrives info about the mediaserver for CLI
        */
        class InfoCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            InfoCommand();

            /**
               Destructor
            */
            virtual ~InfoCommand(){}

        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return INFO_COMMAND_ID;
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
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            InfoCommand(const InfoCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InfoCommand& operator=(const InfoCommand& value);
        };

        /**
           @brief Helper class

           Helper class to create CLI command response
        */
        class InfoHelper : public cli::Command
        {
        protected:
            /**
               Constructor
            */
            InfoHelper(const std::string& name,
                                const std::string& summary,
                                const std::string& usage,
                                mod::Type type);

            /// Destructor
            virtual ~InfoHelper(){}
        protected:
            /**
               Retrives module load info

               @param[in] factory - the module factory to retrive the module instance
               @param[in] id - the interested module id

               @return a string with load info
            */
            virtual const std::string getLoadInfo(IModuleFactory* factory,
                                                  const std::string& id) const = 0;

            /**
               Retrives module load info

               @param[in] factory - the module factory to retrive the module instance
               @param[in] id - the interested module id

             @return a string with CPU usage info
            */
            virtual const std::string getCPUUsageInfo(IModuleFactory* factory,
                                                      const std::string& id) const = 0;
        private:
            mod::Type m_type; ///< type

            /**
               Finds max alignment length

               @param[in] factory - the factory
               @param[in] list - the modules list

               @return the max size
            */
            size_t getMaxSize(IModuleFactory* factory,
                              const ModuleList& list) const;

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
               Creates a response

               @param[in] factory - the module factory
               @param[in] name - the application/module name

               @return - the response

               @exception @ref klk::ERROR
            */
            const std::string
                getResponse(IModuleFactory* factory, const std::string& name);

            /// @copydoc cli::ICommand::getCompletion
            virtual const cli::ParameterVector
                getCompletion(const cli::ParameterVector& setparams);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            InfoHelper(const InfoHelper& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InfoHelper& operator=(const InfoHelper& value);
        };

        /**
           @brief Retrives info about the mediaserver

           Retrives info about the mediaserver's modules for CLI
        */
        class ModInfoCommand : public InfoHelper
        {
        public:
            /// Constructor
            ModInfoCommand();

            /// Destructor
            virtual ~ModInfoCommand(){}
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return INFO_MOD_COMMAND_ID;
            }

            /// @copydoc klk::srv::InfoHelper::getLoadInfo()
            virtual const std::string getLoadInfo(IModuleFactory* factory,
                                                  const std::string& id) const;

            /// @copydoc klk::srv::InfoHelper::getCPUUsageInfo()
            virtual const std::string getCPUUsageInfo(IModuleFactory* factory,
                                                      const std::string& id) const;
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ModInfoCommand(const ModInfoCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ModInfoCommand& operator=(const ModInfoCommand& value);
        };

        /**
           @brief Retrives info about the mediaserver

           Retrives info about the mediaserver's appules for CLI
        */
        class AppInfoCommand : public InfoHelper
        {
        public:
            /// Constructor
            AppInfoCommand();

            /// Destructor
            virtual ~AppInfoCommand(){}
        private:
            /// @copydoc klk::srv::InfoHelper::getLoadInfo()
            virtual const std::string getLoadInfo(IModuleFactory* factory,
                                                  const std::string& id) const;

            /// @copydoc klk::srv::InfoHelper::getCPUUsageInfo()
            virtual const std::string getCPUUsageInfo(IModuleFactory* factory,
                                                      const std::string& id) const;

            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return INFO_APP_COMMAND_ID;
            }
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            AppInfoCommand(const AppInfoCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            AppInfoCommand& operator=(const AppInfoCommand& value);
        };

        /**
           Application info command id
        */
        const std::string LOAD_MOD_COMMAND_ID =
            "982a4d15-b844-4d7d-8df3-eadd6d5cd82b";

        /**
           @brief The module loading command

           The module loading command
        */
        class LoadModuleCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            LoadModuleCommand();

            /**
               Destructor
            */
            virtual ~LoadModuleCommand(){}
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return LOAD_MOD_COMMAND_ID;
            }

            /**
               Process the command

               @param[in] params - the input parameters

               @return the result of processing in the form of string
               to be sent back to the CLI client

               @exception @ref Result
            */
            virtual const std::string process(
                const cli::ParameterVector& params)
                ;

            /**
               Retrives usage string

               @return the usage string

               @exception @ref Result
            */
            const std::string getUsage();

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
            LoadModuleCommand(const LoadModuleCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            LoadModuleCommand& operator=(const LoadModuleCommand& value);
        };

        /**
           Application info command id
        */
        const std::string LOAD_APP_COMMAND_ID =
            "29cb7871-835b-4765-9098-ceb2e79e930d";

        /**
           @brief The application loading command

           The application loading command
        */
        class LoadApplicationCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            LoadApplicationCommand();

            /**
               Destructor
            */
            virtual ~LoadApplicationCommand(){}
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return LOAD_APP_COMMAND_ID;
            }

            /**
               Process the command

               @param[in] params - the input parameters

               @return the result of processing in the form of string
               to be sent back to the CLI client

               @exception @ref Result
            */
            virtual const std::string process(
                const cli::ParameterVector& params);

            /**
               Retrives usage string

               @return the usage string

               @exception @ref Result
            */
            const std::string getUsage();

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
            LoadApplicationCommand(const LoadApplicationCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            LoadApplicationCommand& operator=(const LoadApplicationCommand& value);
        };

        /** @} */
    }
}

#endif //KLK_CLICOMMANDS_H
