/**
   @file trapreceiver.cpp
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

/*
  Some code was taken from Sella NMS (snmptrap.cxx)
  Copyright (c) 2001,2002 Digital Genesis Software, LLC. All Rights Reserved.
  Released under the GPL Version 2 License.
  http://www.digitalgenesis.com
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <boost/lexical_cast.hpp>

#include "trapreceiver.h"
#include "log.h"
#include "testfactory.h"
#include "exception.h"

#ifndef RECEIVED_MESSAGE
#define RECEIVED_MESSAGE NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE
#endif

using namespace klk::test;

// Constructor
TrapInfo::TrapInfo(const std::string& oid, const std::string value) :
    m_oid(oid), m_value(value)
{
}

// Copy constructor
// @param[in] value - the copy param
TrapInfo::TrapInfo(const TrapInfo& value) :
    m_oid(value.m_oid), m_value(value.m_value)
{
}

// Assigment operator
// @param[in] value - the copy param
TrapInfo& TrapInfo::operator=(const TrapInfo& value)
{
    if (this == &value)
        return *this;
    m_oid = value.m_oid;
    m_value = value.m_value;
    return *this;
}



//
// SNMPReceiverThread class
//

// Constructor
SNMPReceiverThread::SNMPReceiverThread() :
    Thread(), m_traps()
{
}

// Destructor
SNMPReceiverThread::~SNMPReceiverThread()
{
}

int SNMPReceiverThread::snmptrapCallback(int op, struct snmp_session *ss,
                                         int reqid, struct snmp_pdu *pdu,
                                         void *magic)
{
    struct snmp_pdu **req = static_cast<snmp_pdu**> (magic);

    if (op != RECEIVED_MESSAGE)
    {
        klk_log(KLKLOG_DEBUG,
                "WARNING: snmptrap_callback trigged for timeout, "
                "or other non-RECEIVED_MESSAGE operation.");
        (*req) = NULL;
    }
    else
    {
        (*req) = snmp_clone_pdu(pdu);
    }

    return 1;
}

// Starts the thread
void SNMPReceiverThread::start()
{
    /* SNMP Variables */
    struct snmp_session session;
    int count = 0, numfds = 0, block = 0;
    fd_set fdset;
    struct timeval timeout;
    void *opaque = NULL;
    snmp_pdu *pdu = NULL;

    IFactory* factory = Factory::instance();
    BOOST_ASSERT(factory);

    char* peername = strdup(
        factory->getConfig()->getSNMPInfo()->getReceiver().c_str());
    char* community = strdup(
        factory->getConfig()->getSNMPInfo()->getCommunity().c_str());
    int port =
        factory->getConfig()->getSNMPInfo()->getReceiverPort();

    snmp_sess_init(&session);
    session.version = SNMP_VERSION_1;
    session.peername = peername;
    session.community = reinterpret_cast<u_char*>(community);
    session.community_len = strlen("public");
    session.retries = SNMP_DEFAULT_RETRIES;
    session.timeout = SNMP_DEFAULT_TIMEOUT;
    session.callback = snmptrapCallback;
    session.callback_magic = &pdu;
    session.authenticator = NULL;
    session.isAuthoritative = SNMP_SESS_UNKNOWNAUTH;
    session.local_port = port;

    opaque = snmp_sess_open(&session);
    if (!opaque)
    {
        snmp_sess_perror("snmpreceiver", &session);
        BOOST_ASSERT(opaque);
    }

    while (isStopped() == false)
    {
        numfds = 0;
        FD_ZERO(&fdset);
        block = 0;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        snmp_sess_select_info(opaque, &numfds, &fdset, &timeout, &block);
        /* FIXME: Look into if snmp_sess_select_info() should be
           setting block to 1. When it does, SIGTERM/SIGHUP doesn't
           complete,s since select() never times out. Setting block
           to 0 fixes the problem but may affect trap collection.
        */
        block = 0;
        count = select(numfds, &fdset, 0, 0, (block) ? NULL : &timeout);

        switch (count)
        {
        case -1:
            // "WARNING: generic SNMP session error.")
            BOOST_ASSERT(false);
            continue;
        case 0:
            snmp_sess_timeout(opaque);
            continue;
        default:
            /* This triggers the callback. */
            snmp_sess_read(opaque, &fdset);
        }

        try
        {
            if (pdu)
            {
                // read it
                // only one
                BOOST_ASSERT(pdu->variables->next_variable == NULL);
                std::string oid = "";
                for (size_t i = 0; i < pdu->enterprise_length; i++)
                {
                    int elem = static_cast<int>(pdu->enterprise[i]);
                    oid += "." + boost::lexical_cast<std::string>(elem);
                }

                std::string value = "";

                switch (pdu->variables->type)
                {
                case ASN_OCTET_STR:
                case ASN_OPAQUE:
                {
                    char* tmp = static_cast<char*>(
                        calloc(pdu->variables->val_len + 1,
                               sizeof(char)));
                    memcpy(tmp, pdu->variables->val.string,
                           pdu->variables->val_len);
                    value = tmp;
                    KLKFREE(tmp);
                    break;
                }
                case ASN_INTEGER:
                {
                    int i = *(pdu->variables->val.integer);
                    value = boost::lexical_cast<std::string>(i);
                    break;
                }
                default:
                    KLKASSERT(0);
                    break;
                }
                klk_log(KLKLOG_DEBUG, "SNMP got a trap. OID: %s. Value: %s",
                        oid.c_str(), value.c_str());
                m_traps.push_back(test::TrapInfo(oid, value));
                snmp_free_pdu(pdu);
                pdu = NULL;
            }

        }
        catch(const boost::bad_lexical_cast&)
        {
            throw klk::Exception(__FILE__, __LINE__, err::BADLEXCAST);
        }
    }

    snmp_sess_close(opaque);

    KLKFREE(peername);
    KLKFREE(community);
}

//
// SNMPReceiver class
//

// Constructor
SNMPReceiver::SNMPReceiver() :
    m_thread(new SNMPReceiverThread()), m_scheduler()
{
}

// Destructor
SNMPReceiver::~SNMPReceiver()
{
}

// Starts the receiver
void SNMPReceiver::start()
{
    m_scheduler.startThread(m_thread);
}

// Stops the receiver
void SNMPReceiver::stop() throw()
{
    m_scheduler.stop();
}

