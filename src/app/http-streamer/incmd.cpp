/**
   @file incmd.cpp
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
   - 2009/03/21 created by ipp (Ivan Murashko)
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

#include "incmd.h"
#include "exception.h"
#include "defines.h"
#include "streamer.h"
#include "db.h"
#include "clitable.h"
#include "utils.h"
#include "mod/info.h"

using namespace klk;
using namespace klk::http;

//
// InputShowCommand class
//

const std::string SHOW_SUMMARY = "Prints input HTTP streamer info";
const std::string SHOW_USAGE = "Usage: " + MODNAME +
    + " " + IN_SHOW_COMMAND_NAME + "\n";


// Constructor
InputShowCommand::InputShowCommand() :
    cli::Command(IN_SHOW_COMMAND_NAME, SHOW_SUMMARY, SHOW_USAGE)
{
}

// Destructor
InputShowCommand::~InputShowCommand()
{
}

// Process the command
const std::string
InputShowCommand::process(const cli::ParameterVector& params)
{
    BOOST_ASSERT(params.empty() == true);

    cli::Table table;
    StringList head;
    // name host:port path type
    head.push_back("name");
    head.push_back("address");
    head.push_back("path");
    head.push_back("type");
    table.addRow(head);
    try
    {
        InputInfoList list = getModule<Streamer>()->getInputInfo();
        for (InputInfoList::iterator i = list.begin(); i != list.end(); i++)
        {
            StringList row;
            RouteInfoPtr route = (*i)->getRouteInfo();
            BOOST_ASSERT(route);
            row.push_back(route->getName());
            std::string addr = route->getHost() + ":" +
                boost::lexical_cast<std::string>(route->getPort());
            row.push_back(addr);
            row.push_back((*i)->getPath());
            row.push_back((*i)->getMediaTypeName());
            table.addRow(row);
        }
    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }

    return table.formatOutput();
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
InputShowCommand::getCompletion(const cli::ParameterVector& setparams)
{
    // no parameters
    return cli::ParameterVector();
}


//
// InputAddCommand class
//

const std::string ADD_SUMMARY = "Adds input HTTP streamer info";
const std::string ADD_USAGE = "Usage: " + MODNAME +
    + " " + IN_ADD_COMMAND_NAME
    + " [route name] [path] [media type]\n";


// Constructor
InputAddCommand::InputAddCommand() :
    AddCommand(IN_ADD_COMMAND_NAME, ADD_SUMMARY, ADD_USAGE)
{
}

// Destructor
InputAddCommand::~InputAddCommand()
{
}

// Process the command
const std::string
InputAddCommand::process(const cli::ParameterVector& params)
{
    BOOST_ASSERT(params.size() == 3);

    const std::string name = params[0];
    checkRouteName(name);
    const std::string path = params[1];
    checkPath(path);
    const std::string type = params[2];
    checkMediaType(type);

    const std::string appuuid = getModule<Streamer>()->getAppUUID();
    BOOST_ASSERT(appuuid.empty() == false);

    // assign the name
    db::DB db(getFactory());
    db.connect();

    // `klk_app_http_streamer_route_add4cli` (
    // IN application VARCHAR(40),
    // IN route_name VARCHAR(40),
    // IN path VARCHAR(40),
    // IN media_name VARCHAR(40)
    // OUT return_value INT
    db::Parameters dbparams;
    dbparams.add("@application", appuuid);
    dbparams.add("@name", name);
    dbparams.add("@path", path);
    dbparams.add("@type", type);
    dbparams.add("@return_value");
    db::Result dbres =
        db.callSimple("klk_app_http_streamer_route_add4cli", dbparams);
    if (dbres["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Failed to add the info to the DB");
    }
    return "Route: '" + name + "' has been added\n";
}

struct FindInputInfoByPath
{
    inline bool operator()(const InputInfoPtr info, const std::string path)
    {
        return (info->getPath() == path);
    }
};


// Checks the path parameter
void InputAddCommand::checkPath(const std::string& path)
{
    BOOST_ASSERT(path.empty() == false);
    if (*(path.begin()) != '/')
    {
        throw Exception(__FILE__, __LINE__,
                        "Path '%s' is invalid. "
                        "It should start with '/'",
                        path.c_str());
    }

    // path should be unique
    InputInfoList infos = getModule<Streamer>()->getInputInfo();
    InputInfoList::iterator find = std::find_if(
        infos.begin(), infos.end(),
        boost::bind<bool>(FindInputInfoByPath(), _1, path));
    if (find != infos.end())
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Path '%s' is invalid. "
                             "It it has been already assigned",
                             path.c_str());
    }
}

// Checks the type parameter
void InputAddCommand::checkMediaType(const std::string& type)
{
    BOOST_ASSERT(type.empty() == false);

    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;
    db::ResultVector rv =
        db.callSelect("klk_app_http_streamer_media_type_list", dbparams, NULL);
    std::string possible;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT name
        const std::string name = (*i)["name"].toString();
        if (name  == type)
        {
            // found it - ok
            return;
        }
        if (possible.empty() == false)
        {
            possible += ", ";
        }
        possible += name;
    }

    throw klk::Exception(__FILE__, __LINE__, "Unknown media type '%s'. "
                         "Possible values: %s", type.c_str(),
                         possible.c_str());
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
InputAddCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;

    if (setparams.empty()) // route name
    {
        const StringList names = getRouteList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    else if (setparams.size() == 1) // path
    {
        InputInfoList infos = getModule<Streamer>()->getInputInfo();
        const std::string route = setparams[0];
        // FIXME!!! seems that can not be more than 10000 paths assigned
        for (int i = 0; i < 10000; i++)
        {
            std::stringstream name;
            name << "/" << route; // /route_name
            if (i > 0)
            {
                name << i; // /route_name1, /route_name2 ...
            }

            InputInfoList::iterator find = std::find_if(
                infos.begin(), infos.end(),
                boost::bind<bool>(FindInputInfoByPath(), _1, name.str()));
            if (find == infos.end())
            {
                res.push_back(name.str());
                break;
            }
        }
    }
    else if (setparams.size() == 2) // media type
    {
        db::DB db(getFactory());
        db.connect();
        db::Parameters dbparams;
        db::ResultVector rv =
            db.callSelect("klk_app_http_streamer_media_type_list",
                          dbparams, NULL);
        for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
        {
            // SELECT name
            const std::string name = (*i)["name"].toString();
            res.push_back(name);
        }
    }

    return res;
}


//
// InputDelCommand class
//

const std::string DEL_SUMMARY = "Dels input HTTP streamer info";
const std::string DEL_USAGE = "Usage: " + MODNAME +
    + " " + IN_DEL_COMMAND_NAME + "\n";


// Constructor
InputDelCommand::InputDelCommand() :
    cli::Command(IN_DEL_COMMAND_NAME, DEL_SUMMARY, DEL_USAGE)
{
}

// Destructor
InputDelCommand::~InputDelCommand()
{
}

// Process the command
const std::string
InputDelCommand::process(const cli::ParameterVector& params)
{
    BOOST_ASSERT(params.size() == 1);

    const std::string name = params[0];
    BOOST_ASSERT(name.empty() == false);
    InputInfoList infos = getModule<Streamer>()->getInputInfo();
    StringList names;
    for (InputInfoList::iterator i = infos.begin(); i != infos.end(); i++)
    {
        names.push_back((*i)->getRouteInfo()->getName());
    }
    if (std::find(names.begin(), names.end(), name) == names.end())
    {
        std::string possible = base::Utils::convert2String(names, ", ");
        throw Exception(__FILE__, __LINE__,
                        "Cannot delete route '%s'. Possible values: %s",
                        name.c_str(), possible.c_str());
    }

    // assign the name
    db::DB db(getFactory());
    db.connect();

    // klk_app_http_streamer_route_del_byname` (
    // IN name VARCHAR(40),
    // OUT return_value INT
    db::Parameters dbparams;
    dbparams.add("@name", name);
    dbparams.add("@return_value");
    db::Result dbres =
        db.callSimple("klk_app_http_streamer_route_del_byname", dbparams);
    if (dbres["@return_value"].toInt() != 0)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Failed to del the info from the DB");
    }
    return "Route: '" + name + "' has been deleted\n";
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
InputDelCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;
    if (setparams.empty())
    {
        InputInfoList infos = getModule<Streamer>()->getInputInfo();
        for (InputInfoList::iterator i = infos.begin(); i != infos.end(); i++)
        {
            res.push_back((*i)->getRouteInfo()->getName());
        }
    }
    return res;
}
