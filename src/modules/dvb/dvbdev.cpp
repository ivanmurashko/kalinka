/**
   @file dvbdev.cpp
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
   - 2009/01/24 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dvbdev.h"
#include "db.h"
#include "log.h"
#include "dev.h"
#include "exception.h"

#include "utils/utils.h"

using namespace klk;
using namespace klk::dev;

//
// DVB class
//

// Constructor
DVB::DVB(IFactory* factory, const std::string& type) :
    Base(factory, type)
{
}

// Retrives PCI bus uuid
// FIXME!!! remove it
const std::string DVB::getBusUUID()
{
    IDevList list =
        getFactory()->getResources()->getResourceByType(PCIBUS);
    BOOST_ASSERT(list.size() == 1);
    return (*list.begin())->getStringParam(UUID);
}

// Updates the dev info
void DVB::update()
{
    // Free lost lock
    setParam(LOSTLOCK, 0, true);


    // getting bus uuid
    const std::string bus_uuid = getBusUUID();
    BOOST_ASSERT(bus_uuid.empty() == false);

    // klk_dvb_resource_get_uuid
    // IN bus VARCHAR(40),
    // IN dvb_type VARCHAR(40),
    // IN adapter_no TINYINT,
    // IN frontend_no TINYINT,
    // OUT resource_name VARCHAR(50),
    // OUT resource VARCHAR(40),
    // OUT signal_source VARCHAR(40),
    // OUT bandwidth INT,
    // OUT return_value INT

    db::Parameters params;
    params.add("@bus", bus_uuid);
    params.add("@dvb_type", getStringParam(TYPE));
    params.add("@adapter_no", getIntParam(ADAPTER));
    params.add("@frontend_no", getIntParam(FRONTEND));
    params.add("@resource_name");
    params.add("@resource");
    params.add("@signal_source");
    params.add("@bandwidth");
    params.add("@return_value");
    db::DB db(getFactory());
    db.connect();
    db::Result result = db.callSimple(
        "klk_dvb_resource_get_uuid",
        params);

    if (result["@return_value"].toInt() != 0)
    {
        klk_log(KLKLOG_ERROR,
                "There is a problem with retriving info "
                "from DB for a device '%s'",
                getStringParam(NAME).c_str());
        return;
    }

    BOOST_ASSERT(result["@return_value"].toInt() == 0);
    setParam(UUID, result["@resource"].toString());
    setParam(BUSUUID, bus_uuid);
    setParam(NAME, result["@resource_name"].toString());

    if (result["@bandwidth"].isNull())
    {
        // FIXME!!!
        setParam(BANDWIDTH, 0);
    }
    else
    {
        setParam(BANDWIDTH, result["@bandwidth"].toInt());
    }

    std::string source_name;
    if (result["@signal_source"].isNull())
    {
        setParam(SOURCE, "");
        source_name = "";
        klk_log(KLKLOG_ERROR, "Source for DVB dev '%s' is not set",
                getStringParam(NAME).c_str());
    }
    else
    {
        const std::string source = result["@signal_source"].toString();
        setParam(SOURCE, source);
        source_name = dvb::Utils::getSourceName(getFactory(), source);
    }

    klk_log(
        KLKLOG_INFO,
        "Registered DVB device name: '%s' type: '%s' uuid: '%s' source: '%s'",
        getStringParam(NAME).c_str(),
        getStringParam(TYPE).c_str(),
        getStringParam(UUID).c_str(),
        source_name.c_str());
}

// Updates DB (adds the dev if it's missing)
void DVB::updateDB()
{
    // should not be called for a dev with already set UUID
    if (hasParam(UUID))
    {
        // nothing to do
        // we have already had the DB record
        return;
    }

    // getting bus uuid
    const std::string bus_uuid = getBusUUID();
    BOOST_ASSERT(bus_uuid.empty() == false);

    // `klk_dvb_resource_add` (
    // IN bus VARCHAR(40),
    // IN dvb_type VARCHAR(40),
    // IN signal_source VARCHAR(40),
    // IN enabled TINYINT,
    // IN adapter_no TINYINT,
    // IN frontend_no TINYINT,
    // IN resource_name VARCHAR(50),
    // OUT resource VARCHAR(40),
    // OUT return_value INT

    db::Parameters params;
    params.add("@bus", bus_uuid);
    params.add("@dvb_type", getStringParam(TYPE));
    params.add("@signal_source");
    params.add("@enabled", 1); //FIXME!!!
    params.add("@adapter_no", getIntParam(ADAPTER));
    params.add("@frontend_no", getIntParam(FRONTEND));
    params.add("@resource_name", getStringParam(NAME));
    params.add("@resource");
    params.add("@return_value");
    db::DB db(getFactory());
    db.connect();
    db::Result result = db.callSimple(
        "klk_dvb_resource_add",
        params);

    if (result["@return_value"].toInt() != 0)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "There is a problem with add dev info "
                             "to  DB for a device '" +
                             getStringParam(NAME) + "'");
    }

    BOOST_ASSERT(result["@return_value"].toInt() == 0);
    setParam(UUID, result["@resource"].toString());
    // Free lost lock
    setParam(LOSTLOCK, 0, true);

    klk_log(
        KLKLOG_INFO,
        "Device with name: '%s' type: '%s' uuid: '%s' was added to the DB",
        getStringParam(NAME).c_str(),
        getStringParam(TYPE).c_str(),
        getStringParam(UUID).c_str());
}

// Tests equalance of two devs
bool DVB::equal_to(const IDevPtr& dev) const
{
    boost::shared_ptr<DVB> dvb_dev =
        boost::dynamic_pointer_cast<DVB, IDev>(dev);
    if (dvb_dev != NULL)
    {
        if (dvb_dev->getStringParam(TYPE) ==
            getStringParam(TYPE) &&
            dvb_dev->getIntParam(ADAPTER) ==
            getIntParam(ADAPTER) &&
            dvb_dev->getIntParam(FRONTEND) ==
            getIntParam(FRONTEND))
            return true;
    }
    return false;

}

