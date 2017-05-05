/**
   @file launchmodule.cpp
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
   - 2010/09/30 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <boost/bind.hpp>

#include "launchmodule.h"
#include "service/messages.h"
#include "service/defines.h"
#include "utils.h"
#include "log.h"


using namespace klk::launcher;

//
// Module class
//

/// @copydoc klk::app::Module::Module()
Module::Module(klk::IFactory* factory,
               const std::string& modid,
               const std::string& setmsgid,
               const std::string& showmsgid) :
    klk::app::Module(factory, modid, setmsgid, showmsgid),
    klk::launcher::Base(factory),
    m_proto(NULL), m_proto_lock()
{
}

/// Destructor
Module::~Module()
{
    KLKDELETE(m_proto);
}

/// @copydoc klk::IModule::registerProcessors()
void Module::registerProcessors()
{
    klk::app::Module::registerProcessors();

    if (isLocal(srv::MODID) == false)
    {
        // add some randomize to the sending the cpu usage info
        const std::string seed_str = getID();
        unsigned int seed =  *(reinterpret_cast<const unsigned int*>(seed_str.c_str()));
        srand(seed);

        const time_t UPDATEINTERVAL = 30 + (rand() % 30);

        // periodically send cpu usage info
        registerTimer(boost::bind(&Module::sendCPUUsageInfo, this),
                      UPDATEINTERVAL);
    }
}



// @copydoc klk::IModule::sendSyncMessage
void Module::sendSyncMessage(const klk::IMessagePtr& in,
                             klk::IMessagePtr& out)
{
    // get receiver
    const StringList list = in->getReceiverList();
    BOOST_ASSERT(list.size() == 1);
    const std::string receiver_id = *(list.begin());

    if (isLocal(receiver_id))
    {
        // use local call
        klk::app::Module::sendSyncMessage(in, out);
    }
    else
    {
        // use RPC call
        out = getProtocol()->sendSync(in);
    }
}

/// @return the protocol instance
klk::adapter::MessagesProtocol* Module::getProtocol()
{
    klk::Locker lock(&m_proto_lock);
    if (m_proto == NULL)
    {
        m_proto = new klk::adapter::MessagesProtocol(getFactory());
    }
    return m_proto;
}

/// sends CPU usage info to main service module
void Module::sendCPUUsageInfo()
{
    KLKASSERT(isLocal(srv::MODID) == false);

    IMessagePtr msg =
        getFactory()->getMessageFactory()->getMessage(klk::msg::id::SRVMODINFO);

    msg->setData(klk::msg::key::SRVMODID, getID());
    msg->setData(klk::msg::key::SRVMODCPUUSAGE,
                  klk::base::Utils::percent2Str(getCPUUsage()));


    getProtocol()->sendASync(msg);
}
