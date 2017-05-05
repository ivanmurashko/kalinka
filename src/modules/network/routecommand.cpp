/**
   @file routecommand.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include "routecommand.h"
#include "defines.h"
#include "exception.h"
#include "netdev.h"
#include "network.h"
#include "utils.h"
#include "db.h"

using namespace klk;
using namespace klk::net;

//
// RouteListCommand class
//

const std::string ROUTE_LIST_SUMMARY = "Shows a list with network route info";
const std::string ROUTE_LIST_USAGE = "Usage: " + MODNAME +
    + " " + ROUTE_LIST_NAME + "[route name|" + cli::ALL + "]\n";

// Constructor
RouteListCommand::RouteListCommand() :
    cli::Command(ROUTE_LIST_NAME, ROUTE_LIST_SUMMARY, ROUTE_LIST_USAGE)
{
}

// Process the command
// @param[in] params - the parameters
const std::string
RouteListCommand::process(const cli::ParameterVector& params)
{
    std::string route_name = cli::ALL;
    if (params.size() > 1)
    {
        throw Exception(__FILE__, __LINE__,
                             "Incorrect parameter name. %s",
                             getUsage().c_str());
    }
    if (params.size() == 1)
    {
        route_name = params[0];
    }

    // retrive the route info list
    RouteInfoList list = getModule<Network>()->getRouteInfo();
    std::stringstream result;

    const std::string name_head("name");
    const std::string dev_head("dev");
    const std::string addr_head("address");
    const std::string proto_head("protocol");
    const std::string info_head("info");


    size_t max_name = name_head.size();
    size_t max_dev = dev_head.size();
    size_t max_addr = addr_head.size();
    size_t max_proto = proto_head.size();

    bool was = false;
    if (route_name == cli::ALL)
        was = true;

    for (RouteInfoList::iterator item = list.begin();
         item != list.end(); item++)
    {
        if ((*item)->getName() == route_name)
        {
            was = true;
        }
        max_name = std::max(max_name, (*item)->getName().size());
        max_dev = std::max(
            max_dev,
            (*item)->getDev()->getStringParam(dev::NAME).size());
        max_addr = std::max(max_addr,
                            (*item)->getHost().size() +
                            (*item)->getPort().size() + 1);
    }

    if (was == false)
    {
        throw Exception(__FILE__, __LINE__,
                             "Unknown route name: %s",
                             route_name.c_str());
    }

    max_dev += 2;
    max_addr += 2;
    max_name += 2;
    max_proto += 2;

    result << "Routes list: \n";
    result << base::Utils::align(name_head, max_name);
    result << base::Utils::align(dev_head, max_dev);
    result << base::Utils::align(addr_head, max_addr);
    result << base::Utils::align(proto_head, max_proto);
    result << info_head << "\n";
    for (RouteInfoList::iterator info = list.begin();
         info != list.end(); info++)
    {
        const std::string name = (*info)->getName();

        if (name != route_name && route_name != cli::ALL)
            continue;

        result << base::Utils::align(name, max_name);
        const std::string devname =
            (*info)->getDev()->getStringParam(dev::NAME);
        result << base::Utils::align(devname, max_dev);
        const std::string addr = (*info)->getHost() + ":" + (*info)->getPort();
        result << base::Utils::align(addr, max_addr);

        if ((*info)->getProtocol() == TCPIP)
        {
            result << base::Utils::align(TCPIP_NAME, max_proto);
        }
        else if ((*info)->getProtocol() == UDP)
        {
            result << base::Utils::align(UDP_NAME, max_proto);
        }
        else
        {
            throw Exception(__FILE__, __LINE__,
                                 "Unknown protocol with uuid: %s",
                                 (*info)->getProtocol().c_str());
        }

        if ((*info)->isInUse())
        {
            result << "in use";
        }

        result << "\n";
    }
    return result.str();
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
RouteListCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;
    if (setparams.empty())
    {
        res.push_back(cli::ALL);
        RouteInfoList list = getModule<Network>()->getRouteInfo();
        for (RouteInfoList::iterator item = list.begin();
             item != list.end(); item++)
        {
            res.push_back((*item)->getName());
        }
    }

    return res;
}


//
// RouteAddCommand class
//

const std::string ROUTE_ADD_SUMMARY = "Adds a network route info";
const std::string ROUTE_ADD_USAGE = "Usage: " + MODNAME +
                      " " + ROUTE_ADD_NAME +
                      " [route name] [resource name]" +
                      " [host] [port] [protocol]\n";

// Constructor
RouteAddCommand::RouteAddCommand() :
    cli::Command(ROUTE_ADD_NAME, ROUTE_ADD_SUMMARY, ROUTE_ADD_USAGE),
    m_dev(), m_proto(TCPIP)
{
}

/**
   Helper functor to find a route info by route content
*/
struct FindRouteInfoByContent
{
    bool operator()(const RouteInfoPtr& info,
                    const std::string& host,
                    const std::string& port,
                    const IDevPtr& dev)
    {
        BOOST_ASSERT(info);
        BOOST_ASSERT(dev);
        if (info->getHost() == host &&
            info->getPort() == port &&
            info->getDev()->getStringParam(dev::UUID) ==
            dev->getStringParam(dev::UUID))
        {
            return true;
        }

        return false;
    }
};


// Process the command
// @param[in] params - the parameters
const std::string
RouteAddCommand::process(const cli::ParameterVector& params)
{


    if (params.size() != 4 && params.size() != 5)
    {
        throw Exception(__FILE__, __LINE__,
                             "Incorrect number of parameters (%d). "
                             "Should be 4 or 5. %s",
                             params.size(), getUsage().c_str());
    }

    // check name
    const std::string name = params[0];
    checkName(name);

    // check the dev
    m_dev.reset();
    checkDev(params[1]);
    BOOST_ASSERT(m_dev);
    // check settings
    const std::string host = params[2];
    checkHost(host);
    const std::string port = params[3];
    if (isPortValid(host, port) == false)
    {
        throw Exception(__FILE__, __LINE__,
                             "Incorrect port value: " + port);
    }

    if (params.size() == 5)
    {
        checkProtocol(params[4]);
    }

    // check that we don't have such info
    RouteInfoList list = getModule<Network>()->getRouteInfo();

    RouteInfoList::iterator
        test = std::find_if(list.begin(), list.end(),
                        boost::bind<bool>(FindRouteInfoByContent(),
                                          _1, boost::ref(host),
                                          boost::ref(port),
                                          m_dev));
    if (test != list.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "Route %s:%s has been already assigned on %s "
                             "with name '%s'",
                             host.c_str(), port.c_str(),
                             m_dev->getStringParam(dev::NAME).c_str(),
                             (*test)->getName().c_str());
    }

    // add it to the DB
    db::DB db(getFactory());
    db.connect();

    //  `klk_network_route_add` (
    // INOUT route VARCHAR(40),
    // IN name ...
    // IN address VARCHAR(40),
    // IN host VARCHAR(50),
    // IN port INTEGER,
    // OUT return_value INT
    db::Parameters dbparams;
    dbparams.add("@route");
    dbparams.add("@name", name);
    dbparams.add("@address", m_dev->getStringParam(dev::NETADDRUUID));
    dbparams.add("@host", host);
    dbparams.add("@port", port);
    dbparams.add("@proto", m_proto);
    dbparams.add("@return");

    db::Result res = db.callSimple("klk_network_route_add", dbparams);
    if (res["@return"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Failed to add a route %s:%s on %s. "
                             "DB error",
                             host.c_str(), port.c_str(),
                             m_dev->getStringParam(dev::NAME).c_str());
    }

    std::stringstream out;
    out << "Route " << name << " (" << host << ":" << port << ") ";

    if (m_proto == TCPIP)
    {
        out << "(" << TCPIP_NAME << ")";
    }
    else if (m_proto == UDP)
    {
        out << "(" << UDP_NAME << ")";
    }
    else
    {
        BOOST_ASSERT(false);
    }
    out << " on ";
    out <<  m_dev->getStringParam(dev::NAME) << " has been added\n";
    return out.str();
}

// Check route name to be added
void RouteAddCommand::checkName(const std::string& name)
{
    BOOST_ASSERT(name.empty() == false);
    RouteInfoList list = getModule<Network>()->getRouteInfo();

    RouteInfoList::iterator
        test = std::find_if(list.begin(), list.end(),
                            boost::bind(mod::FindInfoByName(), _1, name));
    if (test != list.end())
    {
        throw Exception(
            __FILE__, __LINE__,
            "Route '%s' has been already assigned on "
            "'%s:%s at %s'",
            name.c_str(),
            (*test)->getHost().c_str(),
            (*test)->getPort().c_str(),
            (*test)->getDev()->getStringParam(dev::NAME).c_str());
    }
}

// Check dev value to be added
void RouteAddCommand::checkDev(const std::string& dev)
{
    IDevList list =
        getFactory()->getResources()->getResourceByType(dev::NET);
    if (list.empty())
    {
        throw Exception(__FILE__, __LINE__,
                             "There is no any network card configured. "
                             "Please use resources detect command "
                             "to detect them and restart the server");
    }

    try
    {
        m_dev = getFactory()->getResources()->getResourceByName(dev);
    }
    catch(Exception&)
    {
        std::stringstream msg;
        msg << "Unknown network devices: " << dev <<
            ".\nPossible values: ";
        for (IDevList::iterator i = list.begin(); i != list.end(); i++)
        {
            if (i != list.begin())
            {
                msg << "; ";
            }
            msg << (*i)->getStringParam(dev::NAME);
        }
        throw Exception(__FILE__, __LINE__, msg.str());
    }
    catch(...)
    {
        BOOST_ASSERT(false);
    }
}

// Check host value to be added
void RouteAddCommand::checkHost(const std::string& host)
{
    m_proto = TCPIP;
    BOOST_ASSERT(host.empty() == false);
    BOOST_ASSERT(m_dev);
    if (host == m_dev->getStringParam(dev::NETADDR))
    {
        // the host value is OK
        return;
    }

    // we assign multicast if the interface support it or
    // in the case of loopback (for tests only)
    if (getModule<Network>()->isMulticast(host) &&
        (m_dev->hasParam(dev::NETMCAST) == false) &&
        (m_dev->hasParam(dev::NETLOOP) == false))
    {
        throw Exception(__FILE__, __LINE__,
                             "Dev %s does not support multicast",
                             m_dev->getStringParam(dev::NAME).c_str());
    }

    if (getModule<Network>()->isMulticast(host))
    {
        m_proto = UDP;
    }
    else
    {
        throw Exception(__FILE__, __LINE__,
                             "Host value '%s' is invalid. "
                             "The following address are allowed for '%s': "
                             "224.0.0.0 - 239.255.255.255 or %s",
                             host.c_str(),
                             m_dev->getStringParam(dev::NAME).c_str(),
                             m_dev->getStringParam(dev::NETADDR).c_str());
    }
}

// Check protocol value to be added
void RouteAddCommand::checkProtocol(const std::string& protocol)
{
    if (protocol == TCPIP_NAME && m_proto == UDP)
    {
        throw Exception(__FILE__, __LINE__,
                             "Only %s protocol is allowed for the host",
                             UDP_NAME.c_str());
    }

    if (protocol == TCPIP_NAME)
    {
        m_proto = TCPIP;
    }
    else if (protocol == UDP_NAME)
    {
        m_proto = UDP;
    }
    else
    {
        throw Exception(
            __FILE__, __LINE__,
            "Unsupported network protocol '%s'. Possuble values: %s, %s",
            protocol.c_str(),
            TCPIP_NAME.c_str(), UDP_NAME.c_str());
    }
}



// Check port value to be added
bool RouteAddCommand::isPortValid(const std::string& host,
                                  const std::string& port)
{
    BOOST_ASSERT(port.empty() == false);

    // check that the port is an integer
    try
    {
        boost::lexical_cast<u_int>(port);
    }
    catch(boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__,
                             "Incorrect port value: %s. Should be integer",
                             port.c_str());
    }


    // check that the port for the host is not used
    if (getModule<Network>()->isMulticast(host))
        return true; // FIXME!!!

    RouteInfoList list = getModule<Network>()->getRouteInfo();
    for (RouteInfoList::iterator item = list.begin();
         item != list.end(); item++)
    {
        if (port == (*item)->getPort() && (*item)->getHost() == host)
            return false; // already in use
    }

    return true;
}

/**
   Helper functor to find an info by hostname
*/
struct FindInfoByHost :
    public std::binary_function<RouteInfoPtr, std::string, bool>
{
    bool operator()(const RouteInfoPtr& info,
                    const std::string& name)
        {
            BOOST_ASSERT(info);
            return (info->getHost() == name);
        }
};


// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
RouteAddCommand::getCompletion(const cli::ParameterVector& setparams)
{
    // [route name] [resource name]
    // [host] [port] [protocol]
    cli::ParameterVector res;
    if (setparams.empty()) // route name
    {
        // try to suggest a not used name
        RouteInfoList list = getModule<Network>()->getRouteInfo();

        // FIXME!!! seems that can not be more than 10000 routes assigned
        for (int i = 1; i < 10000; i++)
        {
            std::stringstream name;
            name << "route" << i;
            RouteInfoList::iterator
                test = std::find_if(list.begin(), list.end(),
                                    boost::bind(mod::FindInfoByName(),
                                                _1, name.str()));
            if (test == list.end())
            {
                res.push_back(name.str());
                break;
            }
        }
    }
    else if (setparams.size() == 1) // resource name
    {
        IDevList list =
            getFactory()->getResources()->getResourceByType(dev::NET);
        for (IDevList::iterator i = list.begin(); i != list.end(); i++)
        {
            res.push_back((*i)->getStringParam(dev::NAME));
        }
    }
    else if (setparams.size() == 2) // host
    {
        m_dev.reset();
        checkDev(setparams[1]);
        // set the dev name host
        BOOST_ASSERT(m_dev);
        res.push_back(m_dev->getStringParam(dev::NETADDR));
        // try to add several multicast addresses
        RouteInfoList list = getModule<Network>()->getRouteInfo();
        for (int i = 1; i < 255; i++)
        {
            std::stringstream name;
            name << "239.10.10." << i;
            RouteInfoList::iterator
                test = std::find_if(list.begin(), list.end(),
                                    boost::bind(FindInfoByHost(),
                                                _1, name.str()));
            if (test == list.end())
            {
                res.push_back(name.str());
                break;
            }
        }
    }
    else if (setparams.size() == 3) // port
    {
        const std::string host = setparams[2];
        for (int i = 20000; i < 30000; i++)
        {
            std::stringstream port;
            port << i;
            if (isPortValid(host, port.str()))
            {
                res.push_back(port.str());
                break;
            }
        }
    }
    else if (setparams.size() == 4) // protocol
    {
        const std::string host = setparams[2];
        if (!getModule<Network>()->isMulticast(host))
        {
            res.push_back(TCPIP_NAME);
        }
        res.push_back(UDP_NAME);
    }

    return res;
}


//
// RouteDelCommand class
//

const std::string ROUTE_DEL_SUMMARY = "Deletes a network route info";
const std::string ROUTE_DEL_USAGE = "Usage: " + MODNAME +
    + " " + ROUTE_DEL_NAME + " [route name]\n";

// Constructor
RouteDelCommand::RouteDelCommand() :
    cli::Command(ROUTE_DEL_NAME, ROUTE_DEL_SUMMARY, ROUTE_DEL_USAGE)
{
}

// Process the command
// @param[in] params - the parameters
const std::string
RouteDelCommand::process(const cli::ParameterVector& params)
{
    const std::string names = getRouteNames();
    if (names.empty())
    {
        throw Exception(__FILE__, __LINE__,
                             "You can not delete any route");
    }

    if (params.size() != 1)
    {
        throw Exception(__FILE__, __LINE__,
                             "Incorrect number of parameters (%d). "
                             "Should be one - route name, where route name "
                             "one from the following: %s",
                             params.size(),
                             names.c_str());
    }

    // check the name
    const std::string name = params[0];

    // check that we don't have such info
    RouteInfoList list = getModule<Network>()->getRouteInfo();
    RouteInfoList::iterator
        test = std::find_if(list.begin(), list.end(),
                            boost::bind(mod::FindInfoByName(),
                                        _1, name));
    if (test == list.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "No such route '%s'. Possible values: %s",
                             name.c_str(),
                             names.c_str());
    }

    if ((*test)->isInUse())
    {
        throw Exception(__FILE__, __LINE__,
                             "Route '%s' is used and "
                             "can not be deleted. Possible values: %s",
                             name.c_str(),
                             names.c_str());
    }

    // we can delete it

    db::DB db(getFactory());
    db.connect();

    // `klk_network_route_delete` (
    // IN route VARCHAR(40),
    // OUT return_value INTEGER
    db::Parameters dbparams;
    dbparams.add("@route", (*test)->getUUID());
    dbparams.add("@return");

    db::Result res = db.callSimple("klk_network_route_delete", dbparams);
    if (res["@return"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Failed to delete a route '%s'. "
                             "DB error",
                             name.c_str());
    }

    std::stringstream out;
    out << "Route '" << name  << "' has been deleted\n";
    return out.str();
}

// Gets the command usage description
const std::string RouteDelCommand::getUsage() const
{
    std::string usage = ROUTE_DEL_USAGE;
    return usage;
}

// Retrives a string with route names
const std::string RouteDelCommand::getRouteNames() const
{
    StringList names =
        getModule<Network>()->getValues(mod::SELECT_NOT_USED,
                                        mod::NAME);
    return base::Utils::convert2String(names, ", ");
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
RouteDelCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;
    if (setparams.empty())
    {
        StringList list =
            getModule<Network>()->getValues(mod::SELECT_NOT_USED,
                                            mod::NAME);
        std::copy(list.begin(), list.end(),
                  std::back_inserter(res));
    }

    return res;
}
