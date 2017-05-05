/**
   @file infocommand.cpp
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
   - 2009/01/17 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "infocommand.h"
#include "defines.h"
#include "dvbdev.h"
#include "utils.h"
#include "dvb.h"
#include "utils/utils.h"

using namespace klk;
using namespace klk::dvb;

//
// InfoCommand class
//

const std::string INFOSUMMARY = "Lists all dvb cards and status information. when adapter name is given, the selected adapter is shown. all lists all available adapters.";
const std::string INFOUSAGE = "Usage: " + MODNAME +
    +" " + INFONAME + " [resource_name] | [all]\n";

// Constructor
InfoCommand::InfoCommand() :
    BaseCommand(INFONAME, INFOSUMMARY, INFOUSAGE)
{
}

// Process the command
// @param[in] params - the parameters
const std::string InfoCommand::process(const cli::ParameterVector& params)

{
    StringList list = getDevNames();

    if (list.empty())
    {
        return "There is no any DVB cards\n";
    }

    if (params.empty() || params.size() > 1)
    {
        return INFOUSAGE;
    }

    BOOST_ASSERT(params.size() == 1);
    std::string result;
    if (params[0] == cli::ALL)
    {
        std::for_each(list.begin(), list.end(),
                      boost::bind(&InfoCommand::getInfo,
                                  this, _1, &result));
    }
    else
    {
        StringList::iterator i = std::find(
            list.begin(), list.end(),
            params[0]);
        if (i == list.end())
        {
            result = "Unknown device name: " + params[0] + "\n";
        }
        else
        {
            getInfo(*i, &result);
        }
    }

    return result;
}

// Retrives specified param info for a dev
const std::string
InfoCommand::getParamInfo(const std::string& param, const IDevPtr& dev) const
{
    if (dev->hasParam(param))
    {
        return dev->getStringParam(param);
    }

    return NOTAVAILABLE;
}

// Retrives info about the specified dev
void InfoCommand::getInfo(const std::string& name,
                          std::string* result)
{
    BOOST_ASSERT(result);
    const std::string NAME = "name: ";
    const std::string ADAPTER = "adapter: ";
    const std::string FRONTEND = "frontend: ";
    const std::string TYPE = "type: ";
    const std::string SIGNAL_SOURCE = "signal source: ";
    const std::string FREQUENCY = "frequency: ";
    const std::string STATUS = "status: ";
    const std::string SIGNAL = "signal: ";
    const std::string SNR = "snr: ";
    const std::string BER = "ber: ";
    const std::string UNC = "unc: ";
    const std::string BANDWIDTH = "bandwidth: ";
    const std::string ACTIVITY = "activity: ";
    const std::string TUNED_CHANNELS = "tuned channels: ";

    // FIXME!!! add check all lengths
    size_t max_size = TUNED_CHANNELS.size();

    IDevPtr dev = getDev(name);
    *result += "\n\n";
    *result += base::Utils::align(NAME, max_size) +
        dev->getStringParam(dev::NAME) + "\n";
    *result += base::Utils::align(ADAPTER, max_size) +
        dev->getStringParam(dev::ADAPTER) + "\n";
    *result += base::Utils::align(FRONTEND, max_size) +
        dev->getStringParam(dev::FRONTEND) + "\n";

    *result += base::Utils::align(TYPE, max_size);
    if (dev->getStringParam(dev::TYPE) == dev::DVB_S)
    {
        *result += DVB_S_NAME;
    }
    else if (dev->getStringParam(dev::TYPE) == dev::DVB_T)
    {
        *result += DVB_T_NAME;
    }
    else if (dev->getStringParam(dev::TYPE) == dev::DVB_C)
    {
        *result += DVB_C_NAME;
    }
    else
    {
        BOOST_ASSERT(false);
    }
    *result += "\n";

    *result += base::Utils::align(FREQUENCY, max_size) +
        getParamInfo(dev::FREQUENCY, dev) + " Hz\n";

    std::string source_name;
    try
    {
        source_name = Utils::getSourceName(getFactory(),
                                           dev->getStringParam(dev::SOURCE));
    }
    catch(...)
    {
        source_name = NOTAVAILABLE;
    }


    *result += base::Utils::align(SIGNAL_SOURCE, max_size) +
        source_name + "\n";

    *result += base::Utils::align(STATUS, max_size);
    if (dev->hasParam(dev::HASLOCK))
    {
        if (dev->getStringParam(dev::HASLOCK) == "0")
        {
            *result += "does not have lock";
        }
        else
        {
            *result += "has lock";
        }
    }
    *result += "\n";

    *result += base::Utils::align(SIGNAL, max_size) +
        getParamInfo(dev::SIGNAL, dev) + "\n";
    *result += base::Utils::align(SNR, max_size) +
        getParamInfo(dev::SNR, dev) + "\n";
    *result += base::Utils::align(BER, max_size) +
        getParamInfo(dev::BER, dev) + "\n";
    *result += base::Utils::align(UNC, max_size) +
        getParamInfo(dev::UNC, dev) + "\n";
    *result += base::Utils::align(BANDWIDTH, max_size) +
        getParamInfo(dev::BANDWIDTH, dev) + " kHz\n";

    if (dev->getState() == dev::IDLE)
    {
        *result += base::Utils::align(ACTIVITY, max_size) +
            "idle\n";
    }
    else if (dev->getStringParam(dev::DVBACTIVITY) == dev::STREAMING)
    {
        *result += base::Utils::align(ACTIVITY, max_size) +
            "in use\n";

        *result += base::Utils::align(TUNED_CHANNELS, max_size);
        StringList list =
            getModule<DVB>()->getProcessor()->getChannelNames(dev);
        for (StringList::iterator i = list.begin(); i !=list.end(); i++)
        {
            if (i != list.begin())
                *result += ", ";
            *result += *i;
        }

        *result += "\n";
    }
    else if (dev->getStringParam(dev::DVBACTIVITY) == dev::SCANING)
    {
        *result += base::Utils::align(ACTIVITY, max_size) +
            "in use\n";
    }
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
InfoCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector result;

    if (setparams.empty())
    {
        result.push_back(cli::ALL);
        StringList list = getDevNames();
        std::copy(list.begin(), list.end(),
                  std::back_inserter(result));
    }

    return result;
}

