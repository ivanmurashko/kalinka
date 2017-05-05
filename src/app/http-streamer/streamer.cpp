/**
   @file streamer.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>


#include "streamer.h"
#include "defines.h"
#include "exception.h"
#include "statcmd.h"
#include "outcmd.h"
#include "incmd.h"
#include "db.h"

#include "snmp/factory.h"
#include "snmp/scalar.h"
#include "snmp/table.h"

// modules specific info
#include "network/defines.h"
#include "network/messages.h"

using namespace klk;
using namespace klk::http;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new Streamer(factory));
}

//
// Streamer class
//

/**
   Set CLI message id
*/
const std::string SETCLI_MSGID = "627d4a0f-52d6-489a-a93f-7937c24200d0";

/**
   Show CLI message id
*/
const std::string SHOWCLI_MSGID = "56c8b78a-918a-4963-8fff-3ef20c60ff47";

// Constructor
Streamer::Streamer(IFactory* factory) :
    launcher::Module(factory, MODID, SETCLI_MSGID, SHOWCLI_MSGID),
    m_httpfactory(new Factory()), m_info()
{
    addDependency(net::MODID);
}

// Destructor
Streamer::~Streamer()
{
}

// Retrives HTTP factory for CLI
Factory* Streamer::getHTTPFactory()
{
    BOOST_ASSERT(m_httpfactory);
    return m_httpfactory;
}


// Register all processors
void Streamer::registerProcessors()
{
    launcher::Module::registerProcessors();

    registerCLI(cli::ICommandPtr(new OutputShowCommand()));
    registerCLI(cli::ICommandPtr(new OutputSetCommand()));

    registerCLI(cli::ICommandPtr(new StatCommand()));
    registerCLI(cli::ICommandPtr(new InputShowCommand()));
    registerCLI(cli::ICommandPtr(new InputAddCommand()));
    registerCLI(cli::ICommandPtr(new InputDelCommand()));

    registerSNMP(boost::bind(&Streamer::processSNMP, this, _1), MODID);

    registerDBUpdateMessage(UPDATEDB_MESSAGE);
}

// Gets a human readable module name
const std::string Streamer::getName() const throw()
{
    return MODNAME;
}

// Retrives an set with data from @ref grDB "database"
const Streamer::InfoSet Streamer::getInputInfoFromDB()
{
    BOOST_ASSERT(getAppUUID().empty() == false);

    // retrive application info
    db::DB db(getFactory());
    db.connect();

    //`klk_app_http_streamer_route_get` (
    // IN application VARCHAR(40)
    db::Parameters params;
    params.add("@application", getAppUUID());

    db::ResultVector rv =
        db.callSelect("klk_app_http_streamer_route_get", params, NULL);
    InfoSet set;
    for (db::ResultVector::iterator res = rv.begin();
         res != rv.end(); res++)
    {
        // SELECT
        // klk_app_http_streamer_route.in_route,
        // klk_app_http_streamer_route.out_path,
        // klk_media_types.name,
        // klk_app_http_streamer_media_types.uuid

        const std::string uuid = (*res)["in_route"].toString();
        // FIXME!!! complex code
        // getInfoByUUID throws an exception in the case of info not found
        try
        {
            InputInfoPtr find = m_info.getInfoByUUID(uuid);
            set.insert(find);
            continue;
        }
        catch(...)
        {
            // we have to add the new element
            // first of all we should get route info from network module
            try
            {
                InputInfoPtr info(new InputInfo(getRouteInfoByUUID(uuid)));
                info->setPath((*res)["out_path"].toString());
                info->setMediaTypeName((*res)["name"].toString());
                info->setMediaTypeUuid((*res)["uuid"].toString());
                set.insert(info);
                klk_log(KLKLOG_DEBUG, "Add an input HTTP route for start: "
                        "%s on %s:%d",
                        info->getRouteInfo()->getName().c_str(),
                        info->getRouteInfo()->getHost().c_str(),
                        info->getRouteInfo()->getPort());
            }
            catch(const std::exception& err)
            {
                klk_log(KLKLOG_ERROR, "Error while getting input routes: %s",
                        err.what());
            }
            catch(...)
            {
                klk_log(KLKLOG_ERROR,
                        "Error while getting input routes: unknown");
            }
        }
    }

    return set;
}

// Process changes at the DB
void Streamer::processDB(const IMessagePtr& msg)
{
    BOOST_ASSERT(getAppUUID().empty() == false);
    processOutRoute();
    processInRoute();
}

// Retrives out route info from DB
const std::string Streamer::getOutRoute()
{
    BOOST_ASSERT(getAppUUID().empty() == false);
    // retrive application info
    db::DB db(getFactory());
    db.connect();

    //`klk_app_http_streamer_get` (
    // IN application VARCHAR(40),
    // OUT out_route VARCHAR(40)
    db::Parameters params;
    params.add("@application", getAppUUID());
    params.add("@route");
    params.add("@return_value");

    db::Result res = db.callSimple("klk_app_http_streamer_get", params);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error while retrive info for HTTP streeamer "
                        "from db");
    }

    return res["@route"].toString();
}


// Retrives (from DB) and sets out route info
void Streamer::processOutRoute()
{
    const std::string route_uuid = getOutRoute();
    BOOST_ASSERT(route_uuid.empty() == false);

    if (getHTTPFactory()->getOutThread()->checkRoute(route_uuid))
    {
        // the route has been already set
        // nothing to do
        return;
    }

    // we need to setup new route
    const RouteInfoPtr new_route = getRouteInfoByUUID(route_uuid);
    // has been something set?
    if (getHTTPFactory()->getOutThread()->checkRoute())
    {
        // stop old
        stopRoute(getHTTPFactory()->getOutThread()->getRoute());
    }
    // setup new one
    getHTTPFactory()->setOutRoute(new_route);
}

// Stops a route usage
void Streamer::stopRoute(const RouteInfoPtr& route)
{
    // FIXME!!! bad check
    if (!route)
    {
        // the route has not been started yet
        return;
    }

    // Stop only TCP/IP routes (UDPs are clients)
    // FIXME!!! bad code
    // see Streamer::getRouteInfoByUUID
    if (route->getProtocol() != sock::TCPIP)
    {
        return;
    }

    IMessageFactory* msgfactory = getFactory()->getMessageFactory();
    IMessagePtr req = msgfactory->getMessage(msg::id::NETSTOP);
    BOOST_ASSERT(req);
    req->setData(msg::key::MODINFOUUID, route->getUUID());
    IMessagePtr res;
    sendSyncMessage(req, res);
    BOOST_ASSERT(res);
    if (res->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // gst module rejected the channel
        throw Exception(__FILE__, __LINE__,
                        "Net module rejected route with name '%s'",
                        route->getName().c_str());
    }

    klk_log(KLKLOG_DEBUG, "Stop an HTTP route: %s on %s:%d",
            route->getName().c_str(),
            route->getHost().c_str(), route->getPort());
}

// Deletes an input route info
void Streamer::delInputInfo(const InputInfoPtr& info) throw()
{
    try
    {
        InputInfoPtr inroute =
            mod::Info::dynamicPointerCast<InputInfo>(info);
        getHTTPFactory()->getInThreadContainer()->delInfo(inroute);
        stopRoute(inroute->getRouteInfo());
        // delete it from the container
        m_info.delElem(info, false);
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_DEBUG,
                "Error while deleting an input route for HTTP streamer: %s",
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_DEBUG,
                "Error while deleting an input route for HTTP streamer");
    }
}

// Adds an input route info
void Streamer::addInputInfo(const InputInfoPtr& info) throw()
{
    try
    {
        InputInfoPtr inroute =
            mod::Info::dynamicPointerCast<InputInfo>(info);
        getHTTPFactory()->getInThreadContainer()->addInfo(inroute);
        m_info.addElem(info, true);
    }
    catch(const std::exception& err)
    {
        delInputInfo(info);
        klk_log(KLKLOG_DEBUG,
                "Error while add an input route for HTTP streamer: %s",
                err.what());
    }
    catch(...)
    {
        delInputInfo(info);
        klk_log(KLKLOG_DEBUG,
                "Error while add an input route for HTTP streamer");
    }
}

// Retrives (from DB) and sets input route info
void Streamer::processInRoute()
{
    InfoSet current = getInputInfoFromDB();

    // elements to be deleted
    InfoSet deldata = m_info.getDel(current);
    std::for_each(deldata.begin(), deldata.end(),
                  boost::bind(&Streamer::delInputInfo, this, _1));

    // elements to be added
    InfoSet newdata = m_info.getAdd(current);
    std::for_each(newdata.begin(), newdata.end(),
                  boost::bind(&Streamer::addInputInfo, this, _1));
}

// Retrives route info by it's uuid
const RouteInfoPtr Streamer::getRouteInfoByUUID(const std::string& uuid)
{
    BOOST_ASSERT(uuid.empty() == false);

    IMessageFactory* msgfactory = getFactory()->getMessageFactory();
    IMessagePtr req = msgfactory->getMessage(msg::id::NETINFO);
    BOOST_ASSERT(req);
    req->setData(msg::key::MODINFOUUID, uuid);
    IMessagePtr res;
    sendSyncMessage(req, res);
    BOOST_ASSERT(res);
    if (res->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // gst module rejected the channel
        throw Exception(__FILE__, __LINE__,
                        "Net module rejected route '%s'",
                        uuid.c_str());
    }
    const std::string name = res->getValue(msg::key::MODINFONAME);
    const std::string host = res->getValue(msg::key::NETHOST);

    sock::Type type = sock::UNICAST;
    if (res->getValue(msg::key::NETTYPE) == msg::key::NETMULTICAST)
    {
        type = sock::MULTICAST;
    }

    u_int port = 0;
    try
    {
        port = boost::lexical_cast<u_int>(res->getValue(msg::key::NETPORT));
    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }

    sock::Protocol proto = sock::TCPIP;
    if (res->getValue(msg::key::NETPROTO) == net::UDP)
    {
        proto = sock::UDP;
    }
    else
    {
        // FIXME!!! place it at separate method
        // we should lock it (TCP input should be unique - it's a server)
        IMessagePtr req_start = msgfactory->getMessage(msg::id::NETSTART);
        BOOST_ASSERT(req_start);
        req_start->setData(msg::key::MODINFOUUID, uuid);
        IMessagePtr res_start;
        sendSyncMessage(req_start, res_start);
        BOOST_ASSERT(res_start);
        if (res_start->getValue(msg::key::STATUS) != msg::key::OK)
        {
            // gst module rejected the channel
            throw Exception(__FILE__, __LINE__,
                            "Net module rejected route '%s'",
                            uuid.c_str());
        }
    }

    return RouteInfoPtr(new RouteInfo(uuid, name, host, port, proto, type));
}

// Do some actions before main loop
void Streamer::preMainLoop()
{
    launcher::Module::preMainLoop();
    getHTTPFactory()->init();
}


// Do some actions after main loop
void Streamer::postMainLoop() throw()
{
    if (m_httpfactory)
    {
        m_httpfactory->stop();
    }
    launcher::Module::postMainLoop();
}

// Processes SNMP request
const snmp::IDataPtr Streamer::processSNMP(const snmp::IDataPtr& req)
{
    BOOST_ASSERT(req);
    snmp::ScalarPtr reqreal =
        boost::dynamic_pointer_cast<snmp::Scalar, snmp::IData>(req);
    BOOST_ASSERT(reqreal);

    const std::string reqstr = reqreal->getValue().toString();
    // support only snmp::GETSTATUSTABLE
    if (reqstr != snmp::GETSTATUSTABLE)
    {
        throw Exception(__FILE__, __LINE__,
                        "Unknown SNMP request: " + reqstr);

    }

    // create the response
    // table with data
    snmp::TablePtr table(new snmp::Table());
    InputInfoList infos = m_info.getInfoList();
    u_int count = 0;
    for (InputInfoList::iterator i = infos.begin();
         i != infos.end();
         i++, count++)
    {
        // klkIndex             Counter32
        // klkOutputPath        DisplayString,
        // klkInputAddr         DisplayString,
        // klkInputRate         Integer32,
        // klkOutputRate        Integer32,
        // klkOutputConn        Counter32,
        // klkBrokenPackages    Counter32

        snmp::TableRow row;
        row.push_back(count);
        const std::string path = (*i)->getPath();
        row.push_back(path);
        try
        {
            InThreadPtr inthread = getHTTPFactory()->getInThreadContainer()->getThreadByPath(path);
            row.push_back(inthread->getReader()->getPeerName());
            row.push_back(static_cast<int>(inthread->getReader()->getRate()));
            StringWrapper o_rate(
                static_cast<int>(getHTTPFactory()->getConnectThreadContainer()->getConnectionRate(path)));
            row.push_back(o_rate);
            row.push_back(inthread->getConnectionCount());
            row.push_back(inthread->getReader()->getBrokenCount());
        }
        catch(const std::exception&)
        {
            row.clear();
            row.push_back(count);
            row.push_back(path);
            row.push_back("n/a");
            row.push_back(0);
            row.push_back(0);
            row.push_back(0);
            row.push_back(0);
        }
        table->addRow(row);
    }

    return table;
}
