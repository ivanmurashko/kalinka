/**
   @file resourcesproxy.cpp
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
   - 2010/09/12 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iresources.h"
#include "resourcesproxy.h"

using namespace klk;
using namespace klk::adapter::ipc;

//
// ResourcesProxy class
//

// Constructor
ResourcesProxy::ResourcesProxy(IFactory* factory) : m_factory(factory)
{
    BOOST_ASSERT(m_factory);
}

// Check is there the specified dev type
bool ResourcesProxy::hasDev(const ::std::string& type,
                            const ::Ice::Current&)
{
    return m_factory->getResources()->hasDev(type);
}

// Gets the resource id by its name
std::string ResourcesProxy::getResourceByName(const std::string& name,
                                              const Ice::Current&)
{
    IDevPtr dev = m_factory->getResources()->getResourceByName(name);
    return dev->getStringParam(klk::dev::UUID);
}

// Gets a list of devices ids by it's type
std::vector<std::string>
ResourcesProxy::getResourceByType(const std::string& type, const Ice::Current&)
{
    std::vector<std::string> result;
    IDevList list = m_factory->getResources()->getResourceByType(type);
    for (IDevList::iterator dev = list.begin(); dev != list.end(); dev++)
    {
        result.push_back((*dev)->getStringParam(klk::dev::UUID));
    }

    return result;
}
