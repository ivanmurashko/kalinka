/**
   @file modulesprotocol.cpp
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
   - 2010/09/26 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "modulesprotocol.h"
#include "defines.h"
#include "exception.h"

using namespace klk::adapter;

//
// ModulesProtocol class
//

// Constructor
ModulesProtocol::ModulesProtocol(IFactory* factory) :
    Protocol<ipc::IModulesProxyPrx>(factory, obj::MODULES,
                                    klk::MODULE_COMMON_ID)
{
}

// Loads a module
void ModulesProtocol::load(const std::string& id)
{
    m_proxy->load(id);
}

// Unloads a module
void ModulesProtocol::unload(const std::string& id)
{
    m_proxy->unload(id);
}


// Checks is the module loaded or not
bool ModulesProtocol::isLoaded(const std::string& id)
{
    return m_proxy->isLoaded(id);
}
