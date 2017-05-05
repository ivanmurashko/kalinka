/**
   @file ieee1394.cpp
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
   - 2009/03/23 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

// linux1394 includes
#include <libraw1394/raw1394.h>
#include <libavc1394/rom1394.h>
#include <libavc1394/avc1394.h>


#include <boost/bind.hpp>

#include "ieee1394.h"
#include "exception.h"
#include "defines.h"
#include "messages.h"
#include "db.h"

using namespace klk;
using namespace klk::fw;


/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new FireWire(factory));
}

// Constructor
FireWire::FireWire(IFactory *factory) :
    ModuleWithInfo<DVInfo>(factory, MODID,
                           msg::id::IEEE1394START,
                           msg::id::IEEE1394STOP),
    m_update(boost::bind(&FireWire::getDVInfoSet))
{
}

// Destructor
FireWire::~FireWire()
{
}

// Gets a human readable module name
const std::string FireWire::getName() const throw()
{
    return MODNAME;
}

// Do the start
void FireWire::doStart(const IMessagePtr& in, const IMessagePtr& out)
{
    BOOST_ASSERT(out);
    const mod::InfoPtr info = getModuleInfo(in);
    if (info->isInUse())
    {
        // already tuned
        throw Exception(__FILE__, __LINE__,
                        "Failed to start usage the module info with "
                        "name '%s'", info->getName().c_str());
    }
    info->setInUse(true);
    info->fillOutMessage(out);
}

// Retrives an list with data from @ref grDB "database"
const FireWire::InfoSet FireWire::getInfoFromDB()
{
    // getting data from db
    InfoList db_list = getInfoFromDBWithoutMod();

    // getting the current states
    InfoSet current = m_update();

    InfoSet set;

    // delete list creation and delete all not connected devices
    // from the DB list
    for (InfoList::iterator i = db_list.begin();
         i != db_list.end(); i++)
    {
        InfoSet::iterator find = current.find(*i);
        if (find != current.end())
        {
            set.insert(*i);
        }

    }

    return set;
}

// Register all processors
void FireWire::registerProcessors()
{
    ModuleWithInfo<DVInfo>::registerProcessors();

    registerTimer(boost::bind(&FireWire::checkCameras, this),
                  CHECKINTERVAL);
}

// Checks cameras
void FireWire::checkCameras()
{
    // retrive the current cameras states
    InfoSet current = m_update();

    // retrive del set
    InfoSet del_set = m_info.getDel(current);
    // sends event about missing
    IMessageFactory *msgfactory = getFactory()->getMessageFactory();
    for (InfoSet::iterator i = del_set.begin();
         i != del_set.end(); i++)
    {
        IMessagePtr msg = msgfactory->getMessage(msg::id::IEEE1394DEV);
        msg->setData(msg::key::MODINFOUUID, (*i)->getUUID());
        msg->setData(msg::key::IEEE1394STATE, msg::key::IEEE1394STATEDEL);
        getFactory()->getModuleFactory()->sendMessage(msg);
    }
    // retrive new set
    InfoSet new_set = m_info.getAdd(current);
    // sends info about new
    for (InfoSet::iterator i = new_set.begin();
         i != new_set.end(); i++)
    {
        DVInfoPtr info =
            mod::Info::dynamicPointerCast<DVInfo>(*i);
        IMessagePtr msg = msgfactory->getMessage(msg::id::IEEE1394DEV);
        msg->setData(msg::key::MODINFOUUID, info->getUUID());
        msg->setData(msg::key::IEEE1394STATE, msg::key::IEEE1394STATENEW);
        msg->setData(msg::key::IEEE1394PORT, info->getPort());
        getFactory()->getModuleFactory()->sendMessage(msg);

        // add the info about the new element to the db
        addInfo2DB(info);
    }


    // delete missing
    m_info.del(del_set);

    // add new
    m_info.add(new_set, false);
}

// Adds an info about new camera to the DB
void FireWire::addInfo2DB(const DVInfoPtr& info)
{
    BOOST_ASSERT(info);

    // check that the info is not at the db
    InfoList db_list = getInfoFromDBWithoutMod();
    InfoList::iterator
        find = std::find_if(db_list.begin(), db_list.end(),
                            boost::bind(mod::FindInfoByUUID<DVInfo>(),
                                        _1, info->getUUID()));
    if (find != db_list.end())
    {
        // update info
        info->updateInfo(*find);
        return;
    }

    db::DB db(getFactory());
    db.connect();
    db::Parameters params;
    // `klk_ieee1394_add` (
    // IN uuid VARCHAR(40),
    // IN name VARCHAR(40),
    // IN description VARCHAR(255),
    // OUT return_value INT
    params.add("@uuid", info->getUUID());
    params.add("@name", info->getName());
    params.add("@description", info->getDescription());
    params.add("@return_value");

    db::Result res = db.callSimple("klk_ieee1394_add", params);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Failed to add a new DV camera info to the DB");
    }
}

// Retrives info from db without any modifications
const FireWire::InfoList FireWire::getInfoFromDBWithoutMod()
{
    InfoList db_list;
    db::DB db(getFactory());
    db.connect();
    db::Parameters params;
    db::ResultVector rv = db.callSelect("klk_ieee1394_list", params, NULL);
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        db::Result res = *i;

        const std::string uuid = res["uuid"].toString();
        const std::string name = res["name"].toString();
        const std::string description = res["description"].toString();


        DVInfoPtr info(new DVInfo(uuid, name, description));
        db_list.push_back(info);
    }

    return db_list;
}

// Retrives info about current available cameras
const FireWire::InfoSet FireWire::getDVInfoSet()
{
    raw1394handle_t handle = raw1394_new_handle();
    if (!handle)
    {
        throw Exception(__FILE__, __LINE__,
                        "raw1394 - couldn't get handle. "
                        "The module not loaded");
    }

    InfoSet set;
    try
    {
        struct raw1394_portinfo pinf[16];
        int numcards = raw1394_get_port_info(handle, pinf, 16);
        if (numcards < 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "raw1394_get_port_info() failed");
        }

        for (int port = 0; port < numcards; port++)
        {
            if (raw1394_set_port(handle, port) < 0)
            {
                throw Exception(__FILE__, __LINE__,
                                "Couldn't set port");
            }
            for (int device = 0; device < raw1394_get_nodecount(handle);
                 device++ )
            {
                rom1394_directory rom1394_dir;
                if (rom1394_get_directory(handle, device, &rom1394_dir) < 0 )
                {
                    klk_log(KLKLOG_ERROR,  "rom1394_get_directory() failed for port:node "
                            "(%d:%d)",
                            port, device);
                    continue;
                }

                try
                {
                    rom1394_node_types type =
                        rom1394_get_node_type(&rom1394_dir);
                    if ((type == ROM1394_NODE_TYPE_AVC) &&
                        avc1394_check_subunit_type(handle, port,
                                                   AVC1394_SUBUNIT_TYPE_VCR))
                    {
                        octlet_t guid = rom1394_get_guid(handle, device);
                        char guid_str[65];
                        snprintf(guid_str, sizeof(guid_str) - 1, "%08x%08x",
                                 static_cast<quadlet_t>(guid >> 32),
                                 static_cast<quadlet_t>(guid & 0xffffffff));
                        // we have found a DV camera
                        std::string description = "DV camera";
                        if (rom1394_dir.label != NULL)
                        {
                            description = rom1394_dir.label;
                        }
                        description += std::string(". UUID: ") + guid_str;

                        const std::string name = guid_str;
                        DVInfoPtr info(new DVInfo(guid_str, name, description));
                        info->setPort(port);
                        set.insert(info);

                        klk_log(KLKLOG_DEBUG, "Detected a DV camera: %s",
                                description.c_str());
                    }
                }
                catch(...)
                {
                    rom1394_free_directory(&rom1394_dir);
                    throw;
                }
                rom1394_free_directory(&rom1394_dir);
            }
        }
    }
    catch(...)
    {
        raw1394_destroy_handle(handle);
        throw;
    }

    raw1394_destroy_handle(handle);

    return set;
}

