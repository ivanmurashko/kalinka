/**
   @file streamerutils.cpp
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
   - 2009/08/02 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "streamerutils.h"
#include "db.h"
#include "exception.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// Utils class
//

// Constructor
Utils::Utils(IFactory* factory) : m_factory(factory)
{
    BOOST_ASSERT(m_factory);
}

// Adds a station with specified name
void Utils::addStation(const std::string& station, const std::string& route,
                       const char* priority)
{
    db::DB db(m_factory);
    db.connect();
    db::Parameters params;
    // IN channel_name VARCHAR(255),
    // IN route_name VARCHAR(255),
    // IN priority INTEGER,
    // OUT return_value INT
    params.add("@station_name", station);
    params.add("@route_name", route);
    if (priority)
    {
        params.add("@priority", priority);
    }
    else
    {
        params.add("@priority");
    }

    params.add("@return_value");
    db::Result dbres = db.callSimple("klk_app_dvb_station_add_cli",
                                        params);
    if (dbres["@return_value"].toInt() != 0)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Failed to add station: %s -> %s",
                             station.c_str(), route.c_str());
    }
}


// Deletes a station with specified name
void Utils::delStation(const std::string& station)
{
    db::DB db(m_factory);
    db.connect();
    db::Parameters dbparams;
    // IN channel_name VARCHAR(255),
    // OUT return_value INT
    dbparams.add("@channel_name", station);
    dbparams.add("@return_value");
    db::Result dbres = db.callSimple("klk_app_dvb_station_del_cli",
                                        dbparams);

    if (dbres["@return_value"].toInt() != 0)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Failed to del station: %s",
                             station.c_str());
    }
}
