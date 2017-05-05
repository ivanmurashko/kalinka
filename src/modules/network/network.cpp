/**
   @file network.cpp
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
   - 2008/08/22 created by ipp (Ivan Murashko)
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

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <vector>

#include "network.h"
#include "common.h"
#include "log.h"
#include "defines.h"
#include "exception.h"
#include "dev.h"
#include "routecommand.h"
#include "netdev.h"
#include "messages.h"
#include "db.h"

using namespace klk;
using namespace klk::net;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new Network(factory));
}

//
// Network
//

// Constructor
// @param[in] factory the module factory
Network::Network(IFactory *factory) :
    ModuleWithInfo<RouteInfo>(factory, MODID,
                              msg::id::NETSTART,
                              msg::id::NETSTOP)
{
}

// Destructor
Network::~Network()
{
}

// Gets a human readable module name
// @return the name
const std::string Network::getName() const throw()
{
    return MODNAME;
}

// Register all processors
void Network::registerProcessors()
{
    ModuleWithInfo<RouteInfo>::registerProcessors();
    registerSync(
        msg::id::NETLIST,
        boost::bind(&Network::doList, this, _1, _2));
    registerSync(
        msg::id::NETINFO,
        boost::bind(&Network::doInfo, this, _1, _2));

    registerCLI(cli::ICommandPtr(new RouteListCommand()));
    registerCLI(cli::ICommandPtr(new RouteAddCommand()));
    registerCLI(cli::ICommandPtr(new RouteDelCommand()));

    registerTimer(boost::bind(&Network::updateLock, this),
                  LOCK_UPDATE_INTERVAL);

    registerDBUpdateMessage(UPDATEDB_MESSAGE);
}

// Do the start
void Network::doStart(const IMessagePtr& in, const IMessagePtr& out)

{
    BOOST_ASSERT(out);
    const RouteInfoPtr info = getModuleInfo(in);
    if (info->isInUse())
    {
        // already tuned
        throw Exception(__FILE__, __LINE__,
                        "Failed to start usage the module info with "
                        "name '%s'", info->getName().c_str());
    }
    klk_log(KLKLOG_DEBUG, "Network module started to use '%s'",
            info->getName().c_str());
    info->setInUse(true);
    doInfo(in, out);
    BOOST_ASSERT(info->isInUse() == true);
}

// Do the list command
// that retrives available routes
void Network::doList(const IMessagePtr& in, const IMessagePtr& out)
{
    const StringList result = getValues(mod::SELECT_ALL,
                                        mod::NAME);
    out->setData(msg::key::RESULT, result);
}

// retrives info without locking
void Network::doInfo(const IMessagePtr& in, const IMessagePtr& out)
{
    BOOST_ASSERT(out);
    const mod::InfoPtr info = getModuleInfo(in);
    info->fillOutMessage(out);
}

// Process changes at the DB for a specified dev
void Network::processDev(const IDevPtr& dev, InfoSet& result)
{
    BOOST_ASSERT(dev);

    db::Parameters params;
    params.add("@resource", dev->getStringParam(dev::UUID));
    db::DB db(getFactory());
    db.connect();
    db::ResultVector rv = db.callSelect("klk_network_route_get",
                                        params, NULL);
    for (db::ResultVector::iterator res = rv.begin();
         res != rv.end(); res++)
    {
        // klk_network_routes.name,
        // klk_network_routes.route,
	// klk_network_routes.host,
	// klk_network_routes.port,
        // klk_network_routes.protocol,
	// klk_network_protocols.proto_name

        const std::string name = (*res)["name"].toString();
        const std::string route = (*res)["route"].toString();
        const std::string host = (*res)["host"].toString();
        const std::string port = (*res)["port"].toString();
        const std::string protocol = (*res)["protocol"].toString();
        const std::string proto_name = (*res)["proto_name"].toString();

        std::string type = msg::key::NETUNICAST;
        if (isMulticast(host))
        {
            type = msg::key::NETMULTICAST;
        }

        RouteInfoPtr info(new RouteInfo(route, name, host,
                                        port, protocol, type, dev,
                                        getFactory()));
        result.insert(info);
        // can be in use by another mediaserver
        // BOOST_ASSERT(info->isInUse(), false);
    }
}

// Retrives an list with data from @ref grDB "database"
const Network::InfoSet Network::getInfoFromDB()
{
    InfoSet result;
    IDevList devs =
        getFactory()->getResources()->getResourceByType(dev::NET);
    std::for_each(devs.begin(), devs.end(),
                  boost::bind(&Network::processDev, this, _1,
                              boost::ref(result)));
    return result;
}

// Retrives the route info list
const RouteInfoList Network::getRouteInfo() const
{
    return m_info.getInfoList();
}

// Checks is the host multicast or not
const bool Network::isMulticast(const std::string& host)
{

    // check multicast address that have to be assigned to any interface
    // Class Name Address Bits 	From ... To 	            Pourpose
    // Class D 	  1110          224.0.0.0 - 239.255.255.255 Multicast IP Addr
    struct in_addr addr;
    int domain = AF_INET, s = 0;
    s = inet_pton(domain, host.c_str(), &addr);
    if (s == 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Host value '%s' is invalid",
                        host.c_str());
    }
    else if (s < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in inet_pton(): %s. "
                        "Host value '%s' is invalid",
                        s, strerror(s), host.c_str());
    }

    return IN_MULTICAST(ntohl(addr.s_addr));
}

// Updates lock for locked info
void Network::updateLock()
{
    const RouteInfoList list = getRouteInfo();
    for (RouteInfoList::const_iterator item = list.begin();
         item != list.end(); item++)
    {
        (*item)->updateInUse();
    }
}

// Do some actions after main loop
void Network::postMainLoop() throw()
{
    try
    {
        const RouteInfoList list = getRouteInfo();
        for (RouteInfoList::const_iterator item = list.begin();
             item != list.end(); item++)
        {
            (*item)->clearInUse();
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Error while clearing network usage info at DB");
    }

    ModuleWithInfo<RouteInfo>::postMainLoop();
}
