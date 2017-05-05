/**
   @file fwinfo.cpp
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
   - 2010/06/13 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fwinfo.h"
#include "trans.h"

#include "ieee1394/defines.h"
#include "ieee1394/messages.h"

using namespace klk;
using namespace klk::trans;

//
// FWSrcInfo class
//

// Constructor
FWSrcInfo::FWSrcInfo(const std::string& uuid,  const std::string& name,
                     const std::string& media_type) :
    SourceInfo(uuid, name, media_type), m_module(NULL)
{
}

// Inits IEEE1394 element assosiated with the source
// Typical usage is the following
// gst-launch dv1394src ! queue ! decodebin name=d ffmux_flv name=mux !
// tcpclientsink host=127.0.0.1 port=20000 d.  ! ffmpegcolorspace !
// "video/x-raw-yuv",format=\(fourcc\)I420 ! deinterlace ! videoscale !
// "video/x-raw-yuv",width=320,height=240 ! ffenc_flv bitrate=600000 ! mux.
// d. ! queue ! audioconvert ! audioresample ! audio/x-raw-int,rate=44100 !
// ffenc_libmp3lame bitrate=64000 ! mp3parse ! mux.
void FWSrcInfo::initInternal(Transcode* module)
{
    BOOST_ASSERT(module);

    IFactory* factory = module->getFactory();
    IMessageFactory* msgfactory = factory->getMessageFactory();
    IMessagePtr req = msgfactory->getMessage(msg::id::IEEE1394START);
    BOOST_ASSERT(req);
    req->setData(msg::key::MODINFOUUID, getUUID());
    IMessagePtr res;
    module->sendSyncMessage(req, res);
    BOOST_ASSERT(res);
    if (res->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // net module reject it
        throw Exception(__FILE__, __LINE__,
                        "IEEE1394 module rejected request. Name: %s, UUID: '%s'",
                        getName().c_str(),
                        getUUID().c_str());
    }

    // set the module instance for future cleanup
    m_module = module;

    /*
      port                : Port number (-1 automatic)
                        flags: readable, writable
                        Integer. Range: -1 - 16 Default: -1 Current: -1
     */
    int port = boost::lexical_cast<int>(res->getValue(msg::key::IEEE1394PORT));

    initElement("dv1394src", module->getFactory());
    g_object_set(G_OBJECT (m_element), "port", port, NULL);
}

// Deinits the IEEE1394 source element assosiated with the source
void FWSrcInfo::deinit() throw()
{
    try
    {
        if (m_module.getValue())
        {
            IMessageFactory* msgfactory =
                m_module.getValue()->getFactory()->getMessageFactory();
            IMessagePtr req = msgfactory->getMessage(msg::id::IEEE1394STOP);
            BOOST_ASSERT(req);
            req->setData(msg::key::MODINFOUUID, getUUID());
            IMessagePtr res;
            m_module.getValue()->sendSyncMessage(req, res);
            BOOST_ASSERT(res);
            if (res->getValue(msg::key::STATUS) != msg::key::OK)
            {
                // gst module rejected the channel
                throw Exception(__FILE__, __LINE__,
                                "IEEE1394 module rejected request for IEEE1394 stop with name '%s'",
                                getName().c_str());
            }
        }
    }
    catch(...)
    {
        // ignore exception (we should not throw it)
        klk_log(KLKLOG_ERROR, "Error while IEEE1394 info deinitialization");
    }

    m_module = NULL;
    // base cleanup
    SourceInfo::deinit();
}
