/**
   @file outcmd.cpp
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
   - 2009/03/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>

#include "outcmd.h"
#include "exception.h"
#include "defines.h"
#include "streamer.h"
#include "db.h"
#include "utils.h"
#include "mod/info.h"

using namespace klk;
using namespace klk::http;

//
// OutputShowCommand class
//

const std::string SHOW_SUMMARY = "Prints output HTTP streamer info";
const std::string SHOW_USAGE = "Usage: " + MODNAME +
    + " " + OUT_SHOW_COMMAND_NAME + "\n";


// Constructor
OutputShowCommand::OutputShowCommand() :
    cli::Command(OUT_SHOW_COMMAND_NAME, SHOW_SUMMARY, SHOW_USAGE)
{
}

// Destructor
OutputShowCommand::~OutputShowCommand()
{
}

// Process the command
const std::string
OutputShowCommand::process(const cli::ParameterVector& params)
{
    BOOST_ASSERT(params.empty() == true);

    const RouteInfoPtr route =
        getModule<Streamer>()->getHTTPFactory()->getOutThread()->getRoute();

    if (route->getUUID().empty())
    {
        throw Exception(__FILE__, __LINE__,
                        "The output info for HTTP streamer "
                        "has not been set");
    }

    std::stringstream result;
    result << "HTTP streamer output info:\n";
    result << "Route name : " << route->getName() << "\n";
    result << "Route setup: " << route->getHost() << ":"
           << route->getPort() << "\n";
    return result.str();
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
OutputShowCommand::getCompletion(const cli::ParameterVector& setparams)
{
    // no parameters
    return cli::ParameterVector();
}

//
// OutputSetCommand class
//

const std::string SET_SUMMARY = "Sets output HTTP streamer info";
const std::string SET_USAGE = "Usage: " + MODNAME +
                                 + " " + OUT_SET_COMMAND_NAME + " [route_name]\n";


// Constructor
OutputSetCommand::OutputSetCommand() :
    AddCommand(OUT_SET_COMMAND_NAME, SET_SUMMARY, SET_USAGE)
{
}

// Destructor
OutputSetCommand::~OutputSetCommand()
{
}


// Process the command
const std::string
OutputSetCommand::process(const cli::ParameterVector& params)
{
    BOOST_ASSERT(params.size() == 1);
    // retrive current route
    const std::string name = params[0];
    if (getModule<Streamer>()->getHTTPFactory()->getOutThread()->checkRoute())
    {
        const RouteInfoPtr route =
            getModule<Streamer>()->getHTTPFactory()->getOutThread()->getRoute();
        if (route->getName() == name)
        {
            throw Exception(__FILE__, __LINE__,
                            "The route '%s' is already set", name.c_str());
        }
    }

    // check the parameter
    checkRouteName(name);

    const std::string appuuid = getModule<Streamer>()->getAppUUID();
    BOOST_ASSERT(appuuid.empty() == false);

    // assign the name
    db::DB db(getFactory());
    db.connect();

    //`klk_app_http_streamer_set` (
    //IN application VARCHAR(40),
    //IN route_name VARCHAR(40),
    //OUT return_value INT
    db::Parameters dbparams;
    dbparams.add("@application", appuuid);
    dbparams.add("@route_name", name);
    dbparams.add("@return_value", name);
    db::Result dbres = db.callSimple("klk_app_http_streamer_set", dbparams);
    if (dbres["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Failed to set the info at the DB");
    }
    return "Route: '" + name + "' has been set\n";
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
OutputSetCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;

    if (setparams.empty())
    {
        const StringList names = getRouteList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }

    return res;
}
