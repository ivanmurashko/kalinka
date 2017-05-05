/**
   @file sourcecmd.cpp
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
   - 2009/11/06 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sourcecmd.h"
#include "exception.h"
#include "defines.h"
#include "db.h"
#include "clitable.h"

using namespace klk;
using namespace klk::trans;

//
// SourceAddCommand class
//

/**
   Source add command name
*/
const std::string SOURCEADD_COMMAND_NAME = "source add";

const std::string SOURCEADD_COMMAND_SUMMARY =
    "Adds a source info";
const std::string SOURCEADD_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + SOURCEADD_COMMAND_NAME +
    " [source type] [source name] [media type name]\n";


//  Constructor
SourceAddCommand::SourceAddCommand() :
    cli::Command(SOURCEADD_COMMAND_NAME,
                 SOURCEADD_COMMAND_SUMMARY, SOURCEADD_COMMAND_USAGE)
{
}

// Destructor
SourceAddCommand::~SourceAddCommand()
{
}

// Process the command
const std::string SourceAddCommand::process(const cli::ParameterVector& params)
{
    if (params.size() != 3)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        SOURCEADD_COMMAND_USAGE);
    }

    const std::string source_type_name = params[0];
    const std::string source_name = params[1];
    const std::string media_type = params[2];


    // check source type name
    StringList names = getSourceTypeNameList();
    if (std::find(names.begin(), names.end(), source_type_name) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect source type name: " + source_type_name);
    }
    // check source name
    names = getSourceNameList(source_type_name);
    if (std::find(names.begin(), names.end(), source_name) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect source name: " + source_name);
    }

    // check source name
    names = getMediaTypeList();
    if (std::find(names.begin(), names.end(), media_type) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect media type name: " + media_type);
    }


    // add the source
    const std::string name = addSource(source_type_name, source_name, media_type);

    return "Source '" + name + "' has been added\n";
}

// gets completion
const cli::ParameterVector
SourceAddCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;
    if (setparams.empty())
    {
        const StringList names = getSourceTypeNameList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    else if (setparams.size() == 1)
    {
        const StringList names = getSourceNameList(setparams[0]);
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    else if (setparams.size() == 2)
    {
        const StringList names = getMediaTypeList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }

    return res;
}

// Retrives a list with possible source types names
const StringList
SourceAddCommand::getSourceTypeNameList()
{
    StringList list;

    // `klk_app_transcode_source_type_get_cli` (
    // SELECT klk_app_transcode_source_type.type_name AS name FROM
    // klk_app_transcode_source_type;

    db::DB db(getFactory());
    db.connect();
    db::Parameters params;
    db::ResultVector rv = db.callSelect("klk_app_transcode_source_type_get_cli",
                                        params, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["name"].toString());
    }

    return list;
}

// Retrives a list with possible source names
// for the specified type
const StringList
SourceAddCommand::getSourceNameList(const std::string& type_name)
{
    StringList list;

    // `klk_app_transcode_source_name_missing_get_cli` (
    // SELECT XXX AS name FROM

    db::DB db(getFactory());
    db.connect();
    db::Parameters params;
    params.add("@type_name", type_name);
    db::ResultVector rv =
        db.callSelect("klk_app_transcode_source_name_missing_get_cli", params, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["name"].toString());
    }

    return list;
}

// Adds a source with specified parameters
const std::string
SourceAddCommand::addSource(const std::string& type,  const std::string& name,
                            const std::string& media_type_name)
{
    db::DB db(getFactory());
    db.connect();

    db::Parameters params;

    // IN source_type_name VARCHAR(255),
    // IN source_name VARCHAR(255),
    // IN media_type_name VARCHAR(255),
    // OUT assigned_source_name VARCHAR(255)
    params.add("@type", type);
    params.add("@name", name);
    params.add("@media_type_name", media_type_name);
    params.add("@assigned");

    db::Result res = db.callSimple("klk_app_transcode_source_add_cli", params);
    if (res["@assigned"].isNull())
    {
        throw Exception(__FILE__, __LINE__, "Failed to add the source");
    }

    return res["@assigned"].toString();

}

// Retrives a list with possible media types names
const StringList
SourceAddCommand::getMediaTypeList()
{
    StringList list;

    // klk_media_type_list` (
    //SELECT klk_media_types.name AS name FROM klk_media_types;

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    db::ResultVector rv = db.callSelect("klk_media_type_list",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["name"].toString());
    }

    return list;
}


//
// SourceDelCommand class
//

/**
   Source del command name
*/
const std::string SOURCEDEL_COMMAND_NAME = "source delete";

const std::string SOURCEDEL_COMMAND_SUMMARY =
    "Deletes a source info";
const std::string SOURCEDEL_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + SOURCEDEL_COMMAND_NAME + " [source name]\n";


//  Constructor
SourceDelCommand::SourceDelCommand() :
    BaseCommand(SOURCEDEL_COMMAND_NAME,
                SOURCEDEL_COMMAND_SUMMARY, SOURCEDEL_COMMAND_USAGE)
{
}


// Destructor
SourceDelCommand::~SourceDelCommand()
{
}

// Process the command
const std::string SourceDelCommand::process(const cli::ParameterVector& params)
{
    if (params.size() != 1)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        SOURCEDEL_COMMAND_USAGE);
    }

    const std::string source_name = params[0];


    // check source type name
    StringList names = getSourceNameList();
    if (std::find(names.begin(), names.end(), source_name) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect source name: " + source_name);
    }

    // del the source
    delSource(source_name);

    return "Source '" + source_name + "' has been deleted\n";
}

// gets completion
const cli::ParameterVector
SourceDelCommand::getCompletion(const cli::ParameterVector& setparams)
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


// Deletes a source with specified parameters
void SourceDelCommand::delSource(const std::string& name)
{
    db::DB db(getFactory());
    db.connect();

    db::Parameters params;

    // IN source_name VARCHAR(255),
    // OUT return_value INT
    params.add("@name", name);
    params.add("@return_value");

    db::Result res = db.callSimple("klk_app_transcode_source_del_cli", params);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__, "Failed to delete the source");
    }
}


//
// SourceShowCommand class
//

/**
   Source show command name
*/
const std::string SOURCESHOW_COMMAND_NAME = "source show";

const std::string SOURCESHOW_COMMAND_SUMMARY =
    "Shows a source info list";
const std::string SOURCESHOW_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + SOURCESHOW_COMMAND_NAME + "\n";


//  Constructor
SourceShowCommand::SourceShowCommand() :
    cli::Command(SOURCESHOW_COMMAND_NAME,
                 SOURCESHOW_COMMAND_SUMMARY, SOURCESHOW_COMMAND_USAGE)
{
}


// Destructor
SourceShowCommand::~SourceShowCommand()
{
}

// Process the command
const std::string SourceShowCommand::process(const cli::ParameterVector& params)
{
    if (!params.empty())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        SOURCESHOW_COMMAND_USAGE);
    }

    cli::Table table;

    StringList head;
    head.push_back("name");
    head.push_back("type");
    head.push_back("media");
    table.addRow(head);

    // `klk_app_transcode_source_show_cli` (
    // SELECT klk_app_transcode_source.source_name AS source_name,
    // klk_app_transcode_source_type.type_name AS type_name ....

    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;

    db::ResultVector rv = db.callSelect("klk_app_transcode_source_show_cli",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        StringList row;
        row.push_back((*item)["source_name"].toString());
        row.push_back((*item)["type_name"].toString());
        row.push_back((*item)["media_type_name"].toString());
        table.addRow(row);
    }

    return table.formatOutput();
}

// gets completion
const cli::ParameterVector
SourceShowCommand::getCompletion(const cli::ParameterVector& setparams)
{
    return cli::ParameterVector();
}
