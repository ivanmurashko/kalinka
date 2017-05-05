/**
   @file cliapp.h
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
   - 2009/02/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CLIAPP_H
#define KLK_CLIAPP_H

#include <string>

#include "icli.h"
#include "ifactory.h"

namespace klk
{
    namespace cli
    {
        /**
           @brief The base command for working with applications

           The base command for working with applications
        */
        class ApplicationBase : public ICommand
        {
        public:
            /**
               Constructor

               @param[in] msgid - the message id
            */
            explicit ApplicationBase(const std::string& msgid);

            /**
               Destructor
            */
            virtual ~ApplicationBase(){}
        protected:
            /**
               Retrives module name

               @return the module name

               @exception @ref klk::Exception
            */
            const std::string getModuleName() const;

            /**
               Retrives module id

               @return the module id

               @exception @ref klk::Exception
            */
            const std::string getModuleID() const;

            /**
               Retrives factory

               @return the factory

               @exception @ref klk::Exception
            */
            IFactory* getFactory();
        private:
            IFactory* m_factory; ///< factory
            std::string m_msgid; ///< message id
            std::string m_modid; ///< module id

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
            virtual const std::string getMessageID() const throw(){return m_msgid;}

            /**
               Sets factory object

               @param[in] factory - the factory to be set
            */
            virtual void setFactory(IFactory* factory);

            /**
               Sets module ID

               @param[in] id - the id to be set
            */
            virtual void setModuleID(const std::string& id);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ApplicationBase& operator=(const ApplicationBase& value);
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ApplicationBase(const ApplicationBase& value);
        };

        /**
           on parameter
        */
        const std::string AUTOSTARTSET_ON("on");

        /**
           off parameter
        */
        const std::string AUTOSTARTSET_OFF("off");


        /**
           @brief The [application] autostart set on|off command

           The [application] autostart set on|off command
        */
        class AutostartSet : public ApplicationBase
        {
        public:
            /**
               Constructor

               @param[in] msgid - the message id
            */
            explicit AutostartSet(const std::string& msgid);

            /**
               Destructor
            */
            virtual ~AutostartSet(){}
        private:
            /**
               Gets the command name

               @return the command's name
            */
            virtual const std::string getName() const;

            /**
               Gets the command description

               @return the command's description
            */
            virtual const std::string getSummary() const;

            /**
               Gets the command usage description

               @return the command's usage description
            */
            virtual const std::string getUsage() const;

            /**
               Process the command

               @param[in] params - the parameters

               @return the result of processing in the form of string
               to be sent back to the CLI client

               @exception @ref Result
            */
            virtual const std::string process(const ParameterVector& params);

            /**
               @copydoc cli::ICommand::getCompletion
            */
            virtual const ParameterVector
                getCompletion(const ParameterVector& setparams);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            AutostartSet& operator=(const AutostartSet& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            AutostartSet(const AutostartSet& value);
        };


        /**
           @brief The [application] autostart show command

           The [application] autostart show command
        */
        class AutostartShow : public ApplicationBase
        {
        public:
            /**
               Constructor

               @param[in] msgid - the message id
            */
            explicit AutostartShow(const std::string& msgid);

            /**
               Destructor
            */
            virtual ~AutostartShow(){}
        private:
            /**
               Gets the command name

               @return the command's name
            */
            virtual const std::string getName() const;

            /**
               Gets the command description

               @return the command's description
            */
            virtual const std::string getSummary() const;

            /**
               Gets the command usage description

               @return the command's usage description
            */
            virtual const std::string getUsage() const;

            /**
               Process the command

               @param[in] params - the parameters

               @return the result of processing in the form of string
               to be sent back to the CLI client

               @exception @ref Result
            */
            virtual const std::string process(const ParameterVector& params);

            /**
               @copydoc cli::ICommand::getCompletion
            */
            virtual const ParameterVector
                getCompletion(const ParameterVector& setparams);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            AutostartShow& operator=(const AutostartShow& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            AutostartShow(const AutostartShow& value);
        };
    }
}

#endif //KLK_CLIAPP_H
