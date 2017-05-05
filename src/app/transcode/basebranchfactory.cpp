/**
   @file basebranchfactory.cpp
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

#include "basebranchfactory.h"
#include "exception.h"
#include "defines.h"

using namespace klk;
using namespace klk::trans;

//
// BaseBranchFactory class
//

// Makes a queue gst element
// static
GstElement* SimpleBaseBranchFactory::makeQueue()
{
    GstElement *queue = gst_element_factory_make ("queue", NULL);
    BOOST_ASSERT(queue);

    g_object_set (queue,
                  "max-size-time", static_cast<guint64>(300 * GST_SECOND),
                  "max-size-bytes", static_cast<guint64>(0),
                  "max-size-buffers", static_cast<guint64>(0), NULL);

    return queue;
}

// new-decoded-pad signal was recieved
// This signal gets emitted as soon as a new pad
// of the same type as one of the valid 'raw' types is added.
// static
void BaseBranchFactory::onNewDecodedPad(GstElement *decodebin,
                                        GstPad     *new_pad,   gboolean    last,
                                        gpointer    user_data) throw()
    try
    {
        BaseBranchFactory *holder =
            static_cast<BaseBranchFactory*>(user_data);
        BOOST_ASSERT(holder);

        // set pipeline in pause mode
        holder->pausePipeline();

        GstCaps* caps = gst_pad_get_caps (new_pad);
        BOOST_ASSERT(caps);
        gchar* str = gst_caps_to_string (caps);
        BOOST_ASSERT(str);

        klk_log(KLKLOG_DEBUG, "GST: new_decoded_pad_cb () caps: %s", str);
        /* video stream */
        if (g_str_has_prefix (str, "video/"))
        {
            holder->createVideoSink(new_pad);
        }
        /* audio stream */
        else if (g_str_has_prefix (str, "audio/"))
        {
            holder->createAudioSink(new_pad);
        }
        // set pipeline in playing mode
        holder->playPipeline();
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Error in new_decoded_pad_cb(): %s", err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Error in new_decoded_pad_cb():"
                " unknow exception");
    }

//
// BaseBranchFactory class
//

// Constructor
BaseBranchFactory::BaseBranchFactory(IFactory* const factory,
                                     IPipeline* const pipeline) :
    SimpleBaseBranchFactory(), m_factory(factory), m_pipeline(pipeline),
    m_branch(NULL), m_mux(NULL), m_vquality()
{
    BOOST_ASSERT(m_factory);
    BOOST_ASSERT(m_pipeline);
}

// Destructor
BaseBranchFactory::~BaseBranchFactory()
{
    // FIXME!!! add cleanup
    if (m_branch)
    {
        gst_element_set_state (GST_ELEMENT(m_branch), GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(m_branch));
    }
}

// Get video scale elements based on video quality settings
const BaseBranchFactory::GstElementVector
BaseBranchFactory::getVScale() const
{
    BaseBranchFactory::GstElementVector result;

    BOOST_ASSERT(m_vquality);
    const std::string size = m_vquality->getSize();
    if (size == quality::video::SIZE_DEFAULT)
        return result; // nothing to do

    result.reserve(2);

    GstElement* scale = gst_element_factory_make ("videoscale", NULL);
    result.push_back(scale);
    GstElement* filter = gst_element_factory_make ("capsfilter", NULL);
    result.push_back(filter);

    // FIXME!!! bad code
    if (size == quality::video::SIZE_320_240)
    {
        g_object_set (G_OBJECT(filter), "caps",
                      gst_caps_new_simple( "video/x-raw-yuv",
                                           "width", G_TYPE_INT, 320,
                                           "height", G_TYPE_INT, 240,NULL),
                      NULL );
    }
    else if (size == quality::video::SIZE_640_480)
    {
        g_object_set (G_OBJECT(filter), "caps",
                      gst_caps_new_simple( "video/x-raw-yuv",
                                           "width", G_TYPE_INT, 640,
                                           "height", G_TYPE_INT, 480,NULL),
                      NULL );
    }
    else if (size == quality::video::SIZE_720_576)
    {
        g_object_set (G_OBJECT(filter), "caps",
                      gst_caps_new_simple( "video/x-raw-yuv",
                                           "width", G_TYPE_INT, 720,
                                           "height", G_TYPE_INT, 576,NULL),
                      NULL );
    }
    else
    {
        throw Exception(__FILE__, __LINE__, "Unknown video scale size. UUID: " + size);
    }

    return result;
}

// Creates the simple branch
GstElement* BaseBranchFactory::createBranch(const TaskPtr& task)
{
    // fill video quality info
    BOOST_ASSERT(m_vquality == NULL); // only one time
    m_vquality = task->getVideoQuality();
    BOOST_ASSERT(m_vquality);

    BOOST_ASSERT(m_branch == NULL);
    BOOST_ASSERT(m_mux == NULL);
    m_branch = gst_bin_new (NULL);
    BOOST_ASSERT(m_branch);

    GstElement *decodebin = gst_element_factory_make("decodebin", "decodebin");
    BOOST_ASSERT(decodebin);
    GstElement *queue = makeQueue();
    BOOST_ASSERT(queue);

    gst_bin_add_many (GST_BIN (m_branch), queue, decodebin, NULL);

    // create ghost pad for future links
    GstPad *queue_pad = gst_element_get_pad (queue, "sink");
    BOOST_ASSERT(queue_pad);
    GstPad *branch_pad = gst_ghost_pad_new ("sink", queue_pad);
    BOOST_ASSERT(branch_pad);
    if (!gst_element_add_pad(m_branch, branch_pad))
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_add_pad() was failed");
    }
    gst_object_unref(queue_pad);


    g_signal_connect (decodebin, "new-decoded-pad",
                      G_CALLBACK (&BaseBranchFactory::onNewDecodedPad), this);


    // link elements together
    if (!gst_element_link_many (queue, decodebin, NULL))
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_link_many() was failed");
    }

    createMux(task->getDestinationElement());

    // the branch is used inside the class instance too
    // keep owner of the m_branch object here
    return GST_ELEMENT(gst_object_ref(GST_OBJECT(m_branch)));
}

// Creates video sink
void BaseBranchFactory::createVideoSink(GstPad *new_pad)
{
    GstElement         *vsinkbin;

    // Video specific muxer pads
    createMuxVideo();

    vsinkbin = createVideoBin();

    g_object_set (G_OBJECT (vsinkbin), "name", "videobin", NULL);
    BOOST_ASSERT(m_mux);

    gboolean bres = gst_bin_add (GST_BIN (m_branch), vsinkbin);
    BOOST_ASSERT(bres == TRUE);

    GstPad *ghost_sink = gst_element_get_pad(vsinkbin, "sink");
    BOOST_ASSERT(ghost_sink);
    if (gst_pad_link (new_pad, ghost_sink) != GST_PAD_LINK_OK)
    {
        throw Exception(__FILE__, __LINE__, "gst_pad_link() was failed");
    }

    GstPad *link_pad_src = gst_element_get_pad(vsinkbin, "src");
    BOOST_ASSERT(link_pad_src);
    GstPad *link_pad_sink = gst_element_get_pad(m_mux, "sink_video");
    BOOST_ASSERT(link_pad_sink);
    if (gst_pad_link (link_pad_src, link_pad_sink) != GST_PAD_LINK_OK)
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_pad_link() was failed");
    }
    gst_object_unref(link_pad_src);
    gst_object_unref(link_pad_sink);

    if (gst_element_set_state (GST_ELEMENT(vsinkbin), GST_STATE_PAUSED) ==
        GST_STATE_CHANGE_FAILURE)
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_set_state() was failed");
    }
}

// Creates audio sink
void BaseBranchFactory::createAudioSink(GstPad *new_pad)
{
    BOOST_ASSERT(new_pad);

    // Audio specific muxer pads
    createMuxAudio();

    /* audio sink */
    GstElement *asinkbin = createAudioBin();

    gboolean bres = gst_bin_add (GST_BIN (m_branch), asinkbin);
    BOOST_ASSERT(bres == TRUE);

    GstPad *ghost_sink = gst_element_get_pad(asinkbin, "sink");
    BOOST_ASSERT(ghost_sink);
    if (gst_pad_link (new_pad, ghost_sink) != GST_PAD_LINK_OK)
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_pad_link() was failed");
    }

    GstPad *link_pad_src = gst_element_get_pad(asinkbin, "src");
    BOOST_ASSERT(link_pad_src);
    GstPad *link_pad_sink = gst_element_get_pad(m_mux, "sink_audio");
    BOOST_ASSERT(link_pad_sink);
    if (gst_pad_link (link_pad_src, link_pad_sink) != GST_PAD_LINK_OK)
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_pad_link() was failed");
    }
    gst_object_unref(link_pad_src);
    gst_object_unref(link_pad_sink);
    if (gst_element_set_state (GST_ELEMENT(asinkbin), GST_STATE_PAUSED) ==
        GST_STATE_CHANGE_FAILURE)
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_set_state() was failed");
    }
}

// Sets pipeline into the playing mode
void BaseBranchFactory::playPipeline()
{
    BOOST_ASSERT(m_pipeline);
    m_pipeline->play();
}

// Sets pipeline into the paused mode
void BaseBranchFactory::pausePipeline()
{
    BOOST_ASSERT(m_pipeline);
    m_pipeline->pause();
}

// Creates muxer for  transcode
void BaseBranchFactory::createMux(GstElement* sink)
{
    BOOST_ASSERT(sink);

    BOOST_ASSERT(m_mux == NULL);

    m_mux = gst_bin_new (NULL);
    BOOST_ASSERT(m_mux);

    g_object_set (G_OBJECT (m_mux), "name", "muxer", NULL);

    GstElement* muxer = getMuxElement();
    gst_bin_add_many (GST_BIN (m_mux), muxer, sink, NULL);

    if (!gst_element_link_many (muxer, sink, NULL))
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_link_many() was failed");
    }

    BOOST_ASSERT(m_branch);
    if (!gst_bin_add(GST_BIN (m_branch), m_mux))
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_bin_add() was failed");
    }
}

// Creates muxer for FLV transcode
// It creates audio specific pad
void BaseBranchFactory::createMuxAudio()
{
    BOOST_ASSERT(m_mux);
    if (gst_element_set_state (GST_ELEMENT(m_mux), GST_STATE_PAUSED) ==
        GST_STATE_CHANGE_FAILURE)
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_set_state() was failed");
    }

    GstPad *lame_pad = getAudioMuxPad();
    BOOST_ASSERT(lame_pad);
    GstPad* a_mux_pad = gst_ghost_pad_new ("sink_audio", lame_pad);
    BOOST_ASSERT(a_mux_pad);
    gst_pad_set_active(a_mux_pad, TRUE);
    if (!gst_element_add_pad (m_mux, a_mux_pad))
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_add_pad() was failed");
    }
    gst_object_unref (lame_pad);
}

// Creates muxer for FLV transcode
// It creates video specific pad
void BaseBranchFactory::createMuxVideo()
{
    BOOST_ASSERT(m_mux);
    if (gst_element_set_state (GST_ELEMENT(m_mux), GST_STATE_PAUSED) ==
        GST_STATE_CHANGE_FAILURE)
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_set_state() was failed");
    }

    GstPad *flutsmux_pad = getVideoMuxPad();
    BOOST_ASSERT(flutsmux_pad);
    GstPad* v_mux_pad = gst_ghost_pad_new ("sink_video", flutsmux_pad);
    BOOST_ASSERT(v_mux_pad);
    gst_pad_set_active(v_mux_pad, TRUE);
    if (!gst_element_add_pad (m_mux, v_mux_pad))
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_add_pad() was failed");
    }
    gst_object_unref (flutsmux_pad);
}

// Creates video bin container (GstElement) to encode video stream
GstElement*  BaseBranchFactory::createVideoBin() const
{
    GstElement * vsinkbin = gst_bin_new (NULL);

    g_object_set (G_OBJECT (vsinkbin), "name", "videobin", NULL);

    GstElement* queue = makeQueue();
    BOOST_ASSERT(queue);
    GstElement* cspace = gst_element_factory_make ("ffmpegcolorspace", NULL);
    BOOST_ASSERT(cspace);

    GstElement* deinterlace = gst_element_factory_make ("ffdeinterlace", NULL);
    if ( !deinterlace)
    {
        klk_log(KLKLOG_DEBUG, "ffdeinterlace missing");
        deinterlace = gst_element_factory_make ("deinterlace", NULL);
    }
    BOOST_ASSERT(deinterlace);

    GstElementVector vscale = getVScale();
    BOOST_ASSERT(m_vquality);
    GstElement* encoder = createVideoEncoder(m_vquality->getQuality());
    BOOST_ASSERT(encoder);

    GstElement *identity = gst_element_factory_make ("identity", "identity");
    BOOST_ASSERT(identity);
    g_object_set (G_OBJECT (identity), "sync", TRUE, NULL);

    // different elements in the bin for default video scale and not
    // FIXME!!! same code at theorabranchfactory
    if (vscale.size() == 2)
    {
        BOOST_ASSERT(vscale[0]);
        BOOST_ASSERT(vscale[1]);
        gst_bin_add_many(GST_BIN (vsinkbin), queue, identity,
                         cspace, deinterlace,
                         vscale[0], vscale[1], encoder, NULL);
        if ( gst_element_link_many (queue, identity, cspace,
                                    vscale[0], vscale[1], deinterlace,
                                    encoder, NULL) != TRUE)
        {
            throw Exception(__FILE__, __LINE__,
                            "gst_element_link_many () failed");
        }
    }
    else
    {
        gst_bin_add_many(GST_BIN (vsinkbin), queue, identity,
                         cspace, deinterlace, encoder, NULL);
        if ( gst_element_link_many (queue, identity, cspace,
                                    deinterlace, encoder, NULL) != TRUE)
        {
            throw Exception(__FILE__, __LINE__,
                            "gst_element_link_many () failed");
        }
    }

    GstPad * videopad = gst_element_get_pad (queue, "sink");
    GstPad* ghost_sink = gst_ghost_pad_new ("sink", videopad);
    gst_object_unref (videopad);
    gboolean bres = gst_element_add_pad (vsinkbin, ghost_sink);
    BOOST_ASSERT(bres == TRUE);

    GstPad *encoder_pad = gst_element_get_pad (encoder, "src");
    GstPad* ghost_src = gst_ghost_pad_new ("src", encoder_pad);
    //gst_object_unref (encoder_pad);
    bres = gst_element_add_pad (vsinkbin, ghost_src);
    BOOST_ASSERT(bres == TRUE);

    return vsinkbin;
}

