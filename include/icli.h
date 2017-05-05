/**
   @file icli.h
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
   - 2008/11/13 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ICLI_H
#define KLK_ICLI_H

#include <string>
#include <list>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "imessage.h"

namespace klk
{

    class IFactory;

    namespace cli
    {

        /** @defgroup grCLI CLI

            The group keeps defenitions for CLI

            @ingroup grModule

            @{
        */

        /**
           Defines CLI parameters
        */
        typedef std::vector<std::string> ParameterVector;

        /**
           @brief CLI command

           keeps info about a CLI command
        */
        class ICommand
        {
        public:
            /**
               Destructor
            */
            virtual ~ICommand(){}

            /**
               Gets the command name

               @return the command's name
            */
            virtual const std::string getName() const = 0;

            /**
               Gets the command description

               @return the command's description
            */
            virtual const std::string getSummary() const = 0;

            /**
               Gets the command usage description

               @return the command's usage description
            */
            virtual const std::string getUsage() const = 0;

            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw() = 0;

            /**
               Process the command

               @param[in] params - the parameters

               @return the result of processing in the form of string
               to be sent back to the CLI client

               @exception klk::Exception
            */
            virtual const std::string
                process(const ParameterVector& params) = 0;

            /**
               Retrives list of possible completions for a n's parameter

               @param[in] setparams - the parameters that have been already set

               @return the list
            */
            virtual const ParameterVector
                getCompletion(const ParameterVector& setparams) = 0;

            /**
               Sets factory object

               @param[in] factory - the factory to be set
            */
            virtual void setFactory(IFactory* factory) = 0;

            /**
               Sets module ID

               @param[in] id - the id to be set
            */
            virtual void setModuleID(const std::string& id) = 0;

            /**
               Is the command require module be loaded or not

               @return
               - true - the module should be loaded
               - false - the module can be unloaded
            */
            virtual bool isRequireModule() const throw() = 0;
        };

        /**
           The shared pointer to @ref ICommand
        */
        typedef boost::shared_ptr<ICommand> ICommandPtr;

        /**
           Command's list
        */
        typedef std::list<ICommandPtr> ICommandList;

        /** @} */
    }
}

#endif //KLK_ICLI_H
