/**
   @file stream.cpp
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
   - 2009/01/01 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <boost/bind.hpp>

#include "stream.h"
#include "log.h"
#include "exception.h"
#include "defines.h"

// extrenal modules
#include "dvb/dvbdev.h"

/*

  Re: [Libevent-users] new member

  Steven Grimm
  Fri, 12 Jan 2007 07:16:10 -0800
  As the guy who added thread support to memcached, I feel qualified to answer this one:

  What libevent doesn't support is sharing a libevent instance across threads. It works just fine to use libevent in a multithreaded process where only one thread is making libevent calls.

  What also works, and this is what I did in memcached, is to use multiple instances of libevent. Each call to event_init() will give you back an "event base," which you can think of as a handle to a libevent instance. (That's documented in the current manual page, but NOT in the older manpage on the libevent home page.) You can have multiple threads each with its own event base and everything works fine, though you probably don't want to install signal handlers on more than one thread.

  In the case of memcached, I allocate one event base per thread at startup time. One thread handles the TCP listen socket; when a new request comes in, it calls accept() then hands the file descriptor to another thread to handle from that point on -- that is, each client is bound to a particular thread. All you have to do is call event_base_set() after you call event_set() and before you call event_add().

  Unfortunately, you pretty much have to use pipe() to communicate between libevent threads, That's a limitation, and honestly it's a pretty big one: it makes a master/worker thread architecture, where one thread handles all the I/O, much less efficient than you'd like. My initial implementation in memcached used an architecture like that and it chewed lots of CPU time. That's not really libevent's fault -- no UNIX-ish system I'm aware of has an equivalent to the Windows WaitForMultipleObjects API that allows you to wake up on semaphores / condition variables and on input from the network. Without that, any solution is going to end up using pipes (or maybe signals, which have their own set of issues in a multithreaded context) to wake up the libevent threads.

  -Steve
*/

using namespace klk;
using namespace klk::dvb::stream;
using namespace klk::dvb::stream::getstream2;

// dev stat collector callback
static void update_info_timer(int fd, short event, void *arg)
{
    try
    {
        Stream *thread = static_cast<Stream*>(arg);
        BOOST_ASSERT(thread);

        thread->updateInfo();

        thread->initInfoTimer();
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Got an exception while updating info");
    }

}


//
// Stream class
//

// Constructor
Stream::Stream(const IDevPtr& dev) :
    m_dev(dev),
    m_lock(), m_streams(), m_add_streams(), m_del_streams()
{
    BOOST_ASSERT(m_dev);

    memset(&m_adapter, 0, sizeof(m_adapter));
}

// Destructor
Stream::~Stream()
{
    clean();
}

// Do cleanup
void Stream::clean() throw()
{
    if (m_adapter.klkbase)
        event_base_free(m_adapter.klkbase);
    memset(&m_adapter, 0, sizeof(m_adapter));

    // no lock for the dev
    m_dev->setParam(dev::HASLOCK, "0");

    // not processed streams
    Locker lock(&m_lock);
    m_add_streams.clear();
    m_del_streams.clear();
    m_streams.clear();
}

// Adds a station
void Stream::addStation(const IStationPtr& station)
{
    Locker lock(&m_lock);
    m_add_streams.push_back(station);
}

// Dels a station
void Stream::delStation(const IStationPtr& station)
{
    Locker lock(&m_lock);
    m_del_streams.push_back(station);
}

// Checks if there any stream activity or not
const bool Stream::isStream() const
{
    Locker lock(&m_lock);
    return !m_streams.empty();
}

// Inits pipeline
void Stream::init()
{
    clean();

    // event base structure
    m_adapter.klkbase = event_base_new();
    BOOST_ASSERT(m_adapter.klkbase);

    try
    {
        initAdapter();
        checkAdapter();
        // check dev event init
        initInfoTimer();
    }
    catch(...)
    {
        releaseInit();
        clean();
        throw;
    }
}

// Starts the thread
void Stream::start()
{
    // base should be initialized
    BOOST_ASSERT(m_adapter.klkbase);

    try
    {
        // start main loopt
        event_base_dispatch(m_adapter.klkbase);
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Exception in event_base_dispatch(): %s",
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Unknown exception in event_base_dispatch()");
    }

    // deinit adapter
    releaseInit();

    // release streams
    GList *sl = g_list_first(m_adapter.streams);
    while(sl)
    {
        struct stream_s *stream = static_cast<struct stream_s*>(sl->data);
        if (stream)
        {
            // deinit the stream
            stream_deinit(stream);
            freeStream(stream);
        }
        sl=g_list_next(sl);
    }
    g_list_free(m_adapter.streams);
    m_adapter.streams = NULL;

    // final clearing
    clean();
}

// Stops the thread
void Stream::stop() throw()
{
    if (!m_adapter.klkbase)
        return; // nothing to do (not initialized)

    // break the loop
    if (event_base_loopbreak(m_adapter.klkbase) == -1)
    {
        klk_log(KLKLOG_ERROR, "Error in event_base_loopbreak()");
    }
}

// Updates DVB stat info
void Stream::updateInfo()
{
    fe_status_t status;
    uint16_t snr, signal;
    uint32_t ber, unc;

    ioctl(m_adapter.fe.fd, FE_READ_STATUS, &status);
    ioctl(m_adapter.fe.fd, FE_READ_SIGNAL_STRENGTH, &signal);
    ioctl(m_adapter.fe.fd, FE_READ_SNR, &snr);
    ioctl(m_adapter.fe.fd, FE_READ_BER, &ber);
    ioctl(m_adapter.fe.fd, FE_READ_UNCORRECTED_BLOCKS, &unc);

    // update dev
    if (signal == 0)
    {
        klk_log(KLKLOG_ERROR, "Adapter '%s' does not have signal",
                m_dev->getStringParam(dev::NAME).c_str());
    }

    m_dev->setParam(dev::SIGNAL, signal);
    m_dev->setParam(dev::SNR, snr);
    m_dev->setParam(dev::BER, ber);
    m_dev->setParam(dev::UNC, unc);
    if (status & FE_HAS_LOCK)
    {
        m_dev->setParam(dev::HASLOCK, 1);
    }
    else
    {
        m_dev->setParam(dev::HASLOCK, 0);
        klk_log(KLKLOG_ERROR, "Adapter '%s' does not have lock",
                m_dev->getStringParam(dev::NAME).c_str());
    }

    // update dev rate
    try
    {
        int devrate = getRate(m_adapter.dvr.klkstat);
        m_dev->setParam(dev::RATE, devrate);
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Failed to update dev rate in DVB streamer: %s",
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Failed to update dev rate in DVB streamer: unknown error");
    }

    // update new stations to add /delete
    Locker lock(&m_lock);

    // update current rates
    std::for_each(m_streams.begin(), m_streams.end(),
                  boost::bind(&Stream::updateRate, this, _1));


    // add list first
    if (status & FE_HAS_LOCK)
    {
        std::for_each(m_add_streams.begin(), m_add_streams.end(),
                      boost::bind(&Stream::initStation, this, _1));

        if (!m_add_streams.empty())
        {
            m_adapter.klknewprog = 1;
            m_add_streams.clear();
        }
    }

    // now del stations
    std::for_each(m_del_streams.begin(), m_del_streams.end(),
                  boost::bind(&Stream::deinitStation, this, _1));
    m_del_streams.clear();
}

// Inits dev stat timer
void Stream::initInfoTimer()
{
    BOOST_ASSERT(m_adapter.klkbase);
    struct timeval tv;
    tv.tv_sec = PLUGIN_UPDATEINFOINTERVAL;
    tv.tv_usec = 0;
    evtimer_set(&m_checkdevevent, update_info_timer, this);
    event_base_set(m_adapter.klkbase, &m_checkdevevent);
    evtimer_add(&m_checkdevevent, &tv);
}

// Frees resources allocated for stream info
void Stream::freeStream(struct stream_s* stream)
{
    if (!stream)
        return; // nothing to do

    // free tmp data from output
    GList *ol = g_list_first(stream->output);
    while (ol)
    {
        struct output_s *output =
            static_cast<struct output_s*>(ol->data);
        KLKFREE(output->remoteaddr);
        KLKFREE(output->localaddr);
        KLKFREE(output);
        ol=g_list_next(ol);
    }
    g_list_free(stream->output);

    // free input
    GList *il = g_list_first(stream->input);
    while (il)
    {
        struct input_s *input =
            static_cast<struct input_s*>(il->data);
        KLKFREE(input);
        il=g_list_next(il);
    }
    g_list_free(stream->input);

    // free stream itself
    KLKFREE(stream);
}

// Inits adapter
void Stream::initAdapter()
{
    // FIXME!!! add it into config
    m_adapter.budgetmode = 1;
    m_adapter.streams = NULL;
    m_adapter.dvr.stuckinterval = 300000;
    m_adapter.dvr.buffer.size = 50;
    m_adapter.klknewprog = 0;

    const std::string type = m_dev->getStringParam(dev::TYPE);
    int adapter_no = m_dev->getIntParam(dev::ADAPTER);
    m_adapter.no = adapter_no;

    // FIXME!!! frontend does not used
    //int frontend_no =
    //boost::lexical_cast<int>(m_dev->getParam(dev::FRONTEND));
    u_int frequency = m_dev->getIntParam(dev::FREQUENCY);

    // FIXME!!! not used
    //int code_rate_hp =
    //boost::lexical_cast<int>(m_dev->getParam(dev::CODE_RATE_HP));
    if (type == dev::DVB_S)
    {
        m_adapter.type = AT_DVBS;
        m_adapter.fe.dvbs.t_freq = frequency;

        if (m_dev->getStringParam(dev::POLARITY) == "H" ||
            m_dev->getStringParam(dev::POLARITY) == "h")
        {
            m_adapter.fe.dvbs.t_pol = POL_H;
        }
        else if ((m_dev->getStringParam(dev::POLARITY) == "V") ||
                 (m_dev->getStringParam(dev::POLARITY) == "v"))
        {
            m_adapter.fe.dvbs.t_pol = POL_V;
        }
        else
        {
            throw Exception(__FILE__, __LINE__,
                            "Unsupported polarization: " +
                            m_dev->getStringParam(dev::POLARITY));
        }

        u_int symbol_rate = m_dev->getIntParam(dev::SYMBOL_RATE) * 1000;
        m_adapter.fe.dvbs.t_srate = symbol_rate;

        // taken from gst dvbsrc plugin code
        // FIXME!!! This code provides more better determination based
        // on the freq.
        // http://www.google.com/codesearch/p?hl=en&sa=N&cd=6&ct=rc#RGBKplqGMjQ/vlc-0.8.6/modules/access/dvb/linux_dvb.c&q=%220xe0,%200x10,%200x38,%200xf0,%200x00,%200x00%22&l=884
        const u_long SLOF(11700*1000UL);
        const u_long LOF1(9750*1000UL);
        const u_long LOF2(10600*1000UL);
        m_adapter.fe.dvbs.lnb_lof1 = LOF1;
        m_adapter.fe.dvbs.lnb_lof2 = LOF2;
        m_adapter.fe.dvbs.lnb_slof = SLOF;

        // DISEqC source
        m_adapter.fe.dvbs.t_diseqc = m_dev->getIntParam(dev::DISEQC_SRC);

        // FIXME!!! is it real truth
        // just a guess
        m_adapter.fe.dvbs.lnbsharing = 0; //disable sharing
    }
    else if (type == dev::DVB_T)
    {
        m_adapter.type = AT_DVBT;
        m_adapter.fe.dvbt.freq = frequency;

        /* 0 (Auto) 6 (6Mhz), 7 (7Mhz), 8 (8Mhz) */
        m_adapter.fe.dvbt.bandwidth =
            m_dev->getIntParam(dev::DVBBANDWIDTH);

        /* 0 (Auto) 2 (2Khz), 8 (8Khz) */
        int transmode = m_dev->getIntParam(dev::TRANSMODE);
        m_adapter.fe.dvbt.tmode = transmode;

        int modulation = m_dev->getIntParam(dev::MODULATION);
        m_adapter.fe.dvbt.modulation = modulation;

        int guard = m_dev->getIntParam(dev::GUARD);
        m_adapter.fe.dvbt.guard = guard;

        int hierarchy = m_dev->getIntParam(dev::HIERARCHY);
        m_adapter.fe.dvbt.hierarchy = hierarchy;

        // FIOXME!!! not used
        //int code_rate_lp = m_dev->getIntParam(dev::CODE_RATE_LP);

    }


    /* Tune to the one and only transponder */
    int rc = fe_tune_init(&m_adapter);
    if (rc != 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Failed to tune DVB card: " +
                        m_dev->getStringParam(dev::NAME));
    }

    /* Initialize demux0 dvr0 and co */
    rc = dmx_init(&m_adapter);
    if (rc != 1)
    {
        throw Exception(__FILE__, __LINE__,
                        "Failed dmx init for DVB card: " +
                        m_dev->getStringParam(dev::NAME));
    }

    rc = dvr_init(&m_adapter);
    if (rc != 1)
    {
        throw Exception(__FILE__, __LINE__,
                        "Failed drv init for DVB card: " +
                        m_dev->getStringParam(dev::NAME));
    }
}

// Checks DVB adapter state has it lock and signal
void Stream::checkAdapter()
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 600000;

    event_base_loopexit(m_adapter.klkbase, &tv);

    event_base_dispatch(m_adapter.klkbase);

    // now check the status
    fe_status_t status;

    ioctl(m_adapter.fe.fd, FE_READ_STATUS, &status);

    if (!(status & FE_HAS_SIGNAL))
    {
        throw Exception(__FILE__, __LINE__,
                        "No signal for adapter '%s'",
                        m_dev->getStringParam(dev::NAME).c_str());
    }

    if (!(status & FE_HAS_LOCK))
    {
        throw Exception(__FILE__, __LINE__,
                        "The adapter '%s' does not have lock",
                        m_dev->getStringParam(dev::NAME).c_str());
    }

}

// Tune deinit
void Stream::releaseInit()
{
    dvr_deinit(&m_adapter);
    dmx_deinit(&m_adapter);
    fe_tune_deinit(&m_adapter);
}

// Inits a station
void Stream::initStation(const IStationPtr& station)
{
    struct stream_s* stream = NULL;
    try
    {
        BOOST_ASSERT(station);

        stream =
            static_cast<struct stream_s*>(calloc(1, sizeof(struct stream_s)));
        BOOST_ASSERT(stream);
        stream->adapter = &m_adapter;
        stream->psineeded = 1;
        // stat collector
        stream->klkstat.count = 0;
        if (gettimeofday(&stream->klkstat.lastupdate, NULL) < 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "Error %d in gettimeofday(): %s",
                            errno, strerror(errno));
        }

        // input
        struct input_s* input =
            static_cast<struct input_s*>(calloc(1, sizeof(struct input_s)));
        BOOST_ASSERT(input);
        input->type = INPUT_PNR;
        input->pnr.pnr = station->getChannelNumber();
        stream->input = g_list_append(stream->input, input);
        input->stream = stream;

        // output
        struct output_s* output =
            static_cast<struct output_s*>(calloc(1, sizeof(struct output_s)));
        BOOST_ASSERT(output);
	output->type = OTYPE_UDP;
	output->ttl = 15;
        output->remoteaddr = strdup(station->getRoute().getHost().c_str());
        BOOST_ASSERT(output->remoteaddr);
        output->remoteport = station->getRoute().getPort();
        output->localaddr = NULL;
	output->stream = stream;
        output->sap = NULL; // don send sap (not thread safe yet)
        output->klkstat = &stream->klkstat; // stat collector
	stream->output = g_list_append(stream->output, output);
    }
    catch(...)
    {
        freeStream(stream);
        stream = NULL;
    }

    if (stream)
    {
        // NOTE: it thread safe to do it because all operation
        // done from main libevent thread
        stream_init(stream);
        m_adapter.streams = g_list_append(m_adapter.streams, stream);

        klk_log(KLKLOG_DEBUG,
                "DVB plugin has started DVB stream '%s' "
                "-> udp://%s:%d",
                station->getChannelName().c_str(),
                station->getRoute().getHost().c_str(),
                station->getRoute().getPort());

        // add it to stream station list
        m_streams.push_back(station);
    }
    else
    {
        klk_log(KLKLOG_ERROR,
                "DVB plugin has failed to start DVB stream '%s' "
                "-> udp://%s:%d",
                station->getChannelName().c_str(),
                station->getRoute().getHost().c_str(),
                station->getRoute().getPort());
    }
}

// Deinits a station
void Stream::deinitStation(const IStationPtr& station)
{
    try
    {
        BOOST_ASSERT(station);

        u_int pnr = station->getChannelNumber();

        // find the stream at adapter structure by the program number
        struct stream_s *stream = getStreamStruct(pnr);
        if (stream)
        {
            // deinit the stream
            stream_deinit(stream);
            // remove from the list
            m_adapter.streams = g_list_remove(m_adapter.streams, stream);
            // free allocated resource
            freeStream(stream);
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "DVB plugin has failed to stop DVB stream for '%s'",
                station->getChannelName().c_str());
    }

    m_streams.remove(station);
}

// Finds getstream2 stream_s staruct by program number
struct stream_s* Stream::getStreamStruct(const u_int pnr)
{
    // find the stream at adapter structure by the program number
    GList *sl = g_list_first(m_adapter.streams);
    while(sl)
    {
        struct stream_s *stream = static_cast<struct stream_s*>(sl->data);
        if (stream)
        {
            if (stream->input)
            {
                // only one input here
                GList *il = g_list_first(stream->input);
                struct input_s *input =
                    static_cast<struct input_s*>(il->data);
                if (input->pnr.pnr == pnr)
                {
                    // found it
                    return stream;
                }
            }
        }
        sl=g_list_next(sl);
    }
    return NULL;
}

// Updates rates for the specified channel
void Stream::updateRate(IStationPtr& station) throw()
{
    if (!station)
        return; // nothing to do

    u_int pnr = station->getChannelNumber();

    try
    {
        struct stream_s* stream = getStreamStruct(pnr);
        BOOST_ASSERT(stream);

        int rate = getRate(stream->klkstat);
        station->setRate(rate);
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Failed to update rate for channel '%d': %s",
                pnr,
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Failed to update rate for channel '%d': "
                "unknown error", pnr);
    }
}


// Retrives rate in bytes per second from stat info and
// prepare (clear) the stat info for next collection
const int Stream::getRate(struct klkstat_s& stat) const
{
    struct timeval current;
    if (gettimeofday(&current, NULL) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in gettimeofday(): %s",
                        errno, strerror(errno));
    }

    // calculate rate
    struct timeval diff;
    timersub(&current, &stat.lastupdate, &diff);

    int rate = 0;
    if (stat.count != 0)
    {
        if (diff.tv_sec <= 0 && diff.tv_usec <= 0)
        {
            // cannot calculate rate
            throw Exception(__FILE__, __LINE__, "Incorrect time diff");
        }

        double time = diff.tv_sec + diff.tv_usec * 1.0e-6;
        BOOST_ASSERT(time > 0);
        rate = static_cast<int>(stat.count/time);
    }
    // clear rate info
    stat.count = 0;
    stat.lastupdate = current;

    return rate;
}
