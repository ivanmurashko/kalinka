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
   - 2008/09/21 created by ipp (Ivan Murashko)
   - 2008/11/03 move code from DVB module here by ipp
   - 2008/01/04 add some excpetions here by ipp
   - 2009/07/04 DB was cardinally updated
   - 2009/23/04 Views and cursors were introduced at the DB
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "streamer.h"
#include "dev.h"
#include "common.h"
#include "log.h"
#include "defines.h"
#include "exception.h"
#include "stationaddcmd.h"
#include "stationdelcmd.h"
#include "showcmd.h"
#include "station.h"


// modules specific info
#include "network/defines.h"
#include "dvb/defines.h"
#include "dvb/dvbdev.h"
#include "dvb/messages.h"

#include "snmp/factory.h"
#include "snmp/scalar.h"
#include "snmp/table.h"

using namespace klk;
using namespace klk::dvb::stream;

/**
   Stations list
*/
typedef std::list<StationPtr> StationList;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new Streamer(factory));
}

//
// Streamer
//

namespace
{
    /**
       Set cli mcommand id
    */
    const std::string SETCLI_MSGID = "5f5293dd-c19a-4b79-97c9-6cbd4dc327b7";

    /**
       Show cli mcommand id
    */
    const std::string SHOWCLI_MSGID = "f2d9cc84-48c1-418f-b939-22420d31e8da";
}

// Constructor
// @param[in] factory the module factory
Streamer::Streamer(IFactory *factory) :
    launcher::Module(factory, MODID, SETCLI_MSGID, SHOWCLI_MSGID),
    m_info(), m_dvb_scheduler(factory), m_last_dbupdate(0)
{
    // add DVB/NET modules dependency
    addDependency(dvb::MODID);
    addDependency(net::MODID);
}

// Destructor
Streamer::~Streamer()
{
}

// Gets a human readable module name
const std::string Streamer::getName() const throw()
{
    return MODNAME;
}

// Does pre actions before start main loop
void Streamer::preMainLoop()
{
    // Checking staff
    BOOST_ASSERT(getFactory());
    BOOST_ASSERT(getFactory()->getModuleFactory());
    BOOST_ASSERT(getFactory()->getResources());

    // preparation staff
    clearFailed();
    // clear info container
    m_info.clear();
    // base staff
    launcher::Module::preMainLoop();
}

// Does post actions after finish main loop
void Streamer::postMainLoop() throw()
{
    // stop all run streams
    m_dvb_scheduler.stop();
    m_dvb_scheduler.clear();
    // clear failed list
    clearFailed();
    // clear info container
    m_info.clear();
    // base actions
    launcher::Module::postMainLoop();
}

// Register all processors
void Streamer::registerProcessors()
{
    launcher::Module::registerProcessors();

    registerASync(
        msg::id::DVBSIGNALLOST,
        boost::bind(&Streamer::processSignalLost, this, _1));

    registerCLI(cli::ICommandPtr(new StationAddCommand()));
    registerCLI(cli::ICommandPtr(new StationDelCommand()));
    registerCLI(cli::ICommandPtr(new StationShowCommand()));
    registerCLI(cli::ICommandPtr(new NotAssignedShowCommand()));

    registerTimer(boost::bind(&Streamer::updateLock, this),
                  LOCK_UPDATE_INTERVAL);
    registerTimer(boost::bind(&Streamer::checkFailed, this),
                  FAILURES_UPDATEINTERVAL);

    registerSNMP(boost::bind(&Streamer::processSNMP, this, _1), MODID);
}

// Updates lock info
void Streamer::updateLock()
{
    StationList list = m_info.getInfoList();
    std::for_each(list.begin(), list.end(),
                  boost::bind(&Station::updateInUse, _1));
}

// Retrives DB data
const Streamer::InfoSet Streamer::getInfoFromDB()
{
    // First of all we also should retrive a set with stations
    // that are currently
    // streaming by the server and present at the DB
    // (marked as locked by the server)
    InfoSet res = getCurrentInfoFromDB();

    // New stations to stream
    InfoSet new_stations = getNewInfoFromDB();

    // combine them into common result
    std::copy(new_stations.begin(), new_stations.end(),
              std::inserter(res, res.end()));

    return res;
}

// Processes signal lost event
void Streamer::processSignalLost(const IMessagePtr& msg)

{
    const std::string uuid = msg->getValue(msg::key::RESOURCE);
    const IDevPtr dev = getFactory()->getResources()->getResourceByUUID(uuid);
    BOOST_ASSERT(dev);
    m_dvb_scheduler.stopThread(dev);

    InfoSet del;

    const StationList whole_list = m_info.getInfoList();
    for (StationList::const_iterator item = whole_list.begin();
         item != whole_list.end(); item++)
    {
        StationPtr station = *item;
        if (station->getDev()->getStringParam(dev::UUID) ==
            dev->getStringParam(dev::UUID))
        {
            // mark it as failed
            station->setFailed();
            del.insert(station);
        }
    }

    m_info.del(del, false);
}

// Retrives a list with currently streamed stations
// that are present at the DB
const Streamer::InfoSet Streamer::getCurrentInfoFromDB()
{
    InfoSet res;

    db::DB db(getFactory());
    db.connect();
    const std::string used_host(db.getHostUUID());

    db::ResultVector rv = db.callSelect("klk_app_dvb_station_list",
                                        db::Parameters(), NULL);

    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        // klk_app_dvb_streamer.station,
        // klk_app_dvb_streamer.lock_used_host

        if ((*item)["lock_used_host"].isNull())
        {
            // ignore not locked entries
            continue;
        }

        if ((*item)["lock_used_host"].toString() != used_host)
        {
            // another host uses it
            continue;
        }

        try
        {
            const StationPtr info =
                m_info.getInfoByUUID((*item)["station"].toString());
            res.insert(info);
        }
        catch(...)
        {
            // seems that info marked as used by the current host
            // is not really used
            // it has to be cleared
            db::Parameters params;
            params.add("@station", (*item)["station"].toString());
            params.add("@return_value");
            db::Result result =
                db.callSimple("klk_app_dvb_station_unlock", params);
            BOOST_ASSERT(result["@return_value"].toInt() == 0);
        }
    }

    return res;
}

// Retrives a list with new stations to stream
// that are present at the DB
const Streamer::InfoSet Streamer::getNewInfoFromDB()
{
    InfoSet res;
    StringList processed; // processed sources
    // process all know sources
    IDevList list =
        getFactory()->getResources()->getResourceByType(dev::DVB_ALL);
    for (IDevList::iterator dev = list.begin(); dev != list.end(); dev++)
    {
        try
        {
            const std::string source =
                (*dev)->getStringParam(dev::SOURCE);
            // is it in the processed list
            if (std::find(processed.begin(), processed.end(), source) !=
                processed.end())
            {
                // was already processed
                // ignore it
                continue;
            }
            const std::string type =
                (*dev)->getStringParam(dev::TYPE);
            InfoList dev_res = getNewInfoFromDB(source, type);
            // writing results
            processed.push_back(source);
            std::copy(dev_res.begin(), dev_res.end(),
                      std::inserter(res, res.end()));
        }
        catch(...)
        {
            // just log it
            klk_log(KLKLOG_ERROR,
                    "There was an error while fetching "
                    "new stream info for DVB dev: %s",
                    (*dev)->getStringParam(dev::NAME).c_str());
            continue;
        }
    }

    return res;
}

// Starts stream for specified module info
void Streamer::startStream(const StationPtr& station)
{
    if (!station)
    {
        return;
    }

    m_dvb_scheduler.createStreamThread(station->getDev());
    IStreamPtr stream = m_dvb_scheduler.getStream(station->getDev());
    BOOST_ASSERT(stream);
    stream->addStation(station);

    klk_log(KLKLOG_DEBUG, "DVB streamer has been started to stream. "
            "Channel name: %s", station->getName().c_str());
}
// Stops stream for specified module info
void Streamer::stopStream(const StationPtr& station)  throw()
{
    if (!station)
        return;

    try
    {
        if (station->isStream())
        {
            IStreamPtr stream = m_dvb_scheduler.getStream(station->getDev());
            if (stream)
            {
                // only if there any streamin activity for the dev
                stream->delStation(station);
            }
        }
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Failed to stop DVB stream for '%s': %s",
                station->getName().c_str(), err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Failed to stop DVB stream for '%s': unknown exception",
                station->getName().c_str());
    }

    klk_log(KLKLOG_DEBUG, "DVB streamer has been stopped to stream. "
            "Channel name: %s", station->getName().c_str());
}

// Process changes at the DB
// @param[in] msg - the input message
void Streamer::processDB(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg);
    m_last_dbupdate = time(NULL);

    klk_log(KLKLOG_DEBUG, "Processing DB change event for module '%s'",
            getName().c_str());

    // Full list from DB
    const InfoSet dbdata = getInfoFromDB();
    // List with elements that should be deleted
    InfoSet del = m_info.getDel(dbdata);
    // List with elements that should be added
    InfoSet add = m_info.getAdd(dbdata);

    // delete all elements from the del lis
    std::for_each(del.begin(), del.end(),
                  boost::bind(&Streamer::stopStream, this, _1));
    m_info.del(del, false);

    // add new elements from the new list
    // NOTE: the streams has to be already started
    m_info.add(add, false);
}

// Retrives a list with new stations to stream
// that are present at the DB
const Streamer::InfoList
Streamer::getNewInfoFromDB(const std::string& source, const std::string& type)
{
    db::DB db(getFactory());
    db.connect();
    const std::string used_host(db.getHostUUID());

    // result list
    InfoList res;
    // list stations that was failed to tune
    // and needs to be cleared at the end
    StationList fail_list;

    db::Parameters params;

    //IN used host
    //IN source
    params.add("@used_host", used_host);
    params.add("@source", source);

    db::ResultVector rv = db.callSelect("klk_app_dvb_station_get", params,
                                           NULL);
    // processing results
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT station_name, station, channel, route FROM tmp_res;
        const std::string uuid((*i)["station"].toString());
        const std::string name((*i)["station_name"].toString());
        const std::string channel_uuid((*i)["channel"].toString());
        const std::string route_uuid((*i)["route"].toString());

        StationPtr station =
            StationPtr(new Station(uuid, name, type, source, getFactory()));
        BOOST_ASSERT(station);

        try
        {
            // do tune
            station->setChannel(channel_uuid);
            // reserv route
            station->setRoute(route_uuid);
            // start stream
            startStream(station);

            res.push_back(station);
        }
        catch(const std::exception& err)
        {
            klk_log(KLKLOG_ERROR, "Got exception: %s",
                    err.what());

            fail_list.push_back(station);

            continue;
        }
    }

    // process failed list
    for (StationList::iterator fail = fail_list.begin();
         fail != fail_list.end(); fail++)
    {
        (*fail)->setFailed();
        stopStream(*fail);
    }
    // clear fail list
    fail_list.clear();

    return res;
}

// Clears fail list
void Streamer::clearFailed() throw()
{
    try
    {
        // `klk_app_dvb_failed_station_del` (
        //IN host_uuid VARCHAR(40)
        db::DB db(getFactory());
        db.connect();
        db::Parameters params;
        params.add("@host", db.getHostUUID());
        db.callSimple("klk_app_dvb_failed_station_del", params);
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Exception in Streamer::clearFailed()");
    }
}

// Check failers
void Streamer::checkFailed()
{
    if (m_last_dbupdate.getValue() + FAILURES_UPDATEINTERVAL < time(NULL))
    {
        // we to long don't get db updates

        // clear list with our failed attempts
        clearFailed();
        // try again
        IMessagePtr msg =
            getFactory()->getMessageFactory()->getMessage(msg::id::CHANGEDB);
        addMessage(msg);
    }
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

    StationList list = m_info.getInfoList();
    u_int count = 0;
    for (StationList::iterator i = list.begin(); i != list.end(); i++, count++)
    {
        //klkIndex            Counter32
        //klkStation          DisplayString,
        //klkDestinationAddr  DisplayString,
        //klkDataRate         Integer32,
        //klkDevName          DisplayString

        StationPtr station = *i;
        BOOST_ASSERT(station);

        snmp::TableRow row;
        row.push_back(count);
        row.push_back(station->getName());
        try
        {
            const sock::RouteInfo route = station->getRoute();
            const std::string addr = route.getHost() + ":" +
                boost::lexical_cast<std::string>(route.getPort());
            row.push_back(addr);
            row.push_back(station->getRate());
            if (station->getDev())
            {
                row.push_back(station->getDev()->getStringParam(dev::NAME));
            }
            else
            {
                row.push_back(NOTAVAILABLE);
            }
        }
        catch(...)
        {
            row.clear();
            row.push_back(count);
            row.push_back(station->getName());
            row.push_back(NOTAVAILABLE);
            row.push_back(0);
            row.push_back(NOTAVAILABLE);
        }
        table->addRow(row);
    }

    return table;
}
