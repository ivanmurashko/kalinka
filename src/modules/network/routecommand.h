/**
   @file routecommand.h
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
   - 2009/03/04 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ROUTECOMMAND_H
#define KLK_ROUTECOMMAND_H

#include "cli.h"

namespace klk
{
    namespace net
    {
        /**
           Route list command id

           @ingroup grNetModule
        */
        const std::string ROUTE_LIST_COMMAND_ID =
            "833a0805-e503-47cd-8952-e8b408c66338";

        /**
           Route list command name

           @ingroup grNetModule
        */
        const std::string ROUTE_LIST_NAME = "route show";

        /**
           @brief Lists a route

           Lists a route

           @ingroup grNetModule
        */
        class RouteListCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            RouteListCommand();

            /**
               Destructor
            */
            virtual ~RouteListCommand(){}
        private:
            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return ROUTE_LIST_COMMAND_ID;
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
            RouteListCommand(const RouteListCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            RouteListCommand& operator=(const RouteListCommand& value);
        };

        /**
           Route add command id
        */
        const std::string ROUTE_ADD_COMMAND_ID =
            "8dced331-6e13-4339-90df-9804507bc5d2";

        /**
           Route add command name
        */
        const std::string ROUTE_ADD_NAME = "route add";

        /**
           @brief Adds a route

           Adds a route

           @ingroup grNetModule
        */
        class RouteAddCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            RouteAddCommand();

            /**
               Destructor
            */
            virtual ~RouteAddCommand(){}
        private:
            IDevPtr m_dev; ///< device
            std::string m_proto; ///< protocol

            /**
               Is the command require module be loaded or not

               @return
               - true - the module should be loaded
            */
            virtual bool isRequireModule() const throw() {return true;}

            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return ROUTE_ADD_COMMAND_ID;
            }

            /**
               Process the command

               @param[in] params - the input parameters

               @return the result of processing in the form of string
               to be sent back to the CLI client

               @exception @ref klk::Exception
            */
            virtual const std::string
                process(const cli::ParameterVector& params);

            /**
               Check route name to be added

               @param[in] name - the route name value to be checked

               @exception @ref klk::Exception if the name is invalid
            */
            void checkName(const std::string& name);

            /**
               Check dev value to be added

               @param[in] dev - the dev name value to be checked

               @exception @ref klk::Exception if the dev is invalid
            */
            void checkDev(const std::string& dev);

            /**
               Check host value to be added

               @param[in] host - the host value to be checked

               @exception @ref klk::Exception if the host is invalid
            */
            void checkHost(const std::string& host);

            /**
               Check port value to be added

               @param[in] host - the host
               @param[in] port - the port value to be checked

               @return
               - true
               - false
            */
            bool isPortValid(const std::string& host,
                             const std::string& port);

            /**
               Check protocol value to be added

               @param[in] protocol - the protocol value to be checked

               @exception @ref klk::Exception if the protocol is invalid
            */
            void checkProtocol(const std::string& protocol);

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
            RouteAddCommand(const RouteAddCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            RouteAddCommand& operator=(const RouteAddCommand& value);
        };

        /**
           Route del command id
        */
        const std::string ROUTE_DEL_COMMAND_ID =
            "5988402a-3167-42b6-8905-5e102c6e5688";

        /**
           Route delete command name
        */
        const std::string ROUTE_DEL_NAME = "route del";

        /**
           @brief Deletes a route

           Deletes a route

           @ingroup grNetModule
        */
        class RouteDelCommand : public cli::Command
        {
        public:
            /**
               Constructor
            */
            RouteDelCommand();

            /**
               Destructor
            */
            virtual ~RouteDelCommand(){}
        private:
            /**
               Is the command require module be loaded or not

               @return
               - true - the module should be loaded
            */
            virtual bool isRequireModule() const throw() {return true;}

            /**
               Gets ID for CLI processor message's id

               @return the message
            */
            virtual const std::string getMessageID() const throw()
            {
                return ROUTE_DEL_COMMAND_ID;
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
               Gets the command usage description

               @return the command's usage description
            */
            virtual const std::string getUsage() const;

            /**
               Retrives a string with route names

               @return the names of routes that can be deleted
            */
            const std::string getRouteNames() const;

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
            RouteDelCommand(const RouteDelCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            RouteDelCommand& operator=(const RouteDelCommand& value);
        };
    }
}

#endif //KLK_ROUTECOMMAND_H
