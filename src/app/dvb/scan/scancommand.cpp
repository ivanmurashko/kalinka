/**
   @file scancommand.cpp
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/11/15 created by ipp (Ivan Murashko)
   - 2009/01/17 renamed to scancommand.h by ipp (Ivan Murashko)
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
#include <boost/algorithm/string/erase.hpp>

#include "scan.h"
#include "scancommand.h"
#include "defines.h"
#include "paths.h"
#include "utils.h"

// module specific
#include "dvb/dvbdev.h"
#include "dvb/utils/utils.h"

using namespace klk;
using namespace klk::dvb::scan;

//
// ScanStart class
//

const std::string SCAN_START_NAME = "start";
const std::string SCAN_START_SUMMARY = "Starts scan process";
const std::string SCAN_START_USAGE = "Usage: " + MODNAME +
    + " " + SCAN_START_NAME + " [source_name] [frequency_table_name]\n";

// Constructor
ScanStart::ScanStart() :
    cli::Command(SCAN_START_NAME, SCAN_START_SUMMARY, SCAN_START_USAGE)
{
}

// find a resource with the specefied source_name
struct IsMatchResource
{
    inline bool operator()(IFactory* factory,
                           const IDevPtr dev,
                           const std::string source_name)
        {
            const std::string source_uuid = dev->getStringParam(dev::SOURCE);
            const std::string dev_source_name =
                dvb::Utils::getSourceName(factory, source_uuid);
            return (dev_source_name == source_name);
        }
};

// Retrive dev from parameter
const IDevPtr ScanStart::getDev(const std::string& source_name)
{
    IDevList list =
        getFactory()->getResources()->getResourceByType(dev::DVB_ALL);
    IDevList::iterator dev = std::find_if(
        list.begin(), list.end(),
        boost::bind<bool>(IsMatchResource(), getFactory(), _1, source_name));
    if (dev == list.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "There is no any DVB adapter for '" +
                        source_name + "'");
    }

    return *dev;
}


// Process the command
const std::string ScanStart::process(
    const cli::ParameterVector& params)

{
    if (getModule<Scan>()->isScanStarted() == true)
    {
        return "Scan is already running\n";
    }

    if (params.size() != 2)
    {
        return getUsage();
    }

    // check source
    const std::string source_name(params[0]);
    IDevPtr dev = getDev(source_name);

    // check frequency table
    std::string scanfile = params[1];

    if (*scanfile.begin() != '/')
    {
        if (dev->getStringParam(dev::TYPE) == dev::DVB_T)
        {
            scanfile = dir::SHARE + "/scan/dvb-t/" + scanfile;
        }
        else if (dev->getStringParam(dev::TYPE) == dev::DVB_S)
        {
            scanfile = dir::SHARE + "/scan/dvb-s/" + scanfile;
        }
        else
        {
            return "Unsupported DVB dev type\n";
        }
    }

    if (base::Utils::fileExist(scanfile) == false)
    {
        return "Scan data file does not exist: " + scanfile + "\n";
    }

    getModule<Scan>()->startScan(
        dev->getStringParam(dev::SOURCE),
        scanfile);
    std::string info = "started scan\n";
    return info;
}

// Gets the command usage description
const std::string ScanStart::getUsage() const
{
    return SCAN_START_USAGE;
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
ScanStart::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;

    if (setparams.empty())
    {
        // possible source names
        IDevList list =
            getFactory()->getResources()->getResourceByType(dev::DVB_ALL);
        for (IDevList::iterator i = list.begin(); i != list.end(); i++)
        {
            const std::string source = (*i)->getStringParam(dev::SOURCE);
            if (!source.empty())
            {
                const std::string source_name =
                    dvb::Utils::getSourceName(getFactory(),
                                              source);
                if (std::find(res.begin(), res.end(), source_name) ==
                    res.end())
                {
                    res.push_back(source_name);
                }
            }
        }
    }
    else if (setparams.size() == 1)
    {
        IDevPtr dev = getDev(setparams[0]);
        // possible frequency table
        StringList list;
        if (dev->getStringParam(dev::TYPE) == dev::DVB_T)
        {
            list = base::Utils::getFiles(dir::SHARE + "/scan/dvb-t");
        }
        else if (dev->getStringParam(dev::TYPE) == dev::DVB_S)
        {
            list = base::Utils::getFiles(dir::SHARE + "/scan/dvb-s");
        }

        for (StringList::iterator i = list.begin(); i != list.end(); i++)
        {
            const std::string fname = base::Utils::getFileName(*i);
            if (std::find(res.begin(), res.end(), fname) == res.end())
            {
                res.push_back(fname);
            }
        }
    }

    return res;
}


//
// ScanStop class
//

const std::string SCAN_STOP_NAME = "stop";
const std::string SCAN_STOP_SUMMARY = "Stops scan process";
const std::string SCAN_STOP_USAGE = "Usage: " + MODNAME +
    + " " + SCAN_STOP_NAME + "\n";

// Constructor
ScanStop::ScanStop() :
    cli::Command(SCAN_STOP_NAME, SCAN_STOP_SUMMARY, SCAN_STOP_USAGE)
{
}

// Process the command
const std::string ScanStop::process(
    const cli::ParameterVector& params)

{
    if (getModule<Scan>()->isScanStarted() == false)
    {
        return "Scan is not running\n";
    }

    std::stringstream data;
    getModule<Scan>()->stopScan();
    data << "Scan stopped";
    return data.str();
}


// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
ScanStop::getCompletion(const cli::ParameterVector& setparams)
{
    // no parameters
    return cli::ParameterVector();
}
