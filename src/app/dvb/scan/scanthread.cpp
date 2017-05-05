/**
   @file scanthread.cpp
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
   - 2008/12/28 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <errno.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include "scanthread.h"
#include "defines.h"
#include "log.h"
#include "utils.h"
#include "exception.h"
#include "scan.h"

// module specific
#include "dvb/defines.h"
#include "dvb/dvbdev.h"
#include "dvb/messages.h"

using namespace klk;
using namespace klk::dvb::scan;

//
// ScanThread class
//

// Constructor
ScanThread::ScanThread(IFactory* factory) :
    Thread(),
    m_factory(factory),
    m_trigger(),
    m_data(), m_scan_channel(),
    m_dev(), m_conv(), m_processor()
{
    BOOST_ASSERT(m_factory);
}

// starts the thread
void ScanThread::start()
{
    while (!isStopped())
    {
        m_trigger.startWait(0);
        try
        {
            if (!isStopped())
            {
                doScan();
            }
        }
        catch(const std::exception& err)
        {
            klk_log(KLKLOG_ERROR, "Error detected during scan: %s",
                    err.what());
        }
        catch(...)
        {
            klk_log(KLKLOG_ERROR, "Unknown error detected during scan");
        }
    }
}

// stops the thread
void ScanThread::stop() throw()
{
    Thread::stop();
    m_trigger.stopWait();
}

// Is there scan started or not
bool ScanThread::isStarted() const throw()
{
    Locker lock(&m_lock);
    return (m_dev != NULL);
}

// Registers the scan processor
void ScanThread::registerProcessor(const IProcessorPtr& processor)
{
    Locker lock(&m_lock);
    BOOST_ASSERT(processor);
    processor->registerProcessors(
        boost::bind(&ScanThread::saveSDT, this, _1, _2, _3, _4),
        boost::bind(&ScanThread::savePMT, this, _1, _2, _3));
    m_processor = processor;
}



// Stops scan procedure on the specified adapter
void ScanThread::stopScan() throw()
{
    Locker lock(&m_lock);
    if (m_processor)
        m_processor->stopProcessing();
}

// Starts scan procedure on the specified adapter
void ScanThread::startScan(const std::string& uuid, const std::string& data)

{
    BOOST_ASSERT(uuid.empty() == false);
    BOOST_ASSERT(data.empty() == false);


    // check the dev
    IResources* resources = m_factory->getResources();
    BOOST_ASSERT(resources);
    Locker lock(&m_lock);
    BOOST_ASSERT(m_dev == NULL);
    m_dev = resources->getResourceByUUID(uuid);
    if (m_dev == NULL)
    {
        throw Exception(__FILE__, __LINE__,
                        "Dev with UUID '" + uuid +
                        "' is unavailable");
    }

    // parse scan data
    setData(data);

    // start scan procedure
    m_trigger.stopWait();
}

// Do the scan job
void ScanThread::doScan()
{
    klk_log(KLKLOG_DEBUG, "Scan started");

    ScanData tmp;
    {
        Locker lock(&m_lock);
        BOOST_ASSERT(m_processor);
        BOOST_ASSERT(m_dev);
        tmp = m_data;
    }

    for (ScanData::iterator item = tmp.begin(); item != tmp.end(); item++)
    {

        klk_log(KLKLOG_DEBUG, "Scanning process: tune dev to '%s'",
                item->c_str());

        ScanData data = base::Utils::split(*item, " \t");
        BOOST_ASSERT(m_dev);

        try
        {
            if (data[0] == "S")
            {
                doScanDVBS(data);
            }
            else if (data[0] == "T")
            {
                doScanDVBT(data);
            }
            else
            {
                throw Exception(__FILE__, __LINE__,
                                "Unknown DVB type '%s' in scan",
                                data[0].c_str());
            }
        }
        catch(...)
        {
            klk_log(KLKLOG_ERROR, "Got an error during scan");
            // just ignore it
        }
    }

    // stop scanning and so some clearing
    clearScan();

    klk_log(KLKLOG_DEBUG, "Scan finished");
}

// Do scan for DVB-S dev
void ScanThread::doScanDVBS(const ScanData& data)
{
    IDevPtr dev;
    {
        Locker lock(&m_lock);
        tuneDVBS(data);
        dev = m_dev;
    }

    bool found = false;
    for (u_int diseqc = 0; diseqc < 5; diseqc++)
    {
        dev->setParam(dev::DISEQC_SRC, diseqc);
        try
        {
            // processing
            m_processor->start(dev);
        }
        catch(...)
        {
            // ignore any error here
            // incorrect diseqc can produce an error
            // during tune
            // FIXME!!! bad code
        }
        // found something?
        found = wasFound();
        // save result
        save2DB();
        if (found)
            break;
    }

    if (!found)
    {
        throw Exception(__FILE__, __LINE__, "Failed to scan");
    }
}

// Do scan for DVB-T dev
void ScanThread::doScanDVBT(const ScanData& data)
{
    IDevPtr dev;
    {
        Locker lock(&m_lock);
        tuneDVBT(data);
        dev = m_dev;
    }

    // processing
    m_processor->start(dev);
    // save result
    save2DB();
}


// equlance functor
class CheckChannelNumber
{
public:
    inline bool operator()(const ScanChannel channel, uint no)
        {
            return (channel.getChannelNumber() == no);
        }
};

// Save SDT data
void ScanThread::saveSDT(u_int no,
                         const std::string& name,
                         const std::string& prvname,
                         bool scrambled)
{
    Locker lock(&m_lock);
    ScanChannelList::iterator i =
        std::find_if(m_scan_channel.begin(),
                     m_scan_channel.end(),
                     boost::bind<bool>(CheckChannelNumber(), _1, no));
    if (i == m_scan_channel.end())
    {
        klk_log(KLKLOG_DEBUG,
                "TV channel with no '%d' was created from SDT ", no);
        BOOST_ASSERT(m_dev);
        ScanChannel channel(no, m_dev);
        channel.setChannelName(name);
        channel.setProviderName(prvname);
        channel.setScrambling(scrambled);
        m_scan_channel.push_back(channel);
    }
    else
    {
        i->setChannelName(name);
        i->setProviderName(prvname);
        i->setScrambling(scrambled);
    }
}

// Save PMT data
void ScanThread::savePMT(u_int no, u_int pid, const std::string& type)
{
    Locker lock(&m_lock);
    ScanChannelList::iterator i =
        std::find_if(m_scan_channel.begin(),
                     m_scan_channel.end(),
                     boost::bind<bool>(CheckChannelNumber(), _1, no));
    if (i == m_scan_channel.end())
    {
        klk_log(KLKLOG_DEBUG,
                "TV channel with no '%d' was created from PMT ", no);
        BOOST_ASSERT(m_dev);
        ScanChannel channel(no, m_dev);
        channel.addPid(pid, type);
        m_scan_channel.push_back(channel);
    }
    else
    {
        i->addPid(pid, type);
    }
}

// Sets scan data
void ScanThread::setData(const std::string& data)
{
    Locker lock(&m_lock);
    // read whole file
    m_data.clear();
    m_scan_channel.clear();

    const StringList list = base::Utils::getConfig(data);
    for (StringList::const_iterator i = list.begin(); i != list.end(); i++)
    {
        if (*(i->begin()) == 'S' ||
            *(i->begin()) == 'T')
        {
            m_data.push_back(*i);
        }
        else
        {
            klk_log(KLKLOG_DEBUG, "Unsupported scan info: %s", i->c_str());
        }
    }
}

// Do tune for DVB-S dev
void ScanThread::tuneDVBS(const ScanData& data)
{
    // note: it's locked at the caller

    BOOST_ASSERT(data.size() == 5);

    // check dev type
    BOOST_ASSERT(m_dev->getStringParam(dev::TYPE) == dev::DVB_S);

    //# freq pol sr fec
    //S 12578000 H 19850000 3/4
    m_dev->setParam(dev::FREQUENCY, data[1]);
    m_dev->setParam(dev::POLARITY, data[2]);
    // symbol rate has to be deleted by 1000 for GST
    uint sr = boost::lexical_cast<uint>(data[3]);
    const std::string sr_conv =
        boost::lexical_cast<std::string>(sr/1000);
    m_dev->setParam(dev::SYMBOL_RATE, sr_conv);

    m_dev->setParam(dev::CODE_RATE_HP,
                    m_conv.getCodeRate(data[4]));
}

// Do tune for DVB-T dev
void ScanThread::tuneDVBT(const ScanData& data)
{
    // note: it's locked at the caller

    BOOST_ASSERT(data.size() == 9);

    // check dev type
    BOOST_ASSERT(m_dev->getStringParam(dev::TYPE) == dev::DVB_T);

    // # T freq bw fec_hi fec_lo mod transmission-mode
    // guard-interval hierarchy
    // T 578000000 8MHz 3/4 NONE QAM64 8k 1/32 NONE
    m_dev->setParam(dev::FREQUENCY, data[1]);

    m_dev->setParam(dev::DVBBANDWIDTH,
                    m_conv.getBandwidth(data[2]));

    m_dev->setParam(dev::CODE_RATE_HP,
                    m_conv.getCodeRate(data[3]));

    m_dev->setParam(dev::CODE_RATE_LP,
                    m_conv.getCodeRate(data[4]));

    m_dev->setParam(dev::MODULATION,
                    m_conv.getModulation(data[5]));

    m_dev->setParam(dev::TRANSMODE,
                    m_conv.getTransmode(data[6]));

    m_dev->setParam(dev::GUARD,
                    m_conv.getGuard(data[7]));

    m_dev->setParam(dev::HIERARCHY,
                    m_conv.getHierarchy(data[8]));

}

// Saves result to the DB
void ScanThread::save2DB()
{
    Locker lock(&m_lock);
    try
    {
        db::DB db(m_factory);
        db.connect();
        klk_log(KLKLOG_DEBUG, "Saving scan info to database");
        std::for_each(m_scan_channel.begin(), m_scan_channel.end(),
                      boost::bind(&ScanChannel::save2DB, _1, boost::ref(db)));
    }
    catch(...)
    {
        // clear the current state
        m_scan_channel.clear();
        throw;
    }

    // clear the current state
    m_scan_channel.clear();
}

// Do scan stop
void ScanThread::clearScan()
{
    Locker lock(&m_lock);

    // send stop signal to DVB module
    IMessagePtr request =
        m_factory->getMessageFactory()->getMessage(msg::id::DVBSTOP);
    request->setData(msg::key::RESOURCE,
                     m_dev->getStringParam(dev::UUID));
    request->setData(msg::key::DVBACTIVITY, dev::SCANING);
    m_factory->getModuleFactory()->sendMessage(request);

    // reset the dev
    m_dev.reset();
}

//  Checks was there something found during scan or not
bool ScanThread::wasFound() const throw()
{
    Locker lock(&m_lock);
    return !m_scan_channel.empty();
}
