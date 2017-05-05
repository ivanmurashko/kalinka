/**
   @file basecmd.cpp
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
   - 2009/05/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "basecmd.h"
#include "streamer.h"
#include "utils.h"
#include "exception.h"

// modules specific info
#include "network/messages.h"

using namespace klk;
using namespace klk::http;

//
// AddCommand class
//

// Constructor
AddCommand::AddCommand(const std::string& name,
                       const std::string& summary,
                       const std::string& usage) :
    cli::Command(name, summary, usage)
{
}

// Retrives not assigned route list
const StringList AddCommand::getRouteList()
{
    IMessageFactory* msgfactory = getFactory()->getMessageFactory();
    IMessagePtr req = msgfactory->getMessage(msg::id::NETLIST);
    BOOST_ASSERT(req);
    IMessagePtr res;
    getModule<Streamer>()->sendSyncMessage(req, res);
    BOOST_ASSERT(res);
    if (res->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // gst module rejected the channel
        throw Exception(__FILE__, __LINE__,
                        "Cannot get a response from network module");
    }

    return res->getList(msg::key::RESULT);
}

// Checks route name
void AddCommand::checkRouteName(const std::string& name)
{
    const StringList names = getRouteList();
    if (names.empty())
    {
        throw Exception(__FILE__, __LINE__,
                        "You cannot assign any routes, please "
                        "add them with net route add command");
    }
    if (std::find(names.begin(), names.end(), name) == names.end())
    {
        const std::string names_str = base::Utils::convert2String(names, ", ");
        throw Exception(__FILE__, __LINE__,
                        "Route name '%s' cannot be assigned. "
                        "Possible values: %s", name.c_str(),
                        names_str.c_str());
    }
}
