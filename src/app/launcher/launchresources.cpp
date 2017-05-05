/**
   @file launchresources.cpp
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
   - 2010/09/18 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "launchresources.h"
#include "exception.h"
#include "dev.h"
#include "launchdev.h"


using namespace klk;
using namespace klk::launcher;

//
// Resources class
//

/// Constructor
Resources::Resources(IFactory* factory) : m_factory(factory),
                                          m_proto(factory)
{
}

/// Destructor
Resources::~Resources()
{
}

/// @copydoc klk::IResources::getType()
const std::string Resources::getType() const throw()
{
    return klk::dev::BASE;
}

/// @copydoc klk::IResources::hasDev()
bool Resources::hasDev(const std::string& type) const
{
    return m_proto.hasDev(type);
}

/// @copydoc klk::IResources::getResourceByUUID()
const IDevPtr Resources::getResourceByUUID(const std::string& uuid) const
{
    return IDevPtr(new Dev(m_factory, uuid));
}

/// @copydoc klk::IResources::getResourceByName()
const IDevPtr Resources::getResourceByName(const std::string& name) const
{
    const std::string uuid = m_proto.getResourceByName(name);
    return IDevPtr(new Dev(m_factory, uuid));
}

/// @copydoc klk::IResources::getResourceByType()
const IDevList  Resources::getResourceByType(const std::string& type) const
{
    IDevList list;
    StringList ids =   m_proto.getResourceByType(type);
    for (StringList::iterator id = ids.begin(); id != ids.end(); id++)
    {
        list.push_back(IDevPtr(new Dev(m_factory, *id)));
    }

    return list;
}

// @copydoc klk::IResources::addDev()
// @note the method should not be called. The resources can be added only in the
// main application (mediaserver)
void Resources::addDev(const IDevPtr& dev)
{
    NOTIMPLEMENTED;
}

/// @copydoc klk::IResources::updateDB()
void Resources::updateDB(const std::string& type)
{
    NOTIMPLEMENTED;
}

// @copydoc klk::IResources::add()
// @note the method should not be called. The resources can be added only in the
// main application (mediaserver)
void Resources::add(const IResourcesPtr& resources)
{
    NOTIMPLEMENTED;
}

/// @copydoc klk::IResources::initDevs()
void Resources::initDevs()
{
    NOTIMPLEMENTED;
}
