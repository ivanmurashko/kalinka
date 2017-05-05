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
   - 2009/12/20 created by ipp (Ivan Murashko)
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
using namespace klk::file;

//
// BaseCommand class
//

// Retrives a list with possible source types names
const StringList BaseCommand::getNameList()
{
    StringList list;

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;
    dbparams.add("@host", db.getHostUUID());

    db::ResultVector rv = db.callSelect("klk_file_list",
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
    "[name] [path] [type]\n";

//  Constructor
AddCommand::AddCommand() :
    BaseCommand(ADD_COMMAND_NAME,
                ADD_COMMAND_SUMMARY, ADD_COMMAND_USAGE)
{
}

// Process the command
const std::string AddCommand::process(const cli::ParameterVector& params)
{
    if (params.size() != 3)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        ADD_COMMAND_USAGE);
    }

    const std::string name = params[0];
    // chek the name
    StringList names = getNameList();
    if (std::find(names.begin(), names.end(), name) != names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "The file source name has been already assigned: " + name);
    }
    // check path
    const std::string path = params[1];
    names = getPathList();
    if (std::find(names.begin(), names.end(), path) != names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "The file source path has been already assigned: " + path);
    }
    if (*path.begin() != '/')
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect file source path: " + path);
    }

    // check type
    const std::string type = params[2];
    names = getTypeList();
    if (std::find(names.begin(), names.end(), type) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Unknown file source type: " + type);
    }

    // add the info
    //CREATE PROCEDURE `klk_file_add_cli` (
    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;
    // INOUT file VARCHAR(40),
    // IN name VARCHAR(40),
    // IN file_path VARCHAR(255),
    // IN host VARCHAR(40),
    // IN type_name VARCHAR(128),
    // OUT return_value INT
    dbparams.add("@uuid");
    dbparams.add("@name", name);
    dbparams.add("@path", path);
    dbparams.add("@host", db.getHostUUID());
    dbparams.add("@type_name", type);
    dbparams.add("@return_value");

    db::Result res = db.callSimple("klk_file_add_cli", dbparams);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__, "Failed to add the file");
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
        const StringList names = getNameList();
        // FIXME!!! only 1000 valid names
        for (int i = 0; i < 1000; i++)
        {
            std::stringstream name;
            name << "file" << i;
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
        res.push_back("/tmp/klkfile.bin");
    }
    else if (setparams.size() == 2)
    {
        const StringList names = getTypeList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }

    return res;
}

// Retrives a list with assigned paths
const StringList AddCommand::getPathList()
{
    StringList list;

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;
    dbparams.add("@host", db.getHostUUID());

    db::ResultVector rv = db.callSelect("klk_file_list",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["file_path"].toString());
    }

    return list;
}

// Retrives a list with possible file types
const StringList AddCommand::getTypeList()
{
    StringList list;

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    // SELECT type_name FROM klk_file_type;
    db::ResultVector rv = db.callSelect("klk_file_type_list",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["type_name"].toString());
    }

    return list;
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
    StringList names = getNameList();
    if (std::find(names.begin(), names.end(), name) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect  file name: " + name);
    }

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    // IN _name VARCHAR(255),
    // OUT return_value INT
    dbparams.add("@name", name);
    dbparams.add("@return_value");

    db::Result res = db.callSimple("klk_file_del_cli", dbparams);
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
        const StringList names = getNameList();
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
    head.push_back("path");
    head.push_back("type");
    table.addRow(head);


    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;
    dbparams.add("@host", db.getHostUUID());

    db::ResultVector rv = db.callSelect("klk_file_list",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        StringList row;
        row.push_back((*item)["name"].toString());
        row.push_back((*item)["file_path"].toString());
        row.push_back((*item)["type_name"].toString());
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
