/**
   @file snmp.cpp
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
   - 2008/11/20 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <netdb.h>

#include <boost/lexical_cast.hpp>

#include "snmp.h"
#include "log.h"
#include "exception.h"
#include "binarydata.h"

using namespace klk;
using namespace snmp;

/**
   Enterprise OID (default)
*/
static oid objid_enterprise[] = { 1, 3, 6, 1, 4, 1, 3, 1, 1 };

//
// TrapInfoBase class
//

// Constructor
// @param[in] oid - the object ID in text form
TrapInfoBase::TrapInfoBase(const std::string& objid) : m_oid_len(MAX_OID_LEN)
{
    KLKASSERT(sizeof(objid_enterprise) <= sizeof(m_oid));
    oid* rc = NULL;
    if (!objid.empty())
    {
        rc = snmp_parse_oid(objid.c_str(), m_oid, &m_oid_len);
    }

    if (!rc)
    {
        klk_log(KLKLOG_ERROR,
                "Incorrect oid: '%s'. The default enterprise "
                "OID will be used", objid.c_str());

        memcpy(m_oid, objid_enterprise,
               sizeof(objid_enterprise));
        m_oid_len =
            sizeof(objid_enterprise) / sizeof(oid);
    }
}

//
// TrapInfo class
//

// Constructor
TrapInfo::TrapInfo(const std::string& objid, const std::string& value) :
    TrapInfoBase(objid), m_value(value), m_type('s')
{
}

// Constructor
TrapInfo::TrapInfo(const std::string& objid, int value) :
    TrapInfoBase(objid), m_value(), m_type('i')
{
    try
    {
        m_value = boost::lexical_cast<std::string>(value);
    }
    catch(boost::bad_lexical_cast&)
    {
        KLKASSERT(0);
    }
}

// Adds the trap info to the SNMP PDU(protocol data unit)
// @param[in] pdu - the PDU that will be updated
Result TrapInfo::add2PDU(netsnmp_pdu* pdu) const
{
    CHECKNOTNULL(pdu);

    // Generic trap number
    // A number in the range 0-6.
    // The true generic traps have numbers 0-5;
    //if you're sending an enterprise-specific trap, set this number to 6
    pdu->trap_type = 6;

    // Specific trap number
    // A number indicating the specific trap you want to send.
    // If you're sending a generic trap, this parameter is
    // ignoredyou're probably better off setting it to zero.
    // If you're sending a specific trap, the trap number is up to you.
    // For example, if you send a trap with the OID
    //.1.3.6.1.4.1.2500.3003.0, 3003 is the specific trap number.
    pdu->specific_type = m_oid[m_oid_len - 1];

    pdu->enterprise = static_cast<oid*>(calloc(m_oid_len, sizeof(oid)));
    memcpy(pdu->enterprise, m_oid, m_oid_len * sizeof(oid));
    pdu->enterprise_length = m_oid_len;

    if (snmp_add_var(pdu, m_oid, m_oid_len, m_type, m_value.c_str()))
    {
        klk_log(KLKLOG_ERROR,
                "Failed to add trap value to PDU. Type: '%c'; Value: '%s'",
                m_type, m_value.c_str());
        return ERROR;
    }

    return OK;
}


/**
   SNMP ident
 */
const char* SNMPIDENT = "klkapp";

/**
   SNMP input callback
 */
static int snmp_input(int operation, netsnmp_session * session,
                      int reqid, netsnmp_pdu *pdu, void *magic)
{
    return 1;
}

//
// SNMP class
//

// The constructor
// @param[in] config - the config info
SNMP::SNMP(IConfig* config) :
    Mutex(), m_config(config)
{
    // init snmp
    init_snmp(SNMPIDENT);

    snmp_enable_stderrlog();
    //snmp_enable_syslog_ident(SNMPIDENT, LOG_LOCAL7);

}

// Destructor
SNMP::~SNMP()
{
    // terminate snmp
    snmp_shutdown(SNMPIDENT);
}

// Sends a string trap
void SNMP::sendTrap(const std::string& objid, const std::string& value)
{
    TrapInfo trapInfo(objid, value);
    sendTrap(trapInfo);
}

// Sends an integer trap
void SNMP::sendTrap(const std::string& objid, int value)
{
    TrapInfo trapInfo(objid, value);
    sendTrap(trapInfo);
}

// Sends a trap
void SNMP::sendTrap(const TrapInfo& trap)
{
    if (!m_config)
    {
        assert(0);
        return;
    }

    netsnmp_session session, *ss = NULL;
    netsnmp_pdu *pdu = NULL;
    char* receiver = NULL;
    in_addr_t* agent_addr = NULL;
    char* community = NULL;

    // net-snmp is not thread safe for us :(
    // http://www.net-snmp.org/wiki/index.php/FAQ:General_17
    Locker lock(this);
    try
    {
        snmp_sess_init(&session);
        session.version = SNMP_VERSION_1; // hardcoded
        community = strdup(m_config->getSNMPInfo()->getCommunity().c_str());
        BOOST_ASSERT(community);
        session.community = reinterpret_cast<u_char*>(community);
        session.community_len = strlen(community);

        receiver = strdup(m_config->getSNMPInfo()->getReceiver().c_str());
        BOOST_ASSERT(receiver);
        session.peername = receiver; // hostname

        session.callback = snmp_input;
        session.callback_magic = NULL;

        netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID,
                           NETSNMP_DS_LIB_DEFAULT_PORT,
                           SNMP_TRAP_PORT);

        if( (ss = snmp_open(&session)) == NULL )
        {
            snmp_sess_perror("snmptrap", &session);
            throw Exception(__FILE__, __LINE__,
                                 "Error in snmp_open()");
        }

        pdu = snmp_pdu_create(SNMP_MSG_TRAP);

        if (trap.add2PDU(pdu) != OK)
        {
            BOOST_ASSERT(false);
        }

        agent_addr = getAgentAddr();
        if (agent_addr == NULL)
        {
            throw Exception(__FILE__, __LINE__,
                                 "Failed to retrive agent addr");
        }

        memcpy(pdu->agent_addr, agent_addr, sizeof(pdu->agent_addr));
        pdu->time = get_uptime();

        // send it
        // The call to snmp_send() deallocates pdu for us,
        // so it would be intead of snmp_free_pdu()
        if(!snmp_send(ss, pdu))
        {
            snmp_sess_perror("snmptrap", ss);
            snmp_free_pdu(pdu);
            throw Exception(__FILE__, __LINE__,
                                 "Error in snmp_send()");
        }


    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Error while sending a trap: %s",
                err.what());
    }

    // clean up
    KLKFREE(receiver);
    KLKFREE(agent_addr);
    KLKFREE(community);
    snmp_close(ss);
}

// Retrivies agent addr (aka localhost)
in_addr_t* SNMP::getAgentAddr()
{
#if HAVE_GETHOSTBYNAME_R == 1
    struct hostent he, *phe = NULL;
    BinaryData buff(2048);
    int fnord = 0;
    if (gethostbyname_r("localhost", &he,
                        static_cast<char*>(buff.toVoid()),
                        buff.size(), &phe, &fnord))
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in gethostbyname_r(): %s",
                        fnord, strerror(fnord));
    }
#elif HAVE_GETHOSTBYNAME_R == 0
    struct hostent *phe = ::gethostbyname("localhost");
#else
#error "No have gethostbyname_r check"
#endif
    if (phe == NULL)
    {
        assert(0);
        return NULL;
    }

    struct sockaddr_in saddr;
    memcpy(&saddr.sin_addr, phe->h_addr, phe->h_length);
    in_addr_t* result = static_cast<in_addr_t*>(calloc(sizeof(in_addr_t), 1));
    if (result == NULL)
    {
        klk_log(KLKLOG_ERROR, "Memory allocation error");
        return NULL;
    }
    memcpy(result, &saddr.sin_addr.s_addr, sizeof(in_addr_t));
    return result;
}
