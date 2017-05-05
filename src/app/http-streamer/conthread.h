/**
   @file conthread.h
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
   - 2009/03/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CONTHREAD_H
#define KLK_CONTHREAD_H

#include <list>
#include <map>

#include <boost/shared_ptr.hpp>

#include "socket/socket.h"
#include "thread.h"
#include "binarydata.h"
#include "inthread.h"
#include "safelist.h"

namespace klk
{
    namespace http
    {
        /**
           @brief Connection thread

           The connection thread. The thread processes incomming
           connections

           @ingroup grHTTP
        */
        class ConnectThread : public Thread
        {
        public:
            /**
               Constructor

               @param[in] factory - the fctory
               @param[in] sock - the socket
            */
            ConnectThread(Factory* factory, const klk::ISocketPtr& sock);

            /**
               Destructor
            */
            virtual ~ConnectThread();

            /**
               Retrives the path

               @return the path
            */
            const std::string getPath() const throw();

            /**
               @brief Adds a binary data portion for processing

               The method checks path and if the path is OK the data
               will be added for processing

               In other cases just return

               @param[in] path - the path for the data
               @param[in] data - the data to be added

               @exception klk::Exception
            */
            void addData(const std::string& path,
                         const klk::BinaryData& data);

            /**
               Retrives uuid
            */
            const std::string getUUID() const throw(){return m_uuid;}

            /**
               Retrives rate

               @return the rate
            */
            virtual const double getRate() const;
        private:
            /**
               List with data
            */
            typedef SafeList<BinaryData> DataList;

            /**
               Request type
            */
            typedef enum
            {
                REQ_UNKNOWN = 0,
                HEAD = 1,
                GET = 2
            } RequestType;

            /**
               Result type
            */
            typedef enum
            {
                RES_UNKNOWN = 0,
                OK = 1,
                NOT_FOUND = 2
            } ResponseType;

            /**
               HTTP proto version
            */
            typedef enum
            {
                HTTP_UNKNOWN = 0,
                HTTP10 = 1,
                HTTP11 = 2
            } HTTPVersion;

            const std::string m_uuid; ///< the thread uuid
            klk::ISocketPtr m_sock; ///< socket
            std::string m_path; ///< the path for this connection
            DataList m_data;    ///< data list
            klk::Event m_wait; ///< wait event
            RequestType m_request_type; ///< request type
            ResponseType m_response_type; ///< response type
            HTTPVersion m_http_version; ///< http version
            SafeValue<time_t> m_hang_time; ///< time when hang up was started

            /// @copydoc IThread::init()
            virtual void init();

            /// @copydoc IThread::start()
            virtual void start();

            /// @copydoc IThread::stop()
            virtual void stop() throw();

            /// Sends an event about thread stop
            void notifyStop();

            /**
               Processes request

               determine it's type and sends initial response
            */
            void processRequest();

            /**
               Processes data
            */
            void processData();

            /**
               Retrives in thread

               @return the input thread pointer
            */
            InThreadPtr getInThread();

            /**
               Parses request and set internal parameters:
               request type, path and HTTP version

               @param[in] reqstr - the request string
               for example HEAD /path HTTP/1.0
            */
            void parseRequest(const std::string& reqstr);

            /**
               Check path

               Is the path match or not

               @param[in] path - the path to be checked

               @return
               - true
               - false
            */
            const bool isPathMatch(const std::string& path) const throw();

            /**
               Processes a response

               @param[in] inthread - the input thread

               @exception klk::Exception
            */
            void processResponse(const InThreadPtr& inthread);

            /**
               Processes an OK response

               @param[in] inthread - the input thread

               @exception klk::Exception
            */
            void processOK(const InThreadPtr& inthread);

            /**
               Processes a Not Found response

               @exception klk::Exception
            */
            void processNotFound();

            /**
               Sends a data portion

               @param[in] data - the data to be sent

               @exception klk::Exception
            */
            void sendData(const klk::BinaryData& data);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ConnectThread(const ConnectThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ConnectThread& operator=(const ConnectThread& value);
        };

        /**
           Smart pointer for connection thread
        */
        typedef boost::shared_ptr<ConnectThread> ConnectThreadPtr;

        /**
           @brief List with connection threads

           List with connection threads

           @ingroup grHTTP
        */
        class ConnectThreadContainer : public BaseContainer
        {
        public:
            /**
               Constructor
            */
            ConnectThreadContainer(Factory* factory);
            /**
               Destructor
            */
            virtual ~ConnectThreadContainer();

            /**
               Sends a connection data to all out threads

               @param[in] path - the path for determining socket data
               @param[in] data - the data to be sent

               @exception klk::Exception
            */
            void sendConnectionData(const std::string& path,
                                    const klk::BinaryData& data);

            /**
               Starts connection thread

               @param[in] thread - the thread to be started

               @exception klk::Exception
            */
            void startConnectThread(const ConnectThreadPtr& thread);

            /**
               Sends a notification about stop connection thread

               @param[in] thread - the thread to be stopped

               @exception klk::Exception
            */
            void notifyStopConnectThread(const ConnectThread* thread);

            /**
               Stops all connection threads associated with the current input one

               @param[in] thread - the input thread

               @exception klk::Exception
            */
            void stopThreads(const InThreadPtr& thread);

            /**
               Retrive connection rate by input thread

               @param[in] path - the input thread path

               @exception klk::Exception
            */
            const double getConnectionRate(const std::string& path) const;
        private:
            /**
               List with input threads
            */
            typedef std::list<ConnectThreadPtr> ConnectThreadList;

            ConnectThreadList m_list; ///< list with input threads
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ConnectThreadContainer(const ConnectThreadContainer& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ConnectThreadContainer& operator=(const ConnectThreadContainer& value);
        };

        /**
           Input thread container smart pointer
        */
        typedef boost::shared_ptr<ConnectThreadContainer>
            ConnectThreadContainerPtr;
    }
}

#endif //KLK_CONTHREAD_H
