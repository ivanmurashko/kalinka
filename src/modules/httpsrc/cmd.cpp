/**
   @file cmd.cpp
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
   - 2009/12/19 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <sstream>
#include <algorithm>

#include "db.h"
#include "cmd.h"
#include "defines.h"
#include "clitable.h"

using namespace klk;
using namespace klk::httpsrc;

//
// BaseCommand class
//

// Retrives a list with possible source types names
const StringList BaseCommand::getSourceNameList()
{
    StringList list;

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    db::ResultVector rv = db.callSelect("klk_httpsrc_list",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["name"].toString());
    }

    return list;
}

//
// AddCommand class
//

/**
    add command name
*/
const std::string ADD_COMMAND_NAME = "add";

const std::string ADD_COMMAND_SUMMARY =
    "Adds a  info";
const std::string ADD_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + ADD_COMMAND_NAME +
    "[name] [http address] [login] [pasword]\n";

namespace
{
    const char* HTTP_SCHEME = "http://";
    const char* HTTPS_SCHEME = "https://";
}

//  Constructor
AddCommand::AddCommand() :
    BaseCommand(ADD_COMMAND_NAME,
                ADD_COMMAND_SUMMARY, ADD_COMMAND_USAGE)
{
}

// Destructor
AddCommand::~AddCommand()
{
}

// Process the command
const std::string AddCommand::process(const cli::ParameterVector& params)
{
    if (params.size() < 2 || params.size() > 4)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        ADD_COMMAND_USAGE);
    }

    const std::string name = params[0];

    // chek the name
    StringList names = getSourceNameList();
    if (std::find(names.begin(), names.end(), name) != names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "The http source name has been already assigned: " + name);
    }

    const std::string address = params[1];
    // check the scheme
    if ((strncasecmp(address.c_str(), HTTP_SCHEME, strlen(HTTP_SCHEME)) != 0) &&
        (strncasecmp(address.c_str(), HTTPS_SCHEME, strlen(HTTPS_SCHEME)) != 0))
    {
        throw Exception(__FILE__, __LINE__, "Incorrect scheme: " + address);
    }

    std::string login, passwd;
    if (params.size() >= 3)
    {
        login = params[2];
    }
    if (params.size() == 4)
    {
        passwd = params[3];
    }

    // add the info
    //CREATE PROCEDURE `klk_httpsrc_add` (
    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;
    // INOUT httpsrc VARCHAR(40),
    // IN title VARCHAR(64),
    // IN address VARCHAR(255),
    // IN login VARCHAR(32),
    // IN passwd VARCHAR(32),
    // IN description VARCHAR(255),
    // OUT return_value INT
    dbparams.add("@uuid");
    dbparams.add("@name", name);
    dbparams.add("@address", address);
    dbparams.add("@login", login);
    dbparams.add("@passwd", passwd);
    dbparams.add("@description", "");
    dbparams.add("@return_value");

    db::Result res = db.callSimple("klk_httpsrc_add", dbparams);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__, "Failed to add the ");
    }
    return " '" + name + "' has been added\n";
}

// gets completion
const cli::ParameterVector
AddCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;
    if (setparams.empty())
    {
        // should no be already assigned
        const StringList names = getSourceNameList();
        // FIXME!!! only 1000 valid names
        for (int i = 0; i < 1000; i++)
        {
            std::stringstream name;
            name << "httpsrc" << i;
            if (std::find(names.begin(), names.end(), name.str()) == names.end())
            {
                // found it
                res.push_back(name.str());
                break;
            }
        }
    }
    if (setparams.size() == 1)
    {
        res.push_back(HTTP_SCHEME);
        res.push_back(HTTPS_SCHEME);
    }

    return res;
}

//
// DelCommand class
//

/**
    del command name
*/
const std::string DEL_COMMAND_NAME = "delete";

const std::string DEL_COMMAND_SUMMARY =
    "Deletes a  info";
const std::string DEL_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + DEL_COMMAND_NAME + " [name]\n";


//  Constructor
DelCommand::DelCommand() :
    BaseCommand(DEL_COMMAND_NAME,
                DEL_COMMAND_SUMMARY, DEL_COMMAND_USAGE)
{
}


// Destructor
DelCommand::~DelCommand()
{
}

// Process the command
const std::string DelCommand::process(const cli::ParameterVector& params)
{
    if (params.size() != 1)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        DEL_COMMAND_USAGE);
    }

    const std::string name = params[0];

    // check  type name
    StringList names = getSourceNameList();
    if (std::find(names.begin(), names.end(), name) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect  http source name: " + name);
    }

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    // IN _name VARCHAR(255),
    // OUT return_value INT
    dbparams.add("@name", name);
    dbparams.add("@return_value");

    db::Result res = db.callSimple("klk_httpsrc_del_cli", dbparams);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__, "Failed to delete the http source");
    }

    return " '" + name + "' has been deleted\n";
}

// gets completion
const cli::ParameterVector
DelCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;
    if (setparams.empty())
    {
        const StringList names = getSourceNameList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    return res;
}

//
// ShowCommand class
//

/**
    show command name
*/
const std::string SHOW_COMMAND_NAME = "show";

const std::string SHOW_COMMAND_SUMMARY =
    "Shows a http source info list";
const std::string SHOW_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + SHOW_COMMAND_NAME + "\n";


//  Constructor
ShowCommand::ShowCommand() :
    cli::Command(SHOW_COMMAND_NAME,
                 SHOW_COMMAND_SUMMARY, SHOW_COMMAND_USAGE)
{
}


// Destructor
ShowCommand::~ShowCommand()
{
}

// Process the command
const std::string ShowCommand::process(const cli::ParameterVector& params)
{
    if (!params.empty())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        SHOW_COMMAND_USAGE);
    }

    cli::Table table;

    StringList head;
    head.push_back("name");
    head.push_back("address");
    head.push_back("login");
    head.push_back("passwd");
    table.addRow(head);

    // `klk_httpsrc_list` (
    // SELECT httpsrc AS uuid, title AS name, address AS addr,
    // login AS login, passwd AS password FROM klk_httpsrc;

    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;

    db::ResultVector rv = db.callSelect("klk_httpsrc_list",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        StringList row;
        row.push_back((*item)["name"].toString());
        row.push_back((*item)["addr"].toString());
        if (!(*item)["login"].toString().empty())
            row.push_back("set");
        else
            row.push_back("not set");
        if (!(*item)["password"].toString().empty())
            row.push_back("set");
        else
            row.push_back("not set");
        table.addRow(row);
    }

    return table.formatOutput();
}

// gets completion
const cli::ParameterVector
ShowCommand::getCompletion(const cli::ParameterVector& setparams)
{
    return cli::ParameterVector();
}
