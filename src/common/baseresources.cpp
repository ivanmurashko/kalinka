/**
   @file baseresources.cpp
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
   - 2008/09/05 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <boost/bind.hpp>
#include <boost/function/function1.hpp>

#include "baseresources.h"
#include "common.h"
#include "log.h"
#include "busdev.h"
#include "db.h"
#include "exception.h"

using namespace klk;
using namespace dev;

// Constructor
BaseResources::BaseResources(IFactory* factory,
                             const std::string& type) :
    m_lock(), m_factory(factory),
    m_type(type),
    m_devlist(),
    m_reslist()
{
    BOOST_ASSERT(m_factory);
}

// Destructor
BaseResources::~BaseResources()
{
}

// helper functor for look for a specified dev
struct IsDevHasParam
{
    bool operator()(const IDevPtr& dev,
                    const std::string& param,
                    const std::string& value)
    {
        if (dev->hasParam(dev::UUID) == false)
        {
            // ignore devs without UUID
            // not have an entry at the DB
            return false;
        }

        if (dev->hasParam(param) == false)
        {
            // we don't know anything about the requested param
            return false;
        }

        return (dev->getStringParam(param) == value);
    }
};

// Check is there the specified dev type
// @param[in] type - the dev's typ[e to be checked
bool BaseResources::hasDev(const std::string& type) const
{
    if (type == m_type)
    {
        // whole list indicates that type match
        return true;
    }

    IDevList devlist = getDevList();

    IDevList::const_iterator i = std::find_if(
        devlist.begin(), devlist.end(),
        boost::bind<bool>(IsDevHasParam(), _1, dev::TYPE, type));

    if (i != devlist.end())
        return true; // found in dev list

    boost::function1<bool, IResourcesPtr>
        compare = boost::bind(&IResources::hasDev, _1, type);
    // not found, try sub resources
    IResourcesList reslist = getResourcesList();
    IResourcesList::const_iterator j = std::find_if(
        reslist.begin(), reslist.end(),
        boost::bind<bool>(compare, _1));
    if (j != reslist.end())
        return true; // found in sub resources

    return false;
}

// Gets the resource by its name
const IDevPtr BaseResources::getResourceByName(const std::string& name)
    const
{
    // FIXME!!! can be slow -change the algorithm
    // get everything
    IDevList list = getResourceByType(m_type);
    // search in the list
    IDevList::iterator i = std::find_if(
        list.begin(), list.end(),
        boost::bind<bool>(IsDevHasParam(), _1, dev::NAME, name));

    if (i == list.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "Cannot find dev with name: " + name);
    }

    return *i;
}

// Gets the resource by its id
// @param[in] uuid - the resource_id
const IDevPtr BaseResources::getResourceByUUID(const std::string& uuid)
    const
{
    // FIXME!!! can be slow -change the algorithm
    // get everything
    IDevList list = getResourceByType(m_type);
    // serch in the result
    IDevList::iterator i = std::find_if(
        list.begin(), list.end(),
        boost::bind<bool>(IsDevHasParam(), _1, dev::UUID, uuid));

    if (i == list.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "Cannot find dev with UUID: " + uuid);
    }

    return *i;
}

struct IsDevNotAtDB
{
    bool operator()(const IDevPtr& dev)
    {
        if (dev->hasParam(dev::UUID) == false)
        {
            return true;
        }

        return false;
    }
};

// Gets a list of devices by it's type
const IDevList BaseResources::getResourceByType(const std::string& type) const
{
    IDevList result, devlist = getDevList();
    IsDevNotAtDB notdbcheck;
    if (type == m_type)
    {
        result = devlist;
        // remove devices that don't have UUID and BUSUUID set
        // this means that the devs are not in the DB
        result.remove_if(notdbcheck);
    }
    else
    {
        IsDevHasParam compare;
        for (IDevList::const_iterator i = devlist.begin();
             i != devlist.end(); i++)
        {
            if (compare(*i, dev::TYPE, type) == true &&
                notdbcheck(*i) == false)
            {
                result.push_back(*i);
            }
        }
    }

    // search in subresources
    IResourcesList reslist = getResourcesList();
    for (IResourcesList::const_iterator i = reslist.begin();
         i != reslist.end(); i++)
    {
        const std::string
            type_to_retrive = (type == m_type ? (*i)->getType() : type);
        IDevList add = (*i)->getResourceByType(type_to_retrive);
        result.insert(result.end(), add.begin(), add.end());
    }

    return result;
}

// Updates info at the DB
void BaseResources::updateDB(const std::string& type)
{
    StringList bus_uuids;
    IDevList devlist = getDevList();
    for (IDevList::iterator i = devlist.begin(); i != devlist.end(); i++)
    {
        if ((*i)->getStringParam(dev::TYPE) == type || type == type)
        {
            if ((*i)->hasParam(dev::UUID) == true)
            {
                // it's already in the db
                (*i)->update();
                klk_log(KLKLOG_DEBUG,
                        "Info for dev '%s' was updated at the DB",
                        (*i)->getStringParam(dev::NAME).c_str());
            }
            else
            {
                // we have to add the info to the DB
                (*i)->updateDB();
                klk_log(KLKLOG_DEBUG,
                        "Info for dev '%s' was stored at the DB",
                        (*i)->getStringParam(dev::NAME).c_str());
            }
        }

        if ((*i)->hasParam(dev::BUSUUID))
        {
            const std::string bus_uuid = (*i)->getStringParam(dev::BUSUUID);
            if (std::find(bus_uuids.begin(), bus_uuids.end(), bus_uuid) ==
                bus_uuids.end())
            {
                bus_uuids.push_back(bus_uuid);
            }
        }
    }

    std::for_each(bus_uuids.begin(),
                  bus_uuids.end(),
                  boost::bind(&BaseResources::removeMissing, this, _1, type));

    // process sub devs
    IResourcesList reslist = getResourcesList();
    for (IResourcesList::iterator i = reslist.begin();
         i != reslist.end(); i++)
    {
        const std::string
            type_to_retrive = (type == type ? (*i)->getType() : type);
        (*i)->updateDB(type_to_retrive);
    }
}

// Removes missing resources from the DB
void BaseResources::removeMissing(const std::string& bus_uuid,
                                  const std::string& type)
{
    // IN bus VARCHAR(40)
    db::Parameters params;
    db::DB db(m_factory);
    db.connect();
    params.add("@bus", bus_uuid);
    db::ResultVector rv = db.callSelect("klk_resource_list",
                                           params, NULL);
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        //SELECT
        //klk_resources.resource,
        //klk_resources.resource_type,
        //klk_resources.bandwidth,
        //klk_resources.enabled,
        //klk_resources.resource_name

        if (type != (*i)["resource_type"].toString())
            continue; // nothing to do
        const std::string uuid = (*i)["resource"].toString();
        try
        {
            getResourceByUUID(uuid);
        }
        catch(...)
        {
            // delete the resource
            // `klk_resource_delete` (
            // IN resource VARCHAR(40),
            // OUT return_value INTEGER
            params.clear();
            params.add("@resource", uuid);
            params.add("@return_value");
            db::Result result = db.callSimple("klk_resource_delete",
                                                 params);
            if (result["@return_value"].toInt() != 0)
            {
                klk_log(KLKLOG_ERROR,
                        "Failed to delete a resource with uuid '%s' "
                        "from the DB", uuid.c_str());
            }
        }
    }
}

/**
   Functor to find a thread by dev
*/
struct FindDev :
    public std::binary_function<IDevPtr, IDevPtr, bool>
{
    /**
       Finder itself
    */
    inline bool operator()(const IDevPtr dev1,
                           const IDevPtr dev2)
    {
        if (dev1->hasParam(NAME) && dev2->hasParam(NAME))
        {
            // name should be unique
            if (dev1->getStringParam(NAME) == dev2->getStringParam(NAME))
                return true;
        }

        if (dev1->hasParam(UUID) && dev2->hasParam(UUID))
        {
            // uuid should be unique
            if (dev1->getStringParam(UUID) == dev2->getStringParam(UUID))
                return true;
        }
        return dev1->equal_to(dev2);
    }
};

// Adds a dev to the list
// @param[in] dev - the dev to be added
void BaseResources::addDev(const IDevPtr& dev)
{
    Locker lock(&m_lock);
    BOOST_ASSERT(dev);

    IDevList::iterator i = std::find_if(
        m_devlist.begin(), m_devlist.end(),
        boost::bind(FindDev(), _1, dev));
    if (i == m_devlist.end())
    {
        dev->update();
        m_devlist.push_back(dev);
    }
    else
    {
        // device is already in
        klk_log(KLKLOG_ERROR, "Device with name '%s' has been already added",
                dev->getStringParam(NAME).c_str());
    }
}

// functor for finding resources
struct IsEqualBaseResources
{
    bool operator()(const IResourcesPtr& res1,
                    const IResourcesPtr& res2)
    {
        BOOST_ASSERT(res1);
        BOOST_ASSERT(res2);
        return (res1->getType() == res2->getType());
    }
};

// Adds sub resources
void BaseResources::add(const IResourcesPtr& resources)
{
    BOOST_ASSERT(resources);
    Locker lock(&m_lock);
    IResourcesList::iterator i =
        std::find_if(m_reslist.begin(), m_reslist.end(),
                     boost::bind<bool>(IsEqualBaseResources(),
                                       resources, _1));
    if (i != m_reslist.end() &&
        /* The resources can be already added*/resources != *i)
    {
        throw Exception(__FILE__, __LINE__,
                             "Resources with type '%s' "
                             "have been already added",
                             resources->getType().c_str());
    }

    m_reslist.push_back(resources);
}

// Retrives dev list
const IDevList BaseResources::getDevList() const
{
    Locker lock(&m_lock);
    return m_devlist;
}

// Retrives sub resources list
const IResourcesList BaseResources::getResourcesList() const
{
    Locker lock(&m_lock);
    return m_reslist;
}

// Clears internal info
void BaseResources::clear() throw()
{
    Locker lock(&m_lock);
    m_devlist.clear();
    m_reslist.clear();
}
