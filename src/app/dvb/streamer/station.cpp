/**
   @file station.cpp
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
   - 2008/10/04 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>

#include "station.h"
#include "log.h"
#include "defines.h"
#include "exception.h"

// modules specific info
#include "network/defines.h"
#include "network/messages.h"
#include "dvb/messages.h"
#include "dvb/dvbdev.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// Station class
//

// Constructor
Station::Station(const std::string& uuid,
                 const std::string& name,
                 const std::string& type,
                 const std::string& source,
                 IFactory* factory) :
    mod::Info(uuid, name),
    m_factory(factory), m_module(),
    m_route(sock::RouteInfo("", 0, sock::UDP, sock::MULTICAST)),
    m_type(type), m_source(source),
    m_channel(""), m_dev(),
    m_route_uuid(""), m_no(0),
    m_rate(0)
{
    BOOST_ASSERT(m_factory);
    m_module = m_factory->getModuleFactory()->getModule(MODID);
    BOOST_ASSERT(m_module);
    BOOST_ASSERT(m_type.getValue().empty() == false);
    BOOST_ASSERT(m_source.getValue().empty() == false);
}

// Destructor
Station::~Station()
{
    freeResources();
}

// Tests if the info is used by somebody
// Checks DB to retrive the state
bool Station::isInUse() throw()
{
    bool res = false;

    try
    {
        // `klk_app_dvb_station_getlock` (
        // IN station VARCHAR(40),
        // OUT return_value INT

        db::Parameters params;
        params.add("@station", getUUID());
        params.add("@return_value");
        db::DB db(m_factory);
        db.connect();
        db::Result db_result = db.callSimple(
            "klk_app_dvb_station_getlock",
            params);

        res = (db_result["@return_value"].toInt() == 1);
    }
    catch (...)
    {
        klk_log(KLKLOG_ERROR, "Got an error while determine "
                "dvb-streamer station in use info");
        return false; // not in use
    }

    return res;
}

// Sets in use flag
void Station::setInUse(bool value)
{
    db::Parameters params;
    params.add("@station", getUUID());

    db::DB db(m_factory);
    db.connect();

    std::string sp;

    if (value)
    {
        params.add("@used_host", db.getHostUUID());
        sp = "klk_app_dvb_station_lock";
    }
    else
    {
        sp = "klk_app_dvb_station_unlock";
    }


    BOOST_ASSERT(sp.empty() == false);
    params.add("@return_value");
    db::Result result = db.callSimple(sp, params);
    BOOST_ASSERT(result["@return_value"].toInt() == 0);
}


// Updates inuse info at the db
void Station::updateInUse() throw()
{
    // some of info can be missing at the db
    // thus we catch all exceptions here
    try
    {
        db::DB db(m_factory);
        db.connect();

        db::Parameters params;
        params.add("@station", getUUID());
        params.add("@used_host", db.getHostUUID());
        params.add("@return_value");

        db::Result result = db.callSimple("klk_app_dvb_station_update_lock",
                                          params);
        BOOST_ASSERT(result["@return_value"].toInt() == 0);
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Failed to update DVB streamer usage "
                "info for '%s'", getName().c_str());
    }
}

// Sets channel
void  Station::setChannel(const std::string& channel)
{
    BOOST_ASSERT(channel.empty() == false);
    // try to tune channel at the DVB module
    IMessageFactory* msgfactory = m_factory->getMessageFactory();
    BOOST_ASSERT(msgfactory);
    IMessagePtr req = msgfactory->getMessage(msg::id::DVBSTART);
    BOOST_ASSERT(req);
    req->setData(msg::key::DVBACTIVITY, dev::STREAMING);
    req->setData(msg::key::TVCHANNELID, channel);
    req->setData(msg::key::DVBSOURCE, m_source);
    req->setData(msg::key::TYPE, m_type);
    IMessagePtr res;

    BOOST_ASSERT(m_module);
    m_module->sendSyncMessage(req, res);
    // test res
    BOOST_ASSERT(res);

    if (res->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // dvb module rejected the channel
        throw Exception(__FILE__, __LINE__,
                        "DVB module rejected DVB channel '%s'. Channel name : %s",
                        channel.c_str(), getName().c_str());
    }
    // station name and channel name are differ
    //const std::string
    // channel_name(res->getValue(msg::key::TVCHANNELNAME));
    //BOOST_ASSERT(channel_name == getName());
    const std::string dev_uuid = res->getValue(msg::key::RESOURCE);
    m_dev = m_factory->getResources()->getResourceByUUID(dev_uuid);
    BOOST_ASSERT(m_dev);

    try
    {
        // channel number
        m_no =
            boost::lexical_cast<u_int>(res->getValue(msg::key::TVCHANNELNO));
    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }

    m_channel = channel;
    setInUse(true);
}

// Sets route
void Station::setRoute(const std::string& route)
{
    BOOST_ASSERT(route.empty() == false);

    // try to tune network at the DVB module
    IMessageFactory* msgfactory = m_factory->getMessageFactory();
    BOOST_ASSERT(msgfactory);
    IMessagePtr request = msgfactory->getMessage(msg::id::NETSTART);
    BOOST_ASSERT(request);
    request->setData(msg::key::MODINFOUUID, route);
    IMessagePtr response;
    m_module->sendSyncMessage(request, response);
    // test response
    BOOST_ASSERT(response);

    if (response->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // net module rejected the route
        throw Exception(__FILE__, __LINE__,
                        "Net module rejected route '%s'",
                        route.c_str());
    }

    BOOST_ASSERT(m_route_uuid.getValue().empty() == true);
    m_route_uuid = route;

    const std::string proto = response->getValue(msg::key::NETPROTO);
    if (proto != net::UDP)
    {
        throw Exception(__FILE__, __LINE__,
                        "Net protocol with uuid '%s' is not supported "
                        "by DVB streamer",
                        proto.c_str());
    }

    try
    {
        const std::string host = response->getValue(msg::key::NETHOST);
        int port =
            boost::lexical_cast<int>(response->getValue(msg::key::NETPORT));

        // FIXME!!! retrive all info from the message
        m_route = sock::RouteInfo(host, port, sock::UDP, sock::MULTICAST);
    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }
}


// deallocates resources
void Station::freeResources() throw()
{
    unsetChannel();
    unsetRoute();

    try
    {
        setInUse(false);
    }
    catch(...)
    {
        // ignore it
        klk_log(KLKLOG_ERROR, "Got an error while stop stream for channel: %s",
                getName().c_str());
    }
}

// Unsets station
void Station::unsetChannel() throw()
{
    if (m_channel.getValue().empty())
        return; // nothing to do

    try
    {
        IMessageFactory* msgfactory = m_factory->getMessageFactory();
        BOOST_ASSERT(msgfactory);
        IMessagePtr request = msgfactory->getMessage(msg::id::DVBSTOP);
        BOOST_ASSERT(request);
        request->setData(msg::key::TVCHANNELID, m_channel);
        request->setData(msg::key::DVBACTIVITY, dev::STREAMING);

        m_factory->getModuleFactory()->sendMessage(request);
    }
    catch(...)
    {
        // dvb module rejected the station
        klk_log(KLKLOG_ERROR,
                "An error while stop for DVB channel: %s",
                getName().c_str());
    }

    m_channel = "";
    m_dev.reset();
}

// Unsets route
void Station::unsetRoute() throw()
{
    if (m_route_uuid.getValue().empty())
        return; // nothing to do

    try
    {
        IMessageFactory* msgfactory = m_factory->getMessageFactory();
        BOOST_ASSERT(msgfactory);
        // free net tune
        IMessagePtr request = msgfactory->getMessage(msg::id::NETSTOP);
        BOOST_ASSERT(request);
        BOOST_ASSERT(m_route_uuid.getValue().empty() == false);
        request->setData(msg::key::MODINFOUUID, m_route_uuid.getValue());
        IMessagePtr response;
        m_module->sendSyncMessage(request, response);
        // test response
        BOOST_ASSERT(response);

        if (response->getValue(msg::key::STATUS) != msg::key::OK)
        {
            // net module rejected the route stop usage
            throw Exception(
                __FILE__, __LINE__,
                "NET module rejected stop for DVB station '%s'",
                getName().c_str());
        }
    }
    catch(...)
    {
        // gst module rejected the station
        klk_log(KLKLOG_ERROR,
                "DVB module rejected stop for DVB station '%s'",
                getName().c_str());
    }

    m_route_uuid = "";
}

// Retrives station number
u_int Station::getChannelNumber() const
{
    BOOST_ASSERT(m_channel.getValue().empty() == false);
    return m_no.getValue();
}

// Retrives route
const sock::RouteInfo Station::getRoute() const
{
    BOOST_ASSERT(m_route_uuid.getValue().empty() == false);
    return m_route.getValue();
}

// Retrives tuned dev
const IDevPtr Station::getDev() const
{
    Locker lock(&m_lock);
    if (m_dev == NULL)
    {
        throw Exception(__FILE__, __LINE__,
                        "Failed to get dev for a DVB station");
    }
    return m_dev;
}

// Sets the station as failed to stream
void Station::setFailed() throw()
{
    try
    {
        // `klk_app_dvb_failed_station_add` (
        //IN station_uuid VARCHAR(40),
        //IN host_uuid VARCHAR(40)
        db::DB db(m_factory);
        db.connect();
        db::Parameters params;
        params.add("@station", getUUID());
        params.add("@host", db.getHostUUID());
        db.callSimple("klk_app_dvb_failed_station_add", params);
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Exception in Station::setFailed()");
    }
}

// This one retrives data rate for the station
const int Station::getRate() const throw()
{
    return m_rate.getValue();
}

// Sets rate
void Station::setRate(const int rate)
{
    BOOST_ASSERT(rate >= 0);
    m_rate = rate;
}

