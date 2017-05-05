/**
   @file fileinfo.cpp
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
   - 2009/08/17 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fileinfo.h"
#include "exception.h"
#include "db.h"
#include "trans.h"
#include "utils.h"

// modules specific
#include <file/messages.h>

using namespace klk;
using namespace klk::trans;

//
// FileInfo class
//

// Constructor
FileInfo::FileInfo(const std::string& uuid,
                   const std::string& name,  const std::string& media_type) :
    SourceInfo(uuid, name, media_type), m_module(NULL)
{
}

// Retrives path from db
const std::string FileInfo::getPath(Transcode* module)
{
    IFactory* factory = module->getFactory();
    IMessageFactory* msgfactory = factory->getMessageFactory();
    IMessagePtr req;
    if (getDirection() == SINK)
    {
        req = msgfactory->getMessage(msg::id::FILESTART);
        BOOST_ASSERT(req);
        req->setData(msg::key::FILEMODE, msg::key::FILEWRITE);
    }
    else
    {
        req = msgfactory->getMessage(msg::id::FILEINFO);
        req->setData(msg::key::FILEMODE, msg::key::FILEREAD);
        BOOST_ASSERT(req);
    }
    req->setData(msg::key::MODINFOUUID, getUUID());

    IMessagePtr res;
    module->sendSyncMessage(req, res);
    BOOST_ASSERT(res);
    if (res->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // net module reject it
        throw Exception(__FILE__, __LINE__,
                        "File module rejected request. Name: %s, UUID: '%s'",
                        getName().c_str(),
                        getUUID().c_str());
    }

    // set the module for future deinit
    if (getDirection() == SINK)
    {
        m_module = module;
        BOOST_ASSERT(m_module);
    }

    // return the path
    return res->getValue(msg::key::FILEPATH);
}


// Inits the file element
void FileInfo::initInternal(Transcode* module)
{
    BOOST_ASSERT(module);
    BOOST_ASSERT(module->getFactory());
    Locker lock(&m_lock);
    if (m_element)
    {
        throw Exception(__FILE__, __LINE__,
                        "The file element has been already initialized."
                        "UUID: %s;"
                        "Direction: %s",
                        getUUID().c_str(),
                        getDirection() == SOURCE ? "source": "destination");
    }
    std::string element_name;
    switch(getDirection())
    {
    case SOURCE:
        element_name = "filesrc";
        break;
    case SINK:
        element_name = "filesink";
        break;
    default:
        throw Exception(__FILE__, __LINE__,
                        "Unsupported direction for file info");
        break;
    }

    initElement(element_name, module->getFactory());

    const std::string path = getPath(module);
    // file should exist for source
    if (getDirection() == SOURCE && !base::Utils::fileExist(path))
    {
        throw Exception(__FILE__, __LINE__, "Source file does not exist: %s", path.c_str());
    }

    g_object_set(G_OBJECT (m_element), "location", path.c_str(), NULL);
}

// Deinits the network source element assosiated with the source
void FileInfo::deinit() throw()
{
    try
    {
        if (m_module.getValue())
        {
            IMessageFactory* msgfactory =
                m_module.getValue()->getFactory()->getMessageFactory();
            IMessagePtr req = msgfactory->getMessage(msg::id::FILESTOP);
            BOOST_ASSERT(req);
            req->setData(msg::key::MODINFOUUID, getUUID());
            IMessagePtr res;
            m_module.getValue()->sendSyncMessage(req, res);
            BOOST_ASSERT(res);
            if (res->getValue(msg::key::STATUS) != msg::key::OK)
            {
                // gst module rejected the channel
                throw Exception(__FILE__, __LINE__,
                                "Filemodule rejected request for file stop with name '%s'",
                                getName().c_str());
            }
        }
    }
    catch(...)
    {
        // ignore exception (we should not throw it)
        klk_log(KLKLOG_ERROR, "Error while file info deinitialization");
    }

    m_module = NULL;
    // base clearing
    SourceInfo::deinit();
}
