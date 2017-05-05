/**
   @file httpinfo.cpp
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
   - 2009/12/06 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "httpinfo.h"
#include "trans.h"

// module specific
#include "httpsrc/messages.h"

using namespace klk;
using namespace klk::trans;

//
// HTTPSrcInfo class
//

// Constructor
HTTPSrcInfo::HTTPSrcInfo(const std::string& uuid,  const std::string& name,
                         const std::string& media_type) :
    SourceInfo(uuid, name, media_type)
{
}

// Inits the network source element assosiated with the source
void HTTPSrcInfo::initInternal(Transcode* module)
{
    BOOST_ASSERT(module);
    IFactory* factory = module->getFactory();
    IMessageFactory* msgfactory = factory->getMessageFactory();
    IMessagePtr req = msgfactory->getMessage(msg::id::HTTPSRCSTART);
    BOOST_ASSERT(req);
    req->setData(msg::key::MODINFOUUID, getUUID());
    IMessagePtr res;
    module->sendSyncMessage(req, res);
    BOOST_ASSERT(res);
    if (res->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // net module reject it
        throw Exception(__FILE__, __LINE__,
                        "HTTP source module rejected request. Name: %s, UUID: '%s'",
                        getName().c_str(),
                        getUUID().c_str());
    }

    const std::string addr = res->getValue(msg::key::HTTPSRC_ADDR);
    BOOST_ASSERT(!addr.empty());
    const std::string login = res->getValue(msg::key::HTTPSRC_LOGIN);
    const std::string pwd = res->getValue(msg::key::HTTPSRC_PWD);

    /*
      gst-launch -v souphttpsrc user-id=Login user-pw=Password location=http://nako-camera.no-ip.org:31080/video/mjpg.cgi?profileid=3 ! multipartdemux ! jpegdec name=d ffmux_flv name=mux ! tcpclientsink host=192.168.134.2 port=20001 d. ! queue ! ffmpegcolorspace ! "video/x-raw-yuv",format=\(fourcc\)I420 ! deinterlace ! videoscale ! "video/x-raw-yuv",width=320,height=240 ! ffenc_flv ! mux.
     */

    initElement("souphttpsrc", module->getFactory());
    g_object_set(G_OBJECT (m_element), "location", addr.c_str(), NULL);
    if (!login.empty())
    {
        g_object_set(G_OBJECT (m_element), "user-id", login.c_str(), NULL);
    }
    if (!pwd.empty())
    {
        g_object_set(G_OBJECT (m_element), "user-pw", pwd.c_str(), NULL);
    }
}
