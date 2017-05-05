/**
   @file netdev.cpp
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
   - 2009/02/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "netdev.h"
#include "exception.h"
#include "dev.h"
#include "db.h"
#include "defines.h"

using namespace klk;
using namespace klk::dev;

//
// Net class
//

// Constructor
Net::Net(IFactory* factory) : Base(factory, NET)
{
}

// Retrives PCI bus uuid
// FIXME!!! remove it
const std::string Net::getBusUUID()
{
    IDevList list =
        getFactory()->getResources()->getResourceByType(PCIBUS);
    BOOST_ASSERT(list.size() == 1);
    return (*list.begin())->getStringParam(UUID);
}

// Updates the device info (from DB for example)
void Net::update()
{
    // getting bus uuid
    const std::string bus_uuid = getBusUUID();
    BOOST_ASSERT(bus_uuid.empty() == false);

    // IN bus VARCHAR(40)
    // SELECT
    // klk_resources.resource_name,
    // klk_resources.bus,
    // klk_resources.resource,
    // klk_resources.bandwidth,
    // klk_network_interface_addresses.ip_address,
    // klk_network_interface_addresses.ip_mask
    // klk_network_interface_addresses.address
    db::DB db(getFactory());
    db.connect();

    db::Parameters params;
    params.add("@host_uuid", db.getHostUUID());

    db::ResultVector
        rv = db.callSelect("klk_network_resources_list", params, NULL);

    for (db::ResultVector::iterator res = rv.begin();
         res != rv.end(); res++)
    {
        // looking for the name
        if (getStringParam(NAME) == (*res)["resource_name"].toString())
        {
            setParam(UUID, (*res)["resource"].toString());
            setParam(NETADDRUUID, (*res)["address"].toString());
            setParam(BUSUUID, bus_uuid);
            if ((*res)["bandwidth"].isNull())
            {
                // FIXME!!!
                setParam(BANDWIDTH, 0);
            }
            else
            {
                setParam(BANDWIDTH, (*res)["bandwidth"].toInt());
            }


            klk_log(
                KLKLOG_INFO,
                "Registered network device name: '%s' type: '%s' uuid: '%s'",
                getStringParam(NAME).c_str(),
                getStringParam(TYPE).c_str(),
                getStringParam(UUID).c_str());
            break;
        }
    }
}

// Updates settings at the DB
void Net::updateDB()
{
    // should not be called for a dev with already set UUID
    if (hasParam(UUID))
    {
        // nothing to do
        // we have already had the DB record
        return;
    }

    db::DB db(getFactory());
    db.connect();

    const std::string bus_uuid = getBusUUID();
    BOOST_ASSERT(bus_uuid.empty() == false);

    // create network card
    //IN bus VARCHAR(40),
    //IN trunk VARCHAR(40),
    //IN enabled TINYINT,
    //IN node_name VARCHAR(10),
    //IN resource_name VARCHAR(50),
    //OUT resource VARCHAR(40),
    //OUT return_value INT

    db::Parameters params;
    params.add("@bus", bus_uuid);
    params.add("@trunk", net::DEFAULT_TRUNK);
    params.add("@enabled", 1);
    params.add("@node_name", getStringParam(NAME));
    params.add("@resource_name", getStringParam(NAME));
    params.add("@resource");
    params.add("@return_value");
    db::Result result = db.callSimple("klk_network_interface_resource_add",
                                         params);
    if (result["@return_value"].toInt() != 0)
    {
        throw Exception(
            __FILE__, __LINE__,
            "DB call klk_network_interface_resource_add failed");
    }
    const std::string resource = result["@resource"].toString();
    BOOST_ASSERT(resource.empty() == false);

    // assign ip address to network card
    params.clear();
    //INOUT address VARCHAR(40),
    //IN resource VARCHAR(40),
    //IN ip_address VARCHAR(16),
    //IN ip_mask VARCHAR(16),
    //OUT return_value INT
    params.add("@address");
    params.add("@resource", resource);
    params.add("@ip_address", getStringParam(NETADDR));
    params.add("@ip_mask", getStringParam(NETMASK));
    params.add("@return_value");
    result = db.callSimple("klk_network_interface_address_add",
                           params);
    if (result["@return_value"].toInt() != 0)
    {
        throw Exception(
            __FILE__, __LINE__,
            "DB call klk_network_interface_address_add failed");
    }

    setParam(UUID, resource);
    setParam(NETADDRUUID, result["@address"].toString());
}

// Tests equalence of two devs
// we get equalence by the dev's name
bool Net::equal_to(const IDevPtr& dev) const
{
    if (boost::dynamic_pointer_cast<Net, IDev>(dev) == NULL)
    {
        // not net dev
        return false;
    }

    if (hasParam(NAME) == false ||
        dev->hasParam(NAME) == false)
    {
        // don't have the name param
        return false;
    }

    if (getStringParam(NAME) == dev->getStringParam(NAME))
    {
        // name does not match
        return true;
    }
    return false;
}
