/**
   @file config.cpp
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
   - 2008/08/30 created by ipp (Ivan Murashko)
   - 2009/01/04 changed all error processing to exceptions
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include <fstream>

#include "klkconfig.h"
#include "common.h"
#include "log.h"
#include "utils.h"
#include "db.h"
#include "paths.h"
#include "exception.h"
#include "options.h"

using namespace klk;

//
// Option class
//

// Constructor
Option::Option(const std::string& name,
               const std::string& summary,
               const std::string& description,
               const std::string& defvalue,
               OptionProcessor processor) :
    m_name(name), m_summary(summary), m_description(description),
    m_value(defvalue),
    m_processor(processor)
{
}


// Sets the option's value
// @param[in] value - the value to be set
void Option::setValue(const std::string& value)
{
    BOOST_ASSERT(value.empty() == false);
    try
    {
        m_processor(value);
    }
    catch(Exception& err)
    {
        std::string descr = "Failed to set option '" + m_name + "' to '" +
            value + "': " + err.what();
        klk_log(KLKLOG_ERROR, descr.c_str());
        if (!m_value.empty())
        {
            // restore prev. state
            m_processor(m_value);
        }
        throw Exception(__FILE__, __LINE__, descr);
    }

    m_value = value;
}


//
// DBInfo class
//

// Constructor
DBInfo::DBInfo() :
    m_host(""), m_dbname(""), m_user(""), m_pwd(""),
    m_port(0)
{
}

// gets DB hostname
const std::string DBInfo::getHost() const throw()
{
    KLKASSERT(!m_host.empty());
    return m_host;
}

// gets DB port
// @return the value
u_int DBInfo::getPort() const throw()
{
    //KLKASSERT(m_port != 0);
    return m_port;
}

// gets DB name
const std::string DBInfo::getDBName() const throw()
{
    KLKASSERT(!m_dbname.empty());
    return m_dbname;
}

// gets user account name
const std::string DBInfo::getUserName() const throw()
{
    KLKASSERT(!m_user.empty());
    return m_user;
}

// gets password
const std::string DBInfo::getUserPwd() const throw()
{
    return m_pwd;
}

// Sets db addr option values
void DBInfo::setDBAddr(const std::string& opt)
{
    BOOST_ASSERT(opt.empty() == false);
    size_t size = opt.size();

    BinaryData specific_ptr(size);
    char *specific = static_cast<char*>(specific_ptr.toVoid());
    BinaryData hostinfo_ptr(size);
    char *hostinfo = static_cast<char*>(hostinfo_ptr.toVoid());
    BinaryData hostname_ptr(size);
    char *hostname = static_cast<char*>(hostname_ptr.toVoid());
    BinaryData auth_ptr(size);
    char *auth = static_cast<char*>(auth_ptr.toVoid());
    BinaryData dbname_ptr(size);
    char *dbname = static_cast<char*>(dbname_ptr.toVoid());
    BinaryData dbhost_ptr(size);
    char *dbhost = static_cast<char*>(dbhost_ptr.toVoid());
    BinaryData dbuser_ptr(size);
    char *dbuser = static_cast<char*>(dbuser_ptr.toVoid());
    BinaryData dbpwd_ptr(size);
    char *dbpwd = static_cast<char*>(dbpwd_ptr.toVoid());

    auth[0] = 0;
    specific[0]=0;
    hostinfo[0]=0;
    hostname[0]=0;
    dbname[0]=0;
    dbuser[0]=0;
    dbpwd[0]=0;
    dbhost[0]=0;

    switch (sscanf(opt.c_str(), "%[^/]/%s", hostinfo, dbname))
    {
    case 1:
        break;
    case 2:
	char *ch;
	if ((ch = strrchr(dbname, '/')))
	    *ch = '\0';
	break;
    default:
        return;
    }

    switch (sscanf(hostinfo, "%[^@]@%s", auth, hostname))
    {
    case 1:
	strcpy(hostname, auth);
	break;
    case 2:
	sscanf(auth, "%[^:]:%s", dbuser, dbpwd);
	break;
    default:
        break;
    }

    sscanf(hostname, "%[^:]:%d", dbhost, &m_port);

    if(dbname[0])
    {
        m_dbname = dbname;
        boost::trim_if(m_dbname,  boost::is_any_of(" \r\n\t"));
    }

    if(dbuser[0])
    {
        m_user = dbuser;
        boost::trim_if(m_user,  boost::is_any_of(" \r\n\t"));
    }

    if(dbpwd[0])
    {
        m_pwd = dbpwd;
        boost::trim_if(m_pwd,  boost::is_any_of(" \r\n\t"));
    }

    if(dbhost[0])
    {
        m_host = dbhost;
        boost::trim_if(m_host,  boost::is_any_of(" \r\n\t"));
    }
}

//
// SNMPInfo class
//

// Constructor
SNMPInfo::SNMPInfo() :
    m_receiver("localhost"), m_community("public"), m_port(SNMP_TRAP_PORT)
{
}

// Sets receiver
// @param[in] receiver - the value to be set
void SNMPInfo::setReceiver(const std::string& receiver)
{
    BOOST_ASSERT(receiver.empty() == false);
    m_receiver = receiver;
    try
    {
        std::size_t pos = m_receiver.find(':');
        if (pos != std::string::npos)
        {
            std::string port = m_receiver.substr(pos + 1);
            m_port = boost::lexical_cast<int>(port);

        }
        else
        {
            m_port = SNMP_TRAP_PORT;
        }
    }
    catch(boost::bad_lexical_cast&)
    {
        std::string descr = "Could not get port from '" + m_receiver +
            "'. Default value will be used";
        klk_log(KLKLOG_ERROR, descr.c_str());
        m_port = SNMP_TRAP_PORT;
        throw Exception(__FILE__, __LINE__, descr);
    }
}

// Sets community
// @param[in] community - the value to be set
void SNMPInfo::setCommunity(const std::string& community)
{
    BOOST_ASSERT(community.empty() == false);
    m_community = community;
}

/**
   Max config option length
*/
const int MAX_CONFIG_OPTION_LEN = 1024;

//
// Config class
//

// Constructor
Config::Config() :
    Mutex(),
    m_hostname(""), m_config_path(dir::CFG),
    m_dbinfo(), m_snmpinfo(), m_options()
{
}

// Destructor
Config::~Config()
{
}

// Sets config file path
// @param[in] path - the path to be set
void Config::setPath(const std::string& path)
{
    BOOST_ASSERT(path.empty() == false);
    Locker lock(this);
    m_config_path = path;
}

// Gets config file path
const std::string Config::getPath() const throw()
{
    KLKASSERT(!m_config_path.empty());
    return m_config_path;
}

struct IsOptionMatch
{
    bool operator()(
        const IOptionPtr& option,
        const std::string& line) const
        {
            int res = strncasecmp(option->getName().c_str(),
                                  line.c_str(),
                                  option->getName().size());
            return (res == 0);
        }
};

// Sets an option value
void Config::setOptionValue(std::string value)
{
    OptionList::iterator opt = std::find_if(
        m_options.begin(),
        m_options.end(),
        boost::bind<bool>(IsOptionMatch(), _1, value));
    if (opt == m_options.end())
    {
        klk_log(KLKLOG_ERROR, "Unknown option at '%s': %s",
                value.c_str(), m_config_path.c_str());
        return;
    }

    boost::erase_head(value, (*opt)->getName().size());
    boost::trim_if(value,  boost::is_any_of(" \r\n\t"));
    (*opt)->setValue(value);
}

// Loads config from the file
// @param[in] filename the file name with config info
void Config::load()
{
    Locker lock(this);
    // fill option list first
    // FIXME!!!
    fillOptionList();

    BOOST_ASSERT(m_config_path.empty() == false);
    BOOST_ASSERT(base::Utils::fileExist(m_config_path.c_str()) == true);

    const StringList list = base::Utils::getConfig(m_config_path);
    std::for_each(list.begin(), list.end(),
                  boost::bind(&Config::setOptionValue, this, _1));

    // setting default hostname
    if (m_hostname.empty())
    {
        char buff[512];
        if (gethostname(buff, sizeof(buff) - 1) < 0)
        {
            klk_log(KLKLOG_ERROR, "Error %d in gethostname(): %s",
                    errno, strerror(errno));
            throw Exception(__FILE__, __LINE__, "Error in gethostname()");
        }
        m_hostname = buff;

        klk_log(KLKLOG_ERROR,
                "Config '%s' does not contain hostname option. "
                "%s will be used.",
                m_config_path.c_str(),
                m_hostname.c_str());
    }
}

// gets mediaserver host name
const std::string Config::getHostName() const throw()
{
    KLKASSERT(!m_hostname.empty());
    return m_hostname;
}

// Saves config to a file
void Config::save()
{
    BOOST_ASSERT(m_options.empty() == false);
    BOOST_ASSERT(m_config_path.empty() == false);

    std::ofstream config;

    config.open(m_config_path.c_str(), std::ofstream::out);

    if (config.fail())
    {
        klk_log(KLKLOG_ERROR, "Error %d in open(): %s",
                errno, strerror(errno));
        throw Exception(__FILE__, __LINE__, "Error in open()");
    }

    // header
    config
        << "#\n" << "# Kalinka configuration file\n"
        << "# Automatically generated at "
        << base::Utils::getCurrentTime("%Y-%m-%d %H:%M:%S")
        << "\n#\n\n";

    for (OptionList::iterator i = m_options.begin();
         i != m_options.end(); i++)
    {
        config << "#\n";
        config << "# " << (*i)->getSummary() << "\n";
        config << "# "
               << base::Utils::replaceAll(
                   (*i)->getDescription(), "\n", "\n#") << "\n";
        config << "#\n";
        config << (*i)->getName() << "   " << (*i)->getValue();
        config << "\n\n";
    }

    config.close();
}

// Fills options
void Config::fillOptionList() throw()
{
    if (!m_options.empty())
        return;

    IOptionPtr option;

    option = IOptionPtr(
        new Option(
            conf::DBADDR,
            "Database connection setup",
            "Usage :\n"
            "       DBUser[:DBPass]@]DBHost[:DBPort]]/DBName/",
            "test:test@localhost/kalinka",
            boost::bind(&DBInfo::setDBAddr, &m_dbinfo, _1)));
    m_options.push_back(option);

    option = IOptionPtr(
        new Option(
            conf::HOSTNAME,
            "Sets mediaserver host name",
            "Format :\n"
            "       string",
            "testserver",
            boost::bind(&Config::setHostName, this, _1)));
    m_options.push_back(option);

    option = IOptionPtr(
        new Option(
            conf::SNMPRECEIVER,
            "Sets SNMP trap receiver address",
            "Format :\n"
            "       addr[:port]",
            "localhost",
            boost::bind(&SNMPInfo::setReceiver, &m_snmpinfo, _1)));
    m_options.push_back(option);

    option = IOptionPtr(
        new Option(
            conf::SNMPCOMMUNITY,
            "Sets SNMP trap community name",
            "Format :\n"
            "       string",
            "public",
            boost::bind(&SNMPInfo::setCommunity, &m_snmpinfo, _1)));
    m_options.push_back(option);
}

// Sets mediaserver host name
void Config::setHostName(const std::string& value)
{
    // private method protected at the Config::load
    // no necessary to lock it
    BOOST_ASSERT(value.empty() == false);
    m_hostname = value;
}
