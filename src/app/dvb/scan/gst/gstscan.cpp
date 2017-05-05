/**
   @file gstscan.cpp
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
   - 2009/07/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include "gstscan.h"
#include "exception.h"
#include "commontraps.h"
#include "defines.h"

// modules specific
#include "dvb/defines.h"
#include "dvb/dvbdev.h"

using namespace klk;
using namespace klk::dvb;
using namespace klk::dvb::scan;

//
// GSTProcessor class
//

// Constructor
GSTProcessor::GSTProcessor(IFactory* factory) :
    gst::Thread(SCANINTERVAL),
    m_factory(factory),
    m_mpegtsparse(NULL),
    m_dumpSDT(boost::bind<void>(DefaultSDT(), _1, _2, _3, _4)),
    m_dumpPMT(boost::bind<void>(DefaultPMT(), _1, _2, _3))
{
}

// Destructor
GSTProcessor::~GSTProcessor()
{
}

// Starts processing
void GSTProcessor::start(const IDevPtr& dev)
{
    try
    {
        init(dev);
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Init failed for DVB scan");
        clean();
        throw;
    }

    gst::Thread::start();
}

// stops processing
void GSTProcessor::stopProcessing() throw()
{
    gst::Thread::stop();
}

// Creates demuxer
void GSTProcessor::init(const IDevPtr& dev)
{
    gst::Thread::init();

    BOOST_ASSERT(m_mpegtsparse == NULL);

    // necessary plugins
    const char* KLKMPEGTSPARSE = "klkmpegtsparse";
    const char* MPEGTSPARSE = "mpegtsparse";
    const char* DVBSRC = "dvbsrc";

    m_mpegtsparse = gst_element_factory_make (KLKMPEGTSPARSE, NULL);
    if (m_mpegtsparse == NULL)
    {
        klk_log(KLKLOG_ERROR, "%s GStreamer plugin missing", KLKMPEGTSPARSE);
        m_factory->getSNMP()->sendTrap(snmp::GSTPLUGINMISSING,
                                       KLKMPEGTSPARSE);
        m_mpegtsparse = gst_element_factory_make (MPEGTSPARSE, NULL);
        if (m_mpegtsparse == NULL)
        {
            m_factory->getSNMP()->sendTrap(snmp::GSTPLUGINMISSING,
                                           MPEGTSPARSE);
            throw Exception(__FILE__, __LINE__,
                            std::string(MPEGTSPARSE) +
                            " GStreamer plugin missing");
        }

        BOOST_ASSERT(m_mpegtsparse);
    }

    GstElement *dvbsrc = gst_element_factory_make (DVBSRC, NULL);
    if (dvbsrc == NULL)
    {
        m_factory->getSNMP()->sendTrap(snmp::GSTPLUGINMISSING, DVBSRC);
        throw Exception(__FILE__, __LINE__,
                        std::string(DVBSRC) +
                        " GStreamer plugin missing");
    }

    BOOST_ASSERT(dev);

    std::string type = dev->getStringParam(dev::TYPE);

    g_object_set (G_OBJECT (dvbsrc), "stats-reporting-interval",
                  5000, NULL);
    //g_object_set (G_OBJECT (dvbsrc), "do-timestamp",
    //TRUE, NULL);

    u_int adapter = dev->getIntParam(dev::ADAPTER);
    g_object_set (G_OBJECT (dvbsrc), "adapter", adapter, NULL);
    klk_log(KLKLOG_DEBUG, "Adapter: %d", adapter);

    u_int frontend = dev->getIntParam(dev::FRONTEND);
    g_object_set (G_OBJECT (dvbsrc), "frontend", frontend, NULL);
    klk_log(KLKLOG_DEBUG, "Frontend: %d", frontend);

    u_int frequency = dev->getIntParam(dev::FREQUENCY);
    g_object_set (G_OBJECT (dvbsrc), "frequency", frequency, NULL);
    klk_log(KLKLOG_DEBUG, "Frequency: %d", frequency);

    u_int code_rate_hp = dev->getIntParam(dev::CODE_RATE_HP);
    g_object_set (G_OBJECT (dvbsrc), "code-rate-hp", code_rate_hp, NULL);
    klk_log(KLKLOG_DEBUG, "Code rate hp: %d", code_rate_hp);

    if (type == dev::DVB_S)
    {
        std::string polarity = dev->getStringParam(dev::POLARITY);
        g_object_set (G_OBJECT (dvbsrc), "polarity",
                      polarity.c_str(), NULL);
        klk_log(KLKLOG_DEBUG, "Polarity: %s", polarity.c_str());

        u_int symbol_rate = dev->getIntParam(dev::SYMBOL_RATE);
        g_object_set (G_OBJECT (dvbsrc), "symbol-rate",
                      symbol_rate, NULL);
        klk_log(KLKLOG_DEBUG, "Symbol rate: %d", symbol_rate);

        int diseqc_src = dev->getIntParam(dev::DISEQC_SRC);
        BOOST_ASSERT(diseqc_src >= 0 && diseqc_src <= 4);
        // diseqc-source       :
        // DISEqC selected source (-1 disabled) (DVB-S)
        // flags: readable, writable
        // Integer. Range: -1 - 7 Default: -1 Current: -1

        // our values are 0 1 2 3 4 (getstream source compatible)
        g_object_set (G_OBJECT (dvbsrc), "diseqc-source",
                      diseqc_src - 1, NULL);
        klk_log(KLKLOG_DEBUG, "DISEqC source: %d", diseqc_src);
    }
    else if (type == dev::DVB_T)
    {
        u_int code_rate_lp = dev->getIntParam(dev::CODE_RATE_LP);
        g_object_set (G_OBJECT (dvbsrc), "code-rate-lp",
                      code_rate_lp, NULL);

        u_int modulation = dev->getIntParam(dev::MODULATION);
        g_object_set (G_OBJECT (dvbsrc), "modulation",
                      modulation, NULL);

        u_int transmode = dev->getIntParam(dev::TRANSMODE);
        g_object_set (G_OBJECT (dvbsrc), "trans-mode",
                      transmode, NULL);

        u_int hierarchy = dev->getIntParam(dev::HIERARCHY);
        g_object_set (G_OBJECT (dvbsrc), "hierarchy",
                      hierarchy, NULL);


        u_int guard = dev->getIntParam(dev::GUARD);
        g_object_set (G_OBJECT (dvbsrc), "guard",
                      guard, NULL);

    }

    // do links
    {
        gst::Element pipeline(getPipeline());
        gst_bin_add_many (GST_BIN (pipeline.getElement()),
                          dvbsrc, m_mpegtsparse, NULL);
        gst_element_link(dvbsrc, m_mpegtsparse);
    }
}

// Call bus callback
gboolean GSTProcessor::callBus(GstMessage* msg)
{
    if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ELEMENT)
    {
        // check SDT/PMT
        if (gst_message_get_structure(msg))
        {
            const GstStructure *structure = gst_message_get_structure(msg);
            const char* name =
                gst_structure_get_name(structure);
            BOOST_ASSERT(name);
            if (strcasecmp(name, "sdt") == 0)
            {
                processSDT(structure);
            }
            if (strcasecmp(name, "pmt") == 0)
            {
                processPMT(structure);
            }
        }
        // should got messages again
        return TRUE;
    }

    // default processing
    return gst::Thread::callBus(msg);
}

// Calls when we receive SDT message
void GSTProcessor::processSDT(const GstStructure *structure)
{
    BOOST_ASSERT(structure);
    gboolean actual_ts = FALSE;
    gst_structure_get_boolean (structure,
                               "actual-transport-stream", &actual_ts);
    if (actual_ts)
    {
        GValue services = (*gst_structure_get_value (structure, "services"));
        guint size = gst_value_list_get_size (&services);
        /* Iterate over services*/
        for (guint i = 0; i < size; i++)
        {
            GValue val = (*gst_value_list_get_value (&services, i));
            const GstStructure* service = gst_value_get_structure (&val);
            /* Returns "service-%d"*/
            BOOST_ASSERT(gst_structure_get_name (service));
            std::string srvname = gst_structure_get_name (service);
            klk_log(KLKLOG_DEBUG, "Service name: %s", srvname.c_str());

            guint program_number = 0U;
            sscanf(srvname.c_str(), "service-%u", &program_number);

            gboolean scrambled = FALSE;
            if (gst_structure_has_field (service, "scrambled"))
            {
                gst_structure_get_boolean(service, "scrambled", &scrambled);
            }
            else
            {
                klk_log(KLKLOG_DEBUG,
                        "Service  %s scrambling status is unknown",
                        srvname.c_str());
            }
            if (scrambled)
            {
                klk_log(KLKLOG_DEBUG, "Service  %s is scrambled",
                        srvname.c_str());
            }
            std::string name = "";
            if (gst_structure_has_field (service, "name"))
            {
                name = gst_structure_get_string (service, "name");
                klk_log(KLKLOG_DEBUG, "Name: %s", name.c_str());
            }

            std::string prvname = "";
            if (gst_structure_has_field (service, "provider-name"))
            {
                prvname =
                    gst_structure_get_string(service, "provider-name");
                klk_log(KLKLOG_DEBUG, "Provider name: %s", prvname.c_str());
            }

            // dump the info
            m_dumpSDT(program_number, name, prvname, scrambled);
        }
    }
}

// Processes pmt structure
void GSTProcessor::processPMT(const GstStructure *structure)
{
    BOOST_ASSERT(structure);
    guint program_number = 0U;
    gst_structure_get_uint (structure, "program-number", &program_number);
    GValue streams = (*gst_structure_get_value (structure, "streams"));
    guint size = gst_value_list_get_size (&streams);
    for (guint i = 0; i < size; i++)
    {
        try
        {
            GValue stream_val = (*gst_value_list_get_value (&streams,
                                                            ((guint) (i))));
            const GstStructure* stream = gst_value_get_structure (&stream_val);
            guint pid = 0U;
            gst_structure_get_uint (stream, "pid", &pid);
            std::string pid_type = PID_UNKNOWN;
            /* See ISO/IEC 13818-1 Table 2-29*/
            guint stream_type = 0U;
            gst_structure_get_uint (stream, "stream-type", &stream_type);
            if (stream_type == 0x01 ||
                stream_type == 0x02 ||
                stream_type == 0x1b)
            {
                // video
                klk_log(KLKLOG_DEBUG, "PMT: Program number %u, video pid: %u",
                        program_number, pid);
                pid_type = PID_VIDEO;

            }
            else if (stream_type == 0x03 || stream_type == 0x04 ||
                     stream_type == 0x0f || stream_type == 0x11)
            {
                // audio
                klk_log(KLKLOG_DEBUG, "PMT: Program number %u, audio pid: %u",
                        program_number, pid);
                pid_type = PID_AUDIO;
            }
            else
            {
                // unknown
                klk_log(KLKLOG_DEBUG, "PMT: Program number %u, unknown pid: %u",
                        program_number, pid);
                continue;

            }
            m_dumpPMT(program_number, pid, pid_type);
        }
        catch(const boost::bad_lexical_cast&)
        {
            klk_log(KLKLOG_ERROR, "Wrong conversion during PMT parse. "
                    "Program number %u",
                    program_number);
            // ignore it
        }
    }
}

// registre processors
void GSTProcessor::registerProcessors(SDTFunction sdt, PMTFunction pmt)
{
    m_dumpSDT = sdt;
    m_dumpPMT = pmt;
}

// cleans the processor internals
void GSTProcessor::clean() throw()
{
    Thread::clean();
    if (m_mpegtsparse)
    {
        //gst_object_unref (m_mpegtsparse);
        m_mpegtsparse = NULL;
    }
}
