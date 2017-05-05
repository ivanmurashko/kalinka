/**
   @file showcmd.cpp
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
   - 2009/07/05 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cwchar>  // for mbstate_t
#include <locale>

#include "showcmd.h"
#include "exception.h"
#include "defines.h"
#include "utils.h"
#include "db.h"
#include "clitable.h"

// external modules
#include "dvb/defines.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// StationShowCommand class
//

const std::string STATION_SHOW_COMMAND_SUMMARY = "Shows a station to stream";
const std::string STATION_SHOW_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + STATION_SHOW_COMMAND_NAME + "\n";

// Constructor
StationShowCommand::StationShowCommand() :
    cli::Command(STATION_SHOW_COMMAND_NAME,
                 STATION_SHOW_COMMAND_SUMMARY,
                 STATION_SHOW_COMMAND_USAGE)
{
}

// Process the command
const std::string
StationShowCommand::process(const cli::ParameterVector& params)
{
    cli::Table table;

    StringList head;
    head.push_back("channel");
    head.push_back("route");
    head.push_back("priority");
    head.push_back("enable");
    head.push_back("used host");
    table.addRow(head);

    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;
    db::ResultVector rv = db.callSelect("klk_app_dvb_station_list",
                                        dbparams, NULL);

    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        StringList row;
        // klk_dvb_channels.name AS station_name,
        // klk_network_routes.name AS route_name,
        // klk_app_dvb_streamer.enable,
        // klk_app_dvb_streamer.priority,
        // klk_app_dvb_streamer.lock_used_host
        row.push_back((*item)["station_name"].toString());
        row.push_back((*item)["route_name"].toString());
        row.push_back((*item)["priority"].toString());
        row.push_back((*item)["enable"].toString());
        std::string host = "not used";
        if (!(*item)["lock_used_host"].isNull())
        {
            host =
                getUsedHostName((*item)["lock_used_host"].toString());
        }
        row.push_back(host);
        table.addRow(row);
    }

    return table.formatOutput();
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
StationShowCommand::getCompletion(const cli::ParameterVector& setparams)
{
    // no data
    cli::ParameterVector res;
    return res;
}

// Retrives used host name by it's uuid
const std::string
StationShowCommand::getUsedHostName(const std::string& uuid)
{
    db::DB db(getFactory());
    db.connect();
    db::Parameters params;
    // IN host_uuid VARCHAR(40),
    // OUT host_name VARCHAR(40),
    // OUT return_value INT
    params.add("@host_uuid", uuid);
    params.add("@host_name");
    params.add("@return_value");
    db::Result dbres = db.callSimple("klk_host_get_name", params);

    if (dbres["@return_value"].toInt() != 0)
    {
        return "n/a";
    }

    return dbres["@host_name"].toString();
}



//
// NotAssignedShowCommand class
//

const std::string NOT_ASSIGNED_SHOW_COMMAND_SUMMARY = "Shows a station to stream";
const std::string NOT_ASSIGNED_SHOW_COMMAND_USAGE = "Usage: " + MODNAME +
                  + " " + NOT_ASSIGNED_SHOW_COMMAND_NAME + "\n";

// Constructor
NotAssignedShowCommand::NotAssignedShowCommand() :
    cli::Command(NOT_ASSIGNED_SHOW_COMMAND_NAME,
                 NOT_ASSIGNED_SHOW_COMMAND_SUMMARY,
                 NOT_ASSIGNED_SHOW_COMMAND_USAGE)
{
}

// Process the command
const std::string
NotAssignedShowCommand::process(const cli::ParameterVector& params)
{
    cli::Table table;

    StringList head;
    head.push_back("name");
    head.push_back("dvb");
    head.push_back("source");
    head.push_back("type");
    table.addRow(head);

    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;
    db::ResultVector rv = db.callSelect(
        "klk_app_dvb_station_get_not_assinged_channels",
        dbparams, NULL);


    const std::string type_tv("tv");
    const std::string type_radio("radio");
    const std::string type_na("n/a");

    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        StringList row;

        // klk_dvb_channels.channel AS channel,
        // klk_dvb_channels.name ... AS station_name,
        // klk_dvb_channels.provider AS provider_name,
        // klk_dvb_types.name AS dvb_type_name,
        // klk_dvb_signal_sources.signal_source.name AS source_name
        row.push_back((*item)["station_name"].toString());
        row.push_back((*item)["dvb_type_name"].toString());
        row.push_back((*item)["source_name"].toString());

        // channel type TV or radio
        const std::string channel = (*item)["channel"].toString();

        // pids counts
        int vpid = 0, apid = 0;
        dbparams.clear();
        dbparams.add("@channel", channel);
        db::ResultVector rvpids = db.callSelect("klk_dvb_channel_pid_list",
                                                dbparams, NULL);
        for (db::ResultVector::iterator pid = rvpids.begin();
             pid != rvpids.end(); pid++)
        {
            if ((*pid)["pid_type"].toString() == dvb::PID_VIDEO)
            {
                vpid++;
            }
            else if ((*pid)["pid_type"].toString() == dvb::PID_AUDIO)
            {
                apid++;
            }

        }

        if (vpid != 0 && apid != 0)
        {
            row.push_back(type_tv);

        }
        else if (vpid == 0 && apid != 0)
        {
            row.push_back(type_radio);
        }
        else
        {
            row.push_back(type_na);
        }

        table.addRow(row);

    }

    return table.formatOutput();
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
NotAssignedShowCommand::getCompletion(const cli::ParameterVector& setparams)
{
    // no data
    cli::ParameterVector res;
    return res;
}

// Retrives used host name by it's uuid
const std::string
NotAssignedShowCommand::getUsedHostName(const std::string& uuid)
{
    db::DB db(getFactory());
    db.connect();
    db::Parameters params;
    // IN host_uuid VARCHAR(40),
    // OUT host_name VARCHAR(40),
    // OUT return_value INT
    params.add("@host_uuid", uuid);
    params.add("@host_name");
    params.add("@return_value");
    db::Result dbres = db.callSimple("klk_host_get_name", params);

    if (dbres["@return_value"].toInt() != 0)
    {
        return "n/a";
    }

    return dbres["@host_name"].toString();
}
