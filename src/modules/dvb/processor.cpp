/**
   @file processor.cpp
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
   - 2009/06/17 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "processor.h"
#include "messages.h"
#include "exception.h"
#include "dvbdev.h"

using namespace klk;
using namespace klk::dvb;

//
// Processor class
//

// Constructor
Processor::Processor(IFactory* factory) :
    m_factory(factory),
    m_channels()
{
    BOOST_ASSERT(m_factory);
}

// Destructor
Processor::~Processor()
{
}

// Do the tune startup
void Processor::doStart(const IMessagePtr& in,
                        const IMessagePtr& out)
{
    BOOST_ASSERT(in);
    // only one can get the start
    Locker lock(&m_tune_lock);
    const std::string activity = in->getValue(msg::key::DVBACTIVITY);
    if (activity == dev::STREAMING)
    {
        doStartStream(in, out);
    }
    else if (activity == dev::SCANING)
    {
        doStartScan(in, out);
    }
    else
    {
        throw Exception(__FILE__, __LINE__,
                             "Unknown DVB activity: " + activity);
    }
}

// Do the tune stop
void Processor::doStop(const IMessagePtr& in)
{
    BOOST_ASSERT(in);
    const std::string activity = in->getValue(msg::key::DVBACTIVITY);
    if (activity == dev::STREAMING)
    {
        doStopStream(in);
    }
    else if (activity == dev::SCANING)
    {
        doStopScan(in);
    }
    else
    {
        throw Exception(__FILE__, __LINE__,
                             "Unknown DVB activity: " + activity);
    }
}

// Do the tune startup for streaming activity
void Processor::doStartStream(const IMessagePtr& in,
                              const IMessagePtr& out)
{
    BOOST_ASSERT(in);

    const std::string type = in->getValue(msg::key::TYPE);
    BOOST_ASSERT(type.empty() == false);
    const std::string source = in->getValue(msg::key::DVBSOURCE);
    BOOST_ASSERT(source.empty() == false);

    StreamChannelPtr channel(new StreamChannel(type, source, m_factory));
    BOOST_ASSERT(channel);

    const std::string channel_id = in->getValue(msg::key::TVCHANNELID);
    BOOST_ASSERT(channel_id.empty() == false);
    // retrive DVB tune data and other info from DB
    channel->setData(channel_id);

    m_channels.insertStreamChannel(channel); // can produce exception

    // setting some result info
    BOOST_ASSERT(out);

    const std::string channel_name = channel->getName();
    BOOST_ASSERT(channel_name.empty() == false);
    out->setData(msg::key::TVCHANNELNAME, channel_name);

    const std::string channel_no = channel->getNumber();
    BOOST_ASSERT(channel_no.empty() == false);
    out->setData(msg::key::TVCHANNELNO, channel_no);

    const std::string resource =
        channel->getDev()->getStringParam(dev::UUID);
    out->setData(msg::key::RESOURCE, resource);

    // set initial dvb info
    IDevPtr dev = channel->getDev();
    BOOST_ASSERT(dev);
    dev->setState(dev::WORK);
    dev->setParam(dev::DVBACTIVITY, dev::STREAMING);
    dev->setParam(dev::HASLOCK, 0);
    dev->setParam(dev::SIGNAL, 0);
    dev->setParam(dev::SNR, 0);
    dev->setParam(dev::UNC, 0);
    dev->setParam(dev::BER, 0);
    dev->setParam(dev::LOSTLOCK, 0);
    dev->setParam(dev::RATE, 0);
}

// Do the streaming tune stop
void Processor::doStopStream(const IMessagePtr& in)
{
    BOOST_ASSERT(in);

    const std::string channel_id = in->getValue(msg::key::TVCHANNELID);
    BOOST_ASSERT(channel_id.empty() == false);

    StreamChannelPtr channel = m_channels.getStreamChannel(channel_id);

    if (!channel)
    {
        // nothing to do
        // no such streaming channels
        return;
    }

    klk_log(KLKLOG_DEBUG, "DVB start stop for %s",
            channel->getName().c_str());

    m_channels.removeStreamChannel(channel);
}

struct CheckFreeDev
{
    inline bool operator()(const IDevPtr dev, const std::string source)
    {
        if (dev->getStringParam(dev::SOURCE) == source)
        {
            return (dev->getState() == dev::IDLE);
        }
        return false;
    }
};


// Do the tune startup for scaning activity
void Processor::doStartScan(const IMessagePtr& in,
                            const IMessagePtr& out)
{
    const std::string source = in->getValue(msg::key::DVBSOURCE);
    BOOST_ASSERT(source.empty() == false);

    // try to find idle dev with such type
    IDevList devs =
        m_factory->getResources()->getResourceByType(dev::DVB_ALL);
    IDevList::iterator find =
        std::find_if(devs.begin(), devs.end(),
                     boost::bind<bool>(CheckFreeDev(), _1, source));
    if (find == devs.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "No free DVB dev for scan on '%s'",
                             source.c_str());
    }

    BOOST_ASSERT(out);
    out->setData(msg::key::RESOURCE, (*find)->getStringParam(dev::UUID));

    // set dev state
    (*find)->setState(dev::WORK);
    (*find)->setParam(dev::DVBACTIVITY, dev::SCANING);
}

// Do the scaning tune stop
void Processor::doStopScan(const IMessagePtr& in)
{
    // get tune dev
    const std::string resource_id = in->getValue(msg::key::RESOURCE);
    BOOST_ASSERT(resource_id.empty() == false);
    // retrive dev info
    IResources* resources = m_factory->getResources();
    BOOST_ASSERT(resources);
    const IDevPtr dev = resources->getResourceByUUID(resource_id);
    BOOST_ASSERT(dev);

    // clear dev state
    dev->setState(dev::IDLE);
}

// Do cleanup
void Processor::clean()
{
    m_channels.clear(); // this will clean temp db info

    // clear dev states
    IDevList devs =
        m_factory->getResources()->getResourceByType(dev::DVB_ALL);
    std::for_each(devs.begin(), devs.end(),
                  boost::bind(&IDev::setState, _1, dev::IDLE));
}

// Processes signal lost message
void Processor::doSignalLost(const IDevPtr& dev) throw()
{
    try
    {
        BOOST_ASSERT(dev);

        // set a field about lost lock
        dev->setParam(dev::LOSTLOCK, 1, true);
        // free the resource
        dev->setState(dev::IDLE);

        // send signal lost event
        IMessagePtr msg =
            m_factory->getMessageFactory()->getMessage(msg::id::DVBSIGNALLOST);
        BOOST_ASSERT(msg);
        msg->setData(msg::key::RESOURCE, dev->getStringParam(dev::UUID));
        m_factory->getModuleFactory()->sendMessage(msg);
    }
    catch (const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Got an error during signal lost processing: %s",
                err.what());
    }
    catch (...)
    {
        klk_log(KLKLOG_ERROR,
                "Got an unknown error during signal lost processing");
    }
}


// Retrives a channel list for the specified device
const StringList Processor::getChannelNames(const IDevPtr& dev) const
{
    StreamChannelList list = m_channels.getStreamChannels(dev);
    StringList res;
    // stop all channel associated with the dev
    for (StreamChannelList::iterator i = list.begin();
         i != list.end(); i++)
    {
        // we should add it
        res.push_back((*i)->getName());
    }

    return res;
}

