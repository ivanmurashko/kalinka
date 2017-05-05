/**
   @file processor.h
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
   - 2009/05/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SNMPPROCESSOR_H
#define KLK_SNMPPROCESSOR_H

#include <boost/function/function1.hpp>

#include "thread.h"
#include "idata.h"
#include "protocol.h"
#include "socket/socket.h"

namespace klk
{
    namespace snmp
    {
        /**
           SNMP data processor

           The processor should take an snmp::IDataPtr and process it
        */
        typedef boost::function1<const IDataPtr,
            const IDataPtr&> DataProcessor;

        /**
           @brief SNMP info processor

           The thread processes SNMP info

           @ingroup grSNMP
        */
        class Processor : public base::Thread
        {
        public:
            /**
               Constructor

               @param[in] f - the data processor
               @param[in] sockname - the socket name for communications
            */
            Processor(DataProcessor f, const std::string& sockname);

            /**
               Destructor
            */
            virtual ~Processor();
        private:
            std::string m_sockname; ///< socket name
            DataProcessor m_f; ///< processor
            klk::IListenerPtr m_listener; ///< listener
            klk::ISocketPtr m_sock; ///< current socket in processing
            mutable klk::Mutex m_sock_lock; ///< locker for m_sock

            /**
               @copydoc IThread::init
            */
            virtual void init();

            /**
               @copydoc IThread::start
            */
            virtual void start();

            /**
               @copydoc IThread::stop
            */
            virtual void stop() throw();

            /**
               Gets socket
            */
            const klk::ISocketPtr getSock() const;

            /**
               Sets socket

               @param[in] sock - the socket to be set
            */
            void setSock(const klk::ISocketPtr& sock);

            /**
               Stops socket check data
            */
            void stopSock() throw();

            /**
               Resets socket check data
            */
            void resetSock() throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Processor(const Processor& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Processor& operator=(const Processor& value);
        };
    }
}

#endif //KLK_SNMPPROCESSOR_H
