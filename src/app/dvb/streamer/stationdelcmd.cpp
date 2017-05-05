/**
   @file stationdelcmd.cpp
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

#include "stationdelcmd.h"
#include "exception.h"
#include "defines.h"
#include "db.h"
#include "streamerutils.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// StationDelCommand class
//

const std::string STATION_DEL_COMMAND_SUMMARY = "Deletes a station from stream";
const std::string STATION_DEL_COMMAND_USAGE = "Usage: " +
    MODNAME + " " + STATION_DEL_COMMAND_NAME +
    " [station_name]\n";

// Constructor
StationDelCommand::StationDelCommand() :
    cli::Command(STATION_DEL_COMMAND_NAME,
                 STATION_DEL_COMMAND_SUMMARY,
                 STATION_DEL_COMMAND_USAGE)
{
}

// Process the command
const std::string
StationDelCommand::process(const cli::ParameterVector& params)
{
    if (params.size() != 1)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        STATION_DEL_COMMAND_USAGE);
    }

    const std::string station_name = params[0];

    // check channel name
    StringList names = getAssignedChannels();
    if (std::find(names.begin(), names.end(), station_name) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect station name: " + station_name);
    }

    Utils(getFactory()).delStation(station_name);

    return "Station '" + station_name + "' has been deleted\n";
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
StationDelCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;

    if (setparams.empty())
    {
        const StringList names = getAssignedChannels();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }

    return res;
}

// Retrives possible channel names (not assigned channels)
const StringList
StationDelCommand::getAssignedChannels()
{
    StringList list;
    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;
    db::ResultVector rv = db.callSelect(
        "klk_app_dvb_station_list",
        dbparams, NULL);

    // determine max sizes
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        //klk_dvb_channels.name . '/' . klk_dvb_channels.provider AS
        //   station_name,
        // ...
        list.push_back((*item)["station_name"].toString());
    }
    return list;
}
