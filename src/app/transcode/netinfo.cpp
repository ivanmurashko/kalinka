/**
   @file netinfo.cpp
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
   - 2009/10/30 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>

#include "netinfo.h"
#include "exception.h"
#include "trans.h"

#include "network/defines.h"
#include "network/messages.h"

using namespace klk;
using namespace klk::trans;

//
// NetInfo class
//

// Constructor
NetInfo::NetInfo(const std::string& uuid, const std::string& name,
                 const std::string& media_type) :
    SourceInfo(uuid, name, media_type), m_module(NULL)
{
}

// Locks resource if it's necessry
const IMessagePtr NetInfo::lockResource(Transcode* module)
{
    // FIXME!!! same code at klk::http::Streamer::getRouteInfoByUUID
    BOOST_ASSERT(module);
    IFactory* factory = module->getFactory();
    IMessageFactory* msgfactory = factory->getMessageFactory();
    IMessagePtr req = msgfactory->getMessage(msg::id::NETINFO);
    BOOST_ASSERT(req);
    req->setData(msg::key::MODINFOUUID, getUUID());
    IMessagePtr res;
    module->sendSyncMessage(req, res);
    BOOST_ASSERT(res);
    if (res->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // net module reject it
        throw Exception(__FILE__, __LINE__,
                        "Net module rejected route. Name: %s, UUID: '%s'",
                        getName().c_str(),
                        getUUID().c_str());
    }

    sock::Protocol proto = sock::TCPIP;
    if (res->getValue(msg::key::NETPROTO) == net::UDP)
    {
        proto = sock::UDP;
    }

    // should we lock the resource
    // we lock only udpsink
    if (getDirection() == SINK && proto == sock::UDP)
    {
        // FIXME!!! place it at separate method
        // we should lock it (TCP input should be unique - it's a server)
        IMessagePtr req_start = msgfactory->getMessage(msg::id::NETSTART);
        BOOST_ASSERT(req_start);
        req_start->setData(msg::key::MODINFOUUID, getUUID());
        IMessagePtr res_start;
        module->sendSyncMessage(req_start, res_start);
        BOOST_ASSERT(res_start);
        if (res_start->getValue(msg::key::STATUS) != msg::key::OK)
        {
            // net module reject it
            throw Exception(__FILE__, __LINE__,
                            "Net module rejected route. Name: %s, UUID: '%s'",
                            getName().c_str(),
                            getUUID().c_str());
        }
        // need unlock
        m_module = module;
    }

    return res;
}


// Inits the network source element assosiated with the source
void NetInfo::initInternal(Transcode* module)
{
    IMessagePtr res = lockResource(module);

    // do initialization

    sock::Protocol proto = sock::TCPIP;
    if (res->getValue(msg::key::NETPROTO) == net::UDP)
    {
        proto = sock::UDP;
    }

    std::string element_name;
    switch(getDirection())
    {
    case SOURCE:
    {
        if (proto == sock::TCPIP)
        {
            element_name = "tcpclientsrc";
        }
        else
        {
            element_name = "udpsrc";
        }
        break;
    }
    case SINK:
    {
        if (proto == sock::TCPIP)
        {
            element_name = "tcpclientsink";
        }
        else
        {
            element_name = "udpsink";
        }
        break;
    }
    default:
        throw Exception(__FILE__, __LINE__,
                        "Unsupported direction for net info");
        break;
    }

    initElement(element_name, module->getFactory());

    const std::string host = res->getValue(msg::key::NETHOST);
    u_int port = 0;
    try
    {
        port = boost::lexical_cast<u_int>(res->getValue(msg::key::NETPORT));
    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }

    // only udpsrc has uri format all others sets host, port values
    if (proto == sock::UDP && getDirection() == SOURCE)
    {
        // URI in the form of udp://multicast_group:port
        char buff[512];
        snprintf(buff, sizeof(buff) - 1, "udp://%s:%d", host.c_str(), port);
        g_object_set(G_OBJECT (m_element), "uri", buff, NULL);
    }
    else
    {
        g_object_set(G_OBJECT (m_element), "host", host.c_str(), NULL);
        g_object_set(G_OBJECT (m_element), "port", port, NULL);
    }

    if (  getMediaType() == klk::media::RTP_MPEGTS &&
          proto == sock::UDP &&
          getDirection() == SOURCE )
    {
        // gst-launch -v udpsrc port=22222 caps="application/x-rtp, clock-rate=(int)90000" !
        // gstrtpjitterbuffer latency=2000 ! rtpmp2tdepay !
        // tcpclientsink host=178.79.133.84 port=22222

        GstElement *bin = gst_bin_new (NULL);
        BOOST_ASSERT(bin);
        g_object_set (G_OBJECT (bin), "name", "rtp", NULL);

        GstCaps* caps = gst_caps_from_string(
            "application/x-rtp, clock-rate=(int)90000, encoding-name=(string)MP2T-ES, payload=(int)33" );
        BOOST_ASSERT(caps);
        GstElement* capsfilter    = gst_element_factory_make ( "capsfilter", "capsfilter" );
        BOOST_ASSERT(capsfilter);
        g_object_set( G_OBJECT ( capsfilter ),  "caps",  caps, NULL );

        GstElement *jitter = gst_element_factory_make ("gstrtpjitterbuffer", NULL);
        BOOST_ASSERT(jitter);
        GstElement *depay = gst_element_factory_make ("rtpmp2tdepay",  NULL);
        BOOST_ASSERT(depay);

        Locker lock(&m_lock);
        GstElement* src = m_element;
        gboolean bres;

        gst_bin_add_many (GST_BIN (bin), src, capsfilter, jitter, depay, NULL);
        bres = gst_element_link_many(src, capsfilter, jitter, depay, NULL);
        BOOST_ASSERT(bres);

        GstPad *srcpad = gst_element_get_pad (depay, "src");
        BOOST_ASSERT(srcpad);
        GstPad *ghost_src = gst_ghost_pad_new ("src", srcpad);
        BOOST_ASSERT(ghost_src);
        bres = gst_element_add_pad (bin, ghost_src);
        BOOST_ASSERT(bres == TRUE);
        gst_object_unref (srcpad);

        m_element = bin;
    }
}

// Deinits the network source element assosiated with the source
void NetInfo::deinit() throw()
{
    try
    {
        if (m_module.getValue())
        {
            IMessageFactory* msgfactory =
                m_module.getValue()->getFactory()->getMessageFactory();
            IMessagePtr req = msgfactory->getMessage(msg::id::NETSTOP);
            BOOST_ASSERT(req);
            req->setData(msg::key::MODINFOUUID, getUUID());
            IMessagePtr res;
            m_module.getValue()->sendSyncMessage(req, res);
            BOOST_ASSERT(res);
            if (res->getValue(msg::key::STATUS) != msg::key::OK)
            {
                // gst module rejected the channel
                throw Exception(__FILE__, __LINE__,
                                "Net module rejected route with name '%s'",
                                getName().c_str());
            }
        }
    }
    catch(...)
    {
        // ignore exception (we should not throw it)
        klk_log(KLKLOG_ERROR, "Error while netinfo deinitialization");
    }

    m_module = NULL;
    // base clearing
    SourceInfo::deinit();
}
