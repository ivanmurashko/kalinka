/**
   @file trapreceiver.h
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
   - 2008/11/22 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SNMPRECEIVER_H
#define KLK_SNMPRECEIVER_H

#include <vector>
#include <string>

#include "ithread.h"
#include "scheduler.h"
#include "thread.h"
#include "snmp/snmp.h"

namespace klk
{
    namespace test
    {

        /** @defgroup grSNMPTest Utest for SNMP functionality
            @brief Utest for SNMP functionality

            @ingroup grSNMP

            Classes and structures for SNMP functionality unit tests

            @{
        */

        /**
           @brief The trap info container

           The trap info container
        */
        class TrapInfo
        {
        public:
            /**
               Constructor
            */
            TrapInfo(const std::string& oid, const std::string value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TrapInfo(const TrapInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TrapInfo& operator=(const TrapInfo& value);

            /**
               Destructor
            */
            ~TrapInfo(){}

            /**
               Gets oid
            */
            const std::string getOID() const {return m_oid;}

            /**
               Gets value
            */
            const std::string getValue() const {return m_value;}
        private:
            std::string m_oid; ///< oid
            std::string m_value; ///< value
        private:
            /**
               Constructor
            */
            TrapInfo();
        };


        /**
           The functor checks that OID is match to the text representation
        */
        struct IsOIDMatch :
            public std::binary_function<TrapInfo, std::string, bool>
        {
            /**
               The matcher functor

               @param[in] info - the trap info to be checked
               @param[in] oid - the OID that should match the checked info
            */
            bool operator()(const TrapInfo& info, const std::string oid)
            {
                _TrapInfo checker1(oid);
                _TrapInfo checker2(info.getOID());
                return checker1 == checker2;
            }
        private:
            /**
               Internal trap info holder
             */
            class _TrapInfo : public snmp::TrapInfoBase
            {
            public:
                /**
                   Constructor
                */
                _TrapInfo(const std::string& oid) : snmp::TrapInfoBase(oid)
                {
                }

                /**
                   Checks the oid

                   @return
                   - true the checked data are equal
                   - false there are 2 different data
                */
                bool operator==(const _TrapInfo& info) const
                {
                    if (m_oid_len != info.m_oid_len)
                        return false;
                    return !memcmp(m_oid, info.m_oid, m_oid_len * sizeof(oid));
                }
            };
        };

        /**
           Received traps info

           organized in the for of vector of string
        */
        typedef std::vector<TrapInfo> TrapInfoVector;

        /**
           @brief SNMP receiver thread

           The SNMP receiver thread

           @ingroup grSNMP
        */
        class SNMPReceiverThread : public base::Thread
        {
        public:
            /**
               Constructor
            */
            SNMPReceiverThread();

            /**
               Destructor
            */
            virtual ~SNMPReceiverThread();

            /**
               Gets the received traps
            */
            const TrapInfoVector getTraps() const {return m_traps;}
        private:
            TrapInfoVector m_traps; ///< received traps

            /**
               @copydoc IThread::start
            */
            virtual void start();

            /**
               The snmptrap callback
            */
            static int snmptrapCallback(int op, struct snmp_session *ss,
                                        int reqid, struct snmp_pdu *pdu,
                                        void *magic);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SNMPReceiverThread(const SNMPReceiverThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SNMPReceiverThread& operator=(const SNMPReceiverThread& value);
        };

        /**
           The smart pointer for SNMPReceiverThread
        */
        typedef boost::shared_ptr<SNMPReceiverThread> SNMPReceiverThreadPtr;

        /**
           @brief The SNMP receiver

           The SNMP receiver

           @ingroup grSNMP
        */
        class SNMPReceiver
        {
        public:
            /**
               Constructor
            */
            SNMPReceiver();

            /**
               Destructor
            */
            ~SNMPReceiver();

            /**
               Starts the receiver
            */
            void start();

            /**
               Stops the receiver
            */
            void stop() throw();

            /**
               Gets the received traps
            */
            const TrapInfoVector getTraps() const {return m_thread->getTraps();}
        private:
            SNMPReceiverThreadPtr m_thread; ///< receiver thread
            base::Scheduler m_scheduler; ///< scheduler for the thread start
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SNMPReceiver(const SNMPReceiver& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SNMPReceiver& operator=(const SNMPReceiver& value);
        };

        /** @} */

    }
}

#endif //KLK_SNMPRECEIVER_H
