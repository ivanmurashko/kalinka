/**
   @file launchmodfactory.cpp
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
   - 2010/09/25 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "launchmodfactory.h"

using namespace klk::launcher;

//
// ModuleFactory
//

/// Constructor
ModuleFactory::ModuleFactory(klk::IFactory* factory) :
    klk::ModuleFactory(factory), klk::launcher::Base(factory),
    m_proto(factory), m_msgproto(factory)
{
}

/// @copydoc klk::IModuleFactory::load
void ModuleFactory::load(const std::string& id)
{
    if (isLocal(id))
    {
        klk::ModuleFactory::load(id);
    }
    else
    {
        m_proto.load(id);
    }
}

/// @copydoc klk::IModuleFactory::unload
void ModuleFactory::unload(const std::string& id)
{
    if (isLocal(id))
    {
        klk::ModuleFactory::unload(id);
    }
    else
    {
        m_proto.unload(id);
    }
}

/// @copydoc klk::IModuleFactory::isLoaded
bool ModuleFactory::isLoaded(const std::string& id)
{
    if (isLocal(id))
    {
        return klk::ModuleFactory::isLoaded(id);
    }

    return m_proto.isLoaded(id);
}


/// @copydoc klk::IModuleFactory::sendMessage
void ModuleFactory::sendMessage(const IMessagePtr& msg)
{
    StringList receivers = msg->getReceiverList();
    bool local_receivers = false, remote_receivers = false;
    for (StringList::iterator item = receivers.begin();
         item != receivers.end(); item++)
    {
        if (isLocal(*item))
        {
            local_receivers = true;
        }
        else
        {
            remote_receivers = true;
        }
    }

    // process local
    if (local_receivers)
    {
        klk::ModuleFactory::sendMessage(msg);
    }
    if (remote_receivers)
    {
        m_msgproto.sendASync(msg);
    }

}
