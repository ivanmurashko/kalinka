/**
   @file theorabranchfactory.cpp
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
   - 2010/03/05 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "theorabranchfactory.h"
#include "exception.h"
#include "defines.h"

using namespace klk;
using namespace klk::trans;

//
// TheoraBranchFactory class
//

// Constructor
TheoraBranchFactory::TheoraBranchFactory(IFactory* factory, IPipeline* pipeline) :
    BaseBranchFactory(factory, pipeline), m_theoramux(NULL)
{
}

// Destructor
TheoraBranchFactory::~TheoraBranchFactory()
{
    if (m_theoramux)
    {
        gst_object_unref (m_theoramux);
        m_theoramux = NULL;
    }
}

// Creates video encoder
GstElement* TheoraBranchFactory::createVideoEncoder(const std::string& quality) const
{
    GstElement* encoder = gst_element_factory_make ("theoraenc", NULL);
    BOOST_ASSERT(encoder);
    /*
      quality             : Video quality
      flags: readable, writable
      Integer. Range: 0 - 63 Default: 48 Current: 48
    */
    // setting encoder quality FIXME!!! bad code
    if (quality == quality::video::LOW)
    {
        g_object_set (G_OBJECT (encoder), "quality", 10, NULL);
    }
    else if (quality == quality::video::MEDIUM)
    {
        g_object_set (G_OBJECT (encoder), "quality", 30, NULL);
    }
    else if (quality == quality::video::HIGH)
    {
        g_object_set (G_OBJECT (encoder), "quality", 63, NULL);
    }

    return encoder;
}

// Creates audio bin container
GstElement* TheoraBranchFactory::createAudioBin()
{
    /* audio sink */
    GstElement *asinkbin = gst_bin_new (NULL);
    BOOST_ASSERT(asinkbin);
    g_object_set (G_OBJECT (asinkbin), "name", "audiobin", NULL);

    /* audio part */
    GstElement *queue_sink = makeQueue();
    BOOST_ASSERT(queue_sink);
    GstElement *identity = gst_element_factory_make ("identity", "identity");
    BOOST_ASSERT(identity);
    g_object_set (G_OBJECT (identity), "sync", TRUE, NULL);
    GstElement *conv = gst_element_factory_make ("audioconvert", NULL);
    BOOST_ASSERT(conv);
    GstElement *encoder = gst_element_factory_make ("vorbisenc", NULL);
    BOOST_ASSERT(encoder);
    GstElement *queue_src = makeQueue();
    BOOST_ASSERT(queue_src);

    gst_bin_add_many (GST_BIN (asinkbin), queue_sink, /*identity,*/ conv,
                      encoder, queue_src, NULL);
    gboolean bres =
        gst_element_link_many (queue_sink, /*identity,*/ conv,
                               encoder, queue_src, NULL);
    BOOST_ASSERT(bres == TRUE);

    GstPad *audiopad = gst_element_get_pad (queue_sink, "sink");
    BOOST_ASSERT(audiopad);
    GstPad *ghost_sink = gst_ghost_pad_new ("sink", audiopad);
    gst_element_add_pad (asinkbin, ghost_sink);
    gst_object_unref (audiopad);

    GstPad *encoder_pad = gst_element_get_pad (queue_src, "src");
    GstPad *ghost_src = gst_ghost_pad_new ("src", encoder_pad);
    //gst_object_unref (encoder_pad);
    bres = gst_element_add_pad (asinkbin, ghost_src);
    BOOST_ASSERT(bres == TRUE);

    return asinkbin;
}

// @copydoc klk::trans::BaseBranchFactory::getMuxElement
GstElement* TheoraBranchFactory::getMuxElement()
{
    if (m_theoramux == NULL)
    {
        BOOST_ASSERT(m_theoramux == NULL);
        m_theoramux = gst_element_factory_make ("oggmux", NULL);
        gst_object_ref (m_theoramux);
    }
     return m_theoramux;
}

//  @copydoc klk::trans::BaseBranchFactory::getAudioMuxPad
GstPad *  TheoraBranchFactory::getAudioMuxPad()
{
    BOOST_ASSERT(m_theoramux);
    GstPad *lame_pad = gst_element_get_request_pad (m_theoramux, "sink_%d");
    BOOST_ASSERT(lame_pad);
    return lame_pad;
}

// @copydoc klk::trans::BaseBranchFactory::getVideoMuxPad
GstPad * TheoraBranchFactory::getVideoMuxPad()
{
    BOOST_ASSERT(m_theoramux);
    GstPad *flutsmux_pad = gst_element_get_request_pad (m_theoramux, "sink_%d");
    BOOST_ASSERT(flutsmux_pad);
    return flutsmux_pad;
}
