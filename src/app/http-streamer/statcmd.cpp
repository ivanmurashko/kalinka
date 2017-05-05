/**
   @file statcmd.cpp
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
   - 2009/04/25 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>

#include "statcmd.h"
#include "defines.h"
#include "exception.h"
#include "httpfactory.h"
#include "streamer.h"
#include "utils.h"
#include "clitable.h"

using namespace klk;
using namespace klk::http;

//
// StatCommand class
//

const std::string STAT_SUMMARY = "Prints HTTP streamer stat info";
const std::string STAT_USAGE = "Usage: " + MODNAME +
    + " " + STAT_COMMAND_NAME + " [" + STAT_COMMAND_PARAM_INPUT +
    "|" + STAT_COMMAND_PARAM_OUTPUT + "]\n";

// Constructor
StatCommand::StatCommand() :
    cli::Command(STAT_COMMAND_NAME, STAT_SUMMARY, STAT_USAGE)
{
}

// Destructor
StatCommand::~StatCommand()
{
}

// Process the command
const std::string StatCommand::process(const cli::ParameterVector& params)
{
    if (params.size() != 1)
    {
        return getUsage();
    }
    else if (params[0] == STAT_COMMAND_PARAM_INPUT)
    {
        return getInputInfo();
    }
    else if (params[0] == STAT_COMMAND_PARAM_OUTPUT)
    {
        return getOutputInfo();
    }

    // incorrect parameter name
    std::stringstream result;

    result << "Parameter '" << params[0] <<
        "' is incorrect. There should be " << STAT_COMMAND_PARAM_INPUT <<
        " or " << STAT_COMMAND_PARAM_OUTPUT;

    return result.str();
}

// Retrives input thread pointer by info
const InThreadPtr StatCommand::getInThread(const InputInfoPtr& info) const
{
    BOOST_ASSERT(info);
    Factory* factory = getModule<Streamer>()->getHTTPFactory();
    BOOST_ASSERT(factory);
    InThreadContainerPtr inthreads = factory->getInThreadContainer();
    BOOST_ASSERT(inthreads);
    InThreadPtr inthread = inthreads->getThreadByPath(info->getPath());
    BOOST_ASSERT(inthread);

    return inthread;
}



// Gets addr str for display
const std::string StatCommand::getAddr(const InputInfoPtr& info) const
{
    InThreadPtr inthread = getInThread(info);

    std::string peer;
    try
    {
        peer = inthread->getReader()->getPeerName();
    }
    catch(...)
    {
        peer = NOTAVAILABLE;
    }

    std::stringstream result;
    result << peer << " -> " << info->getRouteInfo()->getHost()
           << ":" << info->getRouteInfo()->getPort();


    return result.str();
}

// Gets broken package count for display
const std::string StatCommand::getBrokenCount(const InputInfoPtr& info) const
{
    InThreadPtr inthread = getInThread(info);

    std::string result;
    try
    {
        const u_long count = inthread->getReader()->getBrokenCount();
        result = boost::lexical_cast<std::string>(count);
    }
    catch(const Exception&)
    {
        result = NOTAVAILABLE;
    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }

    return result;
}

// Gets input rate for display
const std::string StatCommand::getInputRate(const InputInfoPtr& info) const
{
    InThreadPtr inthread = getInThread(info);

    std::string result;
    try
    {
        const double rate = inthread->getReader()->getRate();
        result = getRate(rate);
    }
    catch(const std::exception&)
    {
        result = NOTAVAILABLE;
    }

    return result;
}

// Retrives rate string from double
const std::string StatCommand::getRate(const double rate) const
{
    std::stringstream result;

    result << base::Utils::memsize2Str(rate) << "/s";

    return result.str();
}

// Retrives input info
const std::string StatCommand::getInputInfo() const
{
    cli::Table table;

    StringList head;
    head.push_back("path");
    head.push_back("input");
    head.push_back("broken");
    head.push_back("rate");
    head.push_back("connections");
    table.addRow(head);

    {
        InputInfoList list = getModule<Streamer>()->getInputInfo();
        Factory* factory = getModule<Streamer>()->getHTTPFactory();
        BOOST_ASSERT(factory);
        ConnectThreadContainerPtr
            connections = factory->getConnectThreadContainer();
        BOOST_ASSERT(connections);
        for (InputInfoList::iterator i = list.begin(); i != list.end(); i++)
        {
            StringList row;
            row.push_back((*i)->getPath());
            row.push_back(getAddr(*i));
            row.push_back(getBrokenCount(*i));
            row.push_back(getInputRate(*i));
            row.push_back(getConnectionCount(*i));
            table.addRow(row);
        }
    }

    return table.formatOutput();
}

// Gets output rate for display
const std::string StatCommand::getOutputRate(const InputInfoPtr& info) const
{
    BOOST_ASSERT(info);
    Factory* factory = getModule<Streamer>()->getHTTPFactory();
    BOOST_ASSERT(factory);
    ConnectThreadContainerPtr
        connections = factory->getConnectThreadContainer();
    BOOST_ASSERT(connections);

    std::string result;
    try
    {
        const double rate =
            connections->getConnectionRate(info->getPath());

        result = getRate(rate);
    }
    catch(const std::exception&)
    {
        result = NOTAVAILABLE;
    }

    return result;
}

// Gets connection count for display
const std::string
StatCommand::getConnectionCount(const InputInfoPtr& info) const
{
    BOOST_ASSERT(info);
    std::string result;
    try
    {
        const u_long count = getInThread(info)->getConnectionCount();
        result = boost::lexical_cast<std::string>(count);
    }
    catch(const Exception&)
    {
        result = NOTAVAILABLE;
    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }

    return result;
}

// Retrives output info
const std::string StatCommand::getOutputInfo() const
{
    cli::Table table;

    StringList head;
    // path input connections
    head.push_back("path");
    head.push_back("connections");
    head.push_back("rate");
    table.addRow(head);

    {
        InputInfoList list = getModule<Streamer>()->getInputInfo();
        for (InputInfoList::iterator i = list.begin(); i != list.end(); i++)
        {
            StringList row;
            row.push_back((*i)->getPath());
            row.push_back(getConnectionCount(*i));
            row.push_back(getOutputRate(*i));
            table.addRow(row);
        }
    }

    return table.formatOutput();
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
StatCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;
    if (setparams.empty())
    {
        res.push_back(STAT_COMMAND_PARAM_INPUT);
        res.push_back(STAT_COMMAND_PARAM_OUTPUT);
    }

    return res;
}
