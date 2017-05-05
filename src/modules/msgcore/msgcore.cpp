/**
   @file msgcore.cpp
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
   - 2008/07/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "msgcore.h"
#include "common.h"
#include "log.h"
#include "defines.h"
#include "exception.h"

using namespace klk;
using namespace klk::msgcore;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new MessageCore(factory));
}

//
// MessageCore
//

// Constructor
// @param[in] factory the module factory
MessageCore::MessageCore(IFactory *factory) :
    Module(factory, MODID)
{
}

// Destructor
MessageCore::~MessageCore()
{
}

// Gets a human readable module name
const std::string MessageCore::getName() const throw()
{
    return MODNAME;
}

// Process a message
// @param[in] msg the message to be processed
void MessageCore::process(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg);
    BOOST_ASSERT(msg->getType() == msg::ASYNC);


    const StringList receivers = msg->getReceiverList();
    IModuleFactory *mod_factory = getFactory()->getModuleFactory();
    BOOST_ASSERT(mod_factory);
    for (StringList::const_iterator i = receivers.begin();
         i != receivers.end(); i++)
    {
        // check receiver
        if (i->c_str() == getID())
        {
            KLKASSERT(0);
        }

        IModulePtr module = mod_factory->getModule(i->c_str());
        if (module != NULL)
        {
            module->addMessage(msg);
        }
        else
        {
            std::string descr = "Sending message with id '" +
                msg->getID() + "' to module '" + *i +
                "' was failed: unkown module id";
            throw Exception(__FILE__, __LINE__, descr);
        }
    }
}

// Register all processors
void MessageCore::registerProcessors()
{
    registerASync(msg::id::ALL,
        boost::bind(&MessageCore::process, this, _1));
}
