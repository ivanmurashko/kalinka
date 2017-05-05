/**
   @file resourcecommand.cpp
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
   - 2009/01/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>
#include <boost/bind.hpp>

#include "resourcecommand.h"
#include "dvb.h"
#include "defines.h"
#include "dvbdev.h"

using namespace klk;
using namespace klk::dvb;


//
// SetSourceCommand class
//

const std::string SETSOURCE_SUMMARY = "Sets a source for DVB resources";
const std::string SETSOURCE_USAGE = "Usage: " + MODNAME +
    +" " + SETSOURCE_COMMAND_NAME + "[dev name] [source name]";

// Constructor
SetSourceCommand::SetSourceCommand() :
    cli::Command(SETSOURCE_COMMAND_NAME, SETSOURCE_SUMMARY, SETSOURCE_USAGE)
{
}

// Process the command
// @param[in] params - the parameters
const std::string
SetSourceCommand::process(const cli::ParameterVector& params)
{
    // check parameters
    if (params.size() != 2)
    {
        throw Exception(__FILE__, __LINE__,
                             "There should be 2 parameters for the command: "
                             "dev name and new source name");
    }

    const std::string dev_name = params[0];
    const std::string source_name = params[1];

    StringList devs = getDevNames();
    if (std::find(devs.begin(), devs.end(), dev_name) == devs.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "The source name is invalid: " + source_name);
    }


    StringList sources = getSourceNames(dev_name);
    if (std::find(sources.begin(), sources.end(), source_name) == sources.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "The source name is invalid: " + source_name);
    }

    // update the db record
    //`klk_dvb_resource_update_signal_source` (
    // IN dev_name VARCHAR(100),
    // IN source_name VARCHAR(100),
    // OUT return_value INT
    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;
    dbparams.add("@dev_name", dev_name);
    dbparams.add("@source_name", source_name);
    dbparams.add("@return_value");
    db::Result res = db.callSimple(
        "klk_dvb_resource_update_signal_source",
        dbparams);

    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__, "DB update failed");
    }

    return "";
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
SetSourceCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector result;

    if (setparams.empty())
    {
        StringList list = getDevNames();
        std::copy(list.begin(), list.end(),
                  std::back_inserter(result));
    }
    else if (setparams.size() == 1)
    {
        // retrive source names for the specified device
        StringList list = getSourceNames(setparams[0]);
        std::copy(list.begin(), list.end(),
                  std::back_inserter(result));
    }

    return result;
}

// Retrives a list with possible source names for the specified dev
const StringList SetSourceCommand::getSourceNames(const std::string& dev_name)
{
    StringList result;
    // retrive the list with possible sources for the DVB dev type
    db::Parameters params;
    //`klk_dvb_signal_source_list` (
    // IN dev_name VARCHAR(40)
    params.add("@dev_name", dev_name);
    // SELECT signal_source,name,description  ...
    db::DB db(getFactory());
    db.connect();
    db::ResultVector rv =
        db.callSelect("klk_dvb_signal_source_list", params, NULL);
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        result.push_back((*i)["name"].toString());
    }

    return result;
}

// Retrives a list with possible DVB dev names
const StringList SetSourceCommand::getDevNames()
{
    StringList result;
    // retrive the list with possible sources for the DVB dev type
    db::DB db(getFactory());
    db.connect();
    db::Parameters params;
    // `klk_resource_list_bytype`$$
    // IN resource_type VARCHAR(40)
    params.add("@restype", dev::DVB_ALL);
    // SELECT signal_source,name,description  ...
    db::ResultVector rv =
        db.callSelect("klk_resource_list_bytype", params, NULL);
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT ...
        // klk_resources.resource_name AS name
        // ...
        result.push_back((*i)["name"].toString());
    }

    return result;
}
