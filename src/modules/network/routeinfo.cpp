/**
   @file routeinfo.cpp
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
   - 2008/11/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "routeinfo.h"
#include "db.h"
#include "messages.h"

using namespace klk;
using namespace klk::net;

//
// RouteInfo class
//

// Constructor
RouteInfo::RouteInfo(const std::string& route,
                     const std::string& name,
                     const std::string& host,
                     const std::string& port,
                     const std::string& protocol,
                     const std::string& type,
                     const IDevPtr& dev,
                     IFactory* factory) :
    mod::Info(route, name), m_host(host),
    m_port(port), m_protocol(protocol), m_type(type), m_dev(dev),
    m_factory(factory)
{
    BOOST_ASSERT(m_dev);
    BOOST_ASSERT(m_factory);
}

// Retrives route host
const std::string RouteInfo::getHost() const throw()
{
    Locker lock(&m_lock);
    return m_host;
}


// Retrives route port
const std::string RouteInfo::getPort() const throw()
{
    Locker lock(&m_lock);
    return m_port;
}

// Retrives route protocol
const std::string RouteInfo::getProtocol() const throw()
{
    Locker lock(&m_lock);
    return m_protocol;
}


// Retrives dev info
const IDevPtr RouteInfo::getDev() const throw()
{
    Locker lock(&m_lock);
    return m_dev;
}

// Retrives route type (unicast/multicast)
const std::string RouteInfo::getType() const throw()
{
    Locker lock(&m_lock);
    return m_type;
}

// Tests if the info is used by somebody
// Checks DB to retrive the state
bool RouteInfo::isInUse() throw()
{
    bool res = false;

    try
    {
        // `klk_network_route_getlock` (
        // IN route VARCHAR(40),
        // OUT return_value INT

        db::Parameters params;
        params.add("@route", getUUID());
        params.add("@return_value");
        db::DB db(m_factory);
        db.connect();
        db::Result db_result = db.callSimple(
            "klk_network_route_getlock",
            params);

        res = (db_result["@return_value"].toInt() == 1);
    }
    catch (...)
    {
        klk_log(KLKLOG_ERROR, "Got an error while determine "
                "network route in use info");
        return false; // not in use
    }

    return res;
}

// Sets in use flag
void RouteInfo::setInUse(bool value)
{
    db::Parameters params;
    params.add("@route", getUUID());

    db::DB db(m_factory);
    db.connect();

    std::string sp;

    if (value)
    {
        params.add("@resource", m_dev->getStringParam(dev::UUID));
        sp = "klk_network_route_lock";
    }
    else
    {
        sp = "klk_network_route_unlock";
    }


    BOOST_ASSERT(sp.empty() == false);
    params.add("@return_value");
    db::Result result = db.callSimple(sp, params);
    BOOST_ASSERT(result["@return_value"].toInt() == 0);
}

// Updates inuse info at the db
void RouteInfo::updateInUse()
{
    db::Parameters params;
    params.add("@route", getUUID());
    params.add("@resource", m_dev->getStringParam(dev::UUID));
    params.add("@return_value");

    db::DB db(m_factory);
    db.connect();
    db::Result result = db.callSimple("klk_network_route_update_lock",
                                         params);
    BOOST_ASSERT(result["@return_value"].toInt() == 0);
}

// Clears inuse info at the db
void RouteInfo::clearInUse()
{
    db::Parameters params;
    params.add("@route", getUUID());
    params.add("@resource", m_dev->getStringParam(dev::UUID));
    params.add("@return_value");

    db::DB db(m_factory);
    db.connect();
    db::Result result = db.callSimple("klk_network_route_clear_lock",
                                         params);
    BOOST_ASSERT(result["@return_value"].toInt() == 0);
}

// Fills the message with internal info
void RouteInfo::fillOutMessage(const IMessagePtr& out) const
{
    BOOST_ASSERT(out);
    out->setData(msg::key::MODINFONAME, getName());
    out->setData(msg::key::NETHOST, getHost());
    out->setData(msg::key::NETPORT, getPort());
    out->setData(msg::key::NETPROTO, getProtocol());
    out->setData(msg::key::NETTYPE, getType());
}

