/**
   @file streamer.h
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
   - 2009/02/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STREAMER_H
#define KLK_STREAMER_H

#include "launchmodule.h"
#include "httpfactory.h"
#include "mod/infocontainer.h"

namespace klk
{
    namespace http
    {

        /** @defgroup grHTTP HTTP streamer application
            @{

            The HTTP streamer application. The application receivies a multimedia data
            as input and stream them for end-user via HTTP

            @ingroup grApp
        */

        /**
           @brief The base class for HTTP streamer

           The base class for HTTP streamer
        */
        class Streamer : public launcher::Module
        {
            friend class TestStartup;
            friend class TestUDP;

            /// The storage for InputInfo
            typedef mod::InfoContainer<InputInfo>::InfoSet InfoSet;
        public:
            /**
               Constructor

               @param[in] factory - the factory object
            */
            explicit Streamer(IFactory* factory);

            /**
               Destructor
            */
            virtual ~Streamer();

            /**
               Retrives HTTP factory for CLI

               @return the factory

               @exception klk::Exception

               @note this used only to retrive output info
               FIXME!!! remove it
            */
            Factory* getHTTPFactory();

            /**
               Retrives input info for CLI

               @return the list with the info

               @exception klk::Exception
            */
            const InputInfoList getInputInfo() const
            {
                return m_info.getInfoList();
            }
        private:
            Factory* m_httpfactory; ///< http factory
            mod::InfoContainer<InputInfo> m_info; ///< the input info storage

            /// Retrives an set with input info data from @ref grDB "database"
            const InfoSet getInputInfoFromDB();

            /**
               Register all processors
            */
            virtual void registerProcessors();

            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw();

            /**
               Process changes at the DB

               @param[in] msg - the input message

               @exception @ref klk::Exception
            */
            virtual void processDB(const IMessagePtr& msg);

            /**
               Do some actions before main loop

               @exception klk::Exception
            */
            virtual void preMainLoop();

            /**
               Do some actions after main loop
            */
            virtual void postMainLoop() throw();

            /**
               Retrives out route info from DB

               @return the route uuid

               @exception klk::Exception
            */
            const std::string getOutRoute();

            /**
               Retrives (from DB) and sets out route info

               @exception @ref klk::Eexception
            */
            void processOutRoute();

            /**
               Retrives (from DB) and sets input route info

               @exception @ref klk::Eexception
            */
            void processInRoute();

            /**
               Retrives route info by it's uuid from Network module

               @param[in] uuid - the UUID at the @ref grDB "DB"

               @return the route info

               @exception klk::Exception
            */
            const RouteInfoPtr getRouteInfoByUUID(const std::string& uuid);

            /**
               Stops a route usage

               @param[in] route - the route to be stopped

               @exception klk::Exception
            */
            void stopRoute(const RouteInfoPtr& route);

            /**
               Processes SNMP request

               @param[in] req - the request

               @return the response
            */
            const snmp::IDataPtr processSNMP(const snmp::IDataPtr& req);

            /**
               Deletes an input route info

               @param[in] info - the info to be deleted
            */
            inline void delInputInfo(const InputInfoPtr& info) throw();

            /**
               Adds an input route info

               @param[in] info - the info to be added
            */
            inline void addInputInfo(const InputInfoPtr& info) throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Streamer(const Streamer& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Streamer& operator=(const Streamer& value);
        };

        /** @} */

    }
}

#endif //KLK_STREAMER_H
