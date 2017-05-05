/**
   @file flvbranchfactory.cpp
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
   - 2009/11/02 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "flvbranchfactory.h"
#include "exception.h"
#include "defines.h"

using namespace klk;
using namespace klk::trans;

//
// FLVBranchFactory class
//

// Constructor
FLVBranchFactory::FLVBranchFactory(IFactory* factory, IPipeline* pipeline) :
    BaseBranchFactory(factory, pipeline), m_flvmux(NULL)
{
}

// Destructor
FLVBranchFactory::~FLVBranchFactory()
{
    if (m_flvmux)
    {
        gst_object_unref (m_flvmux);
        m_flvmux = NULL;
    }
}

// Creates video encoder element
// virtual
GstElement* FLVBranchFactory::createVideoEncoder(const std::string& quality) const
{
    GstElement* encoder = gst_element_factory_make ("ffenc_flv", NULL);
    BOOST_ASSERT(encoder);

    // setting encoder quality FIXME!!! bad code
    if (quality == quality::video::LOW)
    {
        g_object_set (G_OBJECT (encoder), "bitrate", 100000, NULL);
    }
    else if (quality == quality::video::MEDIUM)
    {
        g_object_set (G_OBJECT (encoder), "bitrate", 300000, NULL);
    }
    else if (quality == quality::video::HIGH)
    {
        g_object_set (G_OBJECT (encoder), "bitrate", 900000, NULL);
    }

    return encoder;
}

// Creates audio bin container
GstElement* FLVBranchFactory::createAudioBin()
{
    /* audio sink */
    GstElement *asinkbin = gst_bin_new (NULL);
    BOOST_ASSERT(asinkbin);
    g_object_set (G_OBJECT (asinkbin), "name", "audiobin", NULL);

    /* audio part */
    GstElement *queue = makeQueue();
    BOOST_ASSERT(queue);
    GstElement *identity = gst_element_factory_make ("identity", "identity");
    BOOST_ASSERT(identity);
    g_object_set (G_OBJECT (identity), "sync", TRUE, NULL);
    GstElement *conv = gst_element_factory_make ("audioconvert", NULL);
    BOOST_ASSERT(conv);
    GstElement *resample = gst_element_factory_make ("audioresample", NULL);
    BOOST_ASSERT(resample);
    GstCaps *filter_resample = gst_caps_new_simple("audio/x-raw-int",
                                                   "rate", G_TYPE_INT, 44100,
                                                   NULL);
    BOOST_ASSERT(filter_resample);
    GstElement *cfilt_resample = gst_element_factory_make("capsfilter",
                                                          "cfilt_resample");
    BOOST_ASSERT(cfilt_resample);
    g_object_set(G_OBJECT(cfilt_resample), "caps", filter_resample, NULL);
    GstElement *encoder = gst_element_factory_make ("lame", NULL);
    if (encoder == NULL)
    {
        encoder = gst_element_factory_make ("ffenc_libmp3lame", NULL);
    }
    if (encoder == NULL)
    {
        throw Exception(__FILE__, __LINE__, "Cannot find a GST plugin for "
                        "mp3 encoder");
    }

    BOOST_ASSERT(encoder);
    GstElement *parser = gst_element_factory_make("mp3parse", NULL);
    BOOST_ASSERT(parser);

    gst_bin_add_many (GST_BIN (asinkbin), queue, identity, conv,
                      resample, cfilt_resample, encoder,
                      parser, NULL);
    gboolean bres =
        gst_element_link_many (queue, identity, conv,
                               resample, cfilt_resample, encoder, parser, NULL);
    BOOST_ASSERT(bres == TRUE);

    GstPad *audiopad = gst_element_get_pad (queue, "sink");
    BOOST_ASSERT(audiopad);
    GstPad *ghost_sink = gst_ghost_pad_new ("sink", audiopad);
    gst_element_add_pad (asinkbin, ghost_sink);
    gst_object_unref (audiopad);

    GstPad *encoder_pad = gst_element_get_pad (parser, "src");
    GstPad *ghost_src = gst_ghost_pad_new ("src", encoder_pad);
    //gst_object_unref (encoder_pad);
    bres = gst_element_add_pad (asinkbin, ghost_src);
    BOOST_ASSERT(bres == TRUE);

    return asinkbin;
}

// @copydoc klk::trans::BaseBranchFactory::getMuxElement
GstElement* FLVBranchFactory::getMuxElement()
{
    if (m_flvmux == NULL)
    {
        BOOST_ASSERT(m_flvmux == NULL);
        m_flvmux = gst_element_factory_make ("ffmux_flv", NULL);
        BOOST_ASSERT(m_flvmux);
        gst_object_ref (m_flvmux);
    }
    return m_flvmux;
}

//  @copydoc klk::trans::BaseBranchFactory::getAudioMuxPad
GstPad *  FLVBranchFactory::getAudioMuxPad()
{
    BOOST_ASSERT(m_flvmux);
    GstPad *lame_pad = gst_element_get_request_pad (m_flvmux, "audio_%d");
    BOOST_ASSERT(lame_pad);
    return lame_pad;
}

// @copydoc klk::trans::BaseBranchFactory::getVideoMuxPad
GstPad * FLVBranchFactory::getVideoMuxPad()
{
    BOOST_ASSERT(m_flvmux);
    GstPad *flutsmux_pad = gst_element_get_request_pad (m_flvmux, "video_%d");
    BOOST_ASSERT(flutsmux_pad);
    return flutsmux_pad;
}
