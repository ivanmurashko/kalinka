/**
   @file cliutils.cpp
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
   - 2009/05/03 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2011/01/08 CLI messages realted ids were moved to separate file climsg.h.in by ipp
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>

#include "cliutils.h"
#include "exception.h"
#include "utils.h"
#include "climsg.h"

using namespace klk::cli;

//
// Utils class
//

// Fills the message with parameters
void Utils::setProcessParams(const IMessagePtr& msg,
                             const ParameterVector& params)
{
    BOOST_ASSERT(msg);
    setParams(msg, params);
    msg->setData(msg::key::cli::TYPE, msg::key::cli::TYPEPROCESS);
}


// Fills the message with parameters
void Utils::setParams(const IMessagePtr& msg,
                      const ParameterVector& params)
{
    BOOST_ASSERT(msg);
    StringList list_params;
    std::copy(params.begin(), params.end(), std::back_inserter(list_params));
#ifdef DEBUG
    BOOST_ASSERT(list_params.size() == params.size());
#endif
    msg->setData(msg::key::cli::PARAMS, list_params);
}

// Retrives parameters from the message
const ParameterVector Utils::getProcessParams(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg);
    BOOST_ASSERT(msg->getValue(msg::key::cli::TYPE) ==
                    msg::key::cli::TYPEPROCESS);
    return getParams(msg);
}


// Retrives parameters from the message
const ParameterVector Utils::getParams(const IMessagePtr& msg)
{
    cli::ParameterVector params;

    StringList list_params = msg->getList(msg::key::cli::PARAMS);

    std::copy(list_params.begin(), list_params.end(),
              std::back_inserter(params));
#ifdef DEBUG
    BOOST_ASSERT(list_params.size() == params.size());
#endif

    return params;
}

// Retrives parameters vector from data string
const ParameterVector Utils::getCommands(const std::string& data)
{
    const ParameterVector commands = base::Utils::split(data, " \r\n");
    return commands;
}

// Sets completion info for sending
void Utils::setCompletionRequest(const IMessagePtr& msg,
                                 const ParameterVector& setparams)
{
    BOOST_ASSERT(msg);
    setParams(msg, setparams);
    msg->setData(msg::key::cli::TYPE, msg::key::cli::TYPECOMPLETE);
}

// Retrives completion position info from a message
const ParameterVector Utils::getCompletionRequest(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg);
    BOOST_ASSERT(msg->getValue(msg::key::cli::TYPE) ==
                    msg::key::cli::TYPECOMPLETE);
    return getParams(msg);
}

// Retrives completion info from a message
void Utils::setCompletionResponse(const IMessagePtr& msg,
                                  const ParameterVector& params)
{
    BOOST_ASSERT(msg);
    setParams(msg, params);
}

// Sets completion info to a message
const ParameterVector Utils::getCompletionResponse(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg);
    return getParams(msg);
}

// Retrives command mode from the message
const CommandMode Utils::getCommandMode(const IMessagePtr& msg)
{
    if (msg->getValue(msg::key::cli::TYPE) == msg::key::cli::TYPECOMPLETE)
        return GETCOMPLETION;
    if (msg->getValue(msg::key::cli::TYPE) == msg::key::cli::TYPEPROCESS)
        return PROCESS;

    return UNKNOWN;
}
