/**
   @file stationaddcmd.cpp
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
   - 2009/02/13 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>

#include "stationaddcmd.h"
#include "exception.h"
#include "defines.h"
#include "db.h"
#include "streamerutils.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// StationAddCommand class
//

const std::string STATION_ADD_COMMAND_SUMMARY = "Adds a station to stream";
const std::string STATION_ADD_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + STATION_ADD_COMMAND_NAME +
    " [station_name] [route_name] ([priority])\n";

// Constructor
StationAddCommand::StationAddCommand() :
    cli::Command(STATION_ADD_COMMAND_NAME,
                 STATION_ADD_COMMAND_SUMMARY,
                 STATION_ADD_COMMAND_USAGE)
{
}

// Process the command
const std::string
StationAddCommand::process(const cli::ParameterVector& params)
{
    if (params.size() != 2 && params.size() != 3)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        STATION_ADD_COMMAND_USAGE);
    }

    const std::string station_name = params[0];
    const std::string route_name = params[1];


    // check channel name
    StringList names = getNotAssignedChannels();
    if (std::find(names.begin(), names.end(), station_name) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect channel name: " + station_name);
    }
    // check routes
    names = getNotAssignedRoutes();
    if (std::find(names.begin(), names.end(), route_name) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect route name: " + route_name);
    }
    // check priority
    if (params.size() == 3)
    {
        try
        {
            boost::lexical_cast<int>(params[2]);
        }
        catch(const boost::bad_lexical_cast&)
        {
            throw Exception(__FILE__, __LINE__,
                            "Incorrect priority parameter: "
                            "should be integer");
        }
    }


    const char* priority = NULL;
    if (params.size() == 3)
    {
        priority = params[2].c_str();
    }

    Utils(getFactory()).addStation(station_name, route_name, priority);

    return "Station '" + station_name + "' has been added\n";
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
StationAddCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;

    if (setparams.empty())
    {
        const StringList names = getNotAssignedChannels();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    else if (setparams.size() == 1)
    {
        const StringList names = getNotAssignedRoutes();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    else if (setparams.size() == 2)
    {
        for (int i = 0; i < 10; i++)
        {
            res.push_back(boost::lexical_cast<std::string>(i));
        }
    }

    return res;
}

// Retrives possible channel names (not assigned channels)
const StringList
StationAddCommand::getNotAssignedChannels()
{
    StringList list;
    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;
    db::ResultVector rv = db.callSelect(
        "klk_app_dvb_station_get_not_assinged_channels",
        dbparams, NULL);

    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["station_name"].toString());
    }
    return list;
}

// Retrives possible route names (not assigned routes)
const StringList
StationAddCommand::getNotAssignedRoutes()
{
    // FIXME!!! need refactoring
    // (same realization as getNotAssignedChannels)
    StringList list;
    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;
    db::ResultVector rv = db.callSelect(
        "klk_app_dvb_station_get_not_assinged_routes",
        dbparams, NULL);

    // determine max sizes
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["name"].toString());
    }
    return list;
}
