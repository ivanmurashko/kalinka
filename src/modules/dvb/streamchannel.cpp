/**
   @file streamchannel.cpp
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
   - 2008/11/03 created by ipp (Ivan Murashko)
   - 2009/01/04 some exceptions were added by ipp (Ivan Murashko)
   - 2009/06/19 Tuned channel staff was removed
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "streamchannel.h"
#include "log.h"
#include "dev.h"
#include "utils.h"
#include "traps.h"
#include "dvbdev.h"
#include "exception.h"

using namespace klk;
using namespace klk::dvb;

//
// StreamChannel class
//

// Constructor
StreamChannel::StreamChannel(const std::string& type,
                             const std::string& source,
                             IFactory* factory) :
    m_factory(factory),
    m_channel_id(""),
    m_type(type), m_source(source),
    m_name(""), m_number(""),
    m_dev()
{
    BOOST_ASSERT(m_factory);
}


// Destructor
StreamChannel::~StreamChannel()
{
}


// Sets the data
void StreamChannel::setData(const std::string& channel_id)
{
    m_channel_id = channel_id;
    BOOST_ASSERT(m_type.getValue().empty() == false);
    if (m_type == dev::DVB_S)
    {
        setTune4DVBS();
    }
    else if (m_type == dev::DVB_T)
    {
        setTune4DVBT();
    }
    else
    {
        // not supported type
        NOTIMPLEMENTED;
    }

    BOOST_ASSERT(m_name.getValue().empty() == false);
    BOOST_ASSERT(m_number.getValue().empty() == false);
}

// tune DVB-S dev
void StreamChannel::setTune4DVBS()
{
    BOOST_ASSERT(m_channel_id.getValue().empty() == false);
    BOOST_ASSERT(m_type.getValue() == dev::DVB_S);

    // retrive info from DB
    db::Parameters params;
    params.add("@channel", m_channel_id);
    const std::string spname = "klk_dvb_s_channel_get";
    // IN channel VARCHAR(40),
    // OUT signal_source VARCHAR(40),
    // OUT name VARCHAR(50),
    // OUT provider VARCHAR(50),
    // OUT diseqc_source TINYINT,
    // OUT frequency INT,
    // OUT polarity TINYINT,
    // OUT symbol_rate INT,
    // OUT code_rate_hp TINYINT,
    // OUT channel_no INTEGER,
    // OUT return_value INT
    params.add("@signal_source");
    params.add("@name");
    params.add("@provider");
    params.add("@diseqc_source");
    params.add("@frequency");
    params.add("@polarity");
    params.add("@symbol_rate");
    params.add("@code_rate_hp");
    params.add("@channel_no");
    params.add("@return_value");
    db::DB db(m_factory);
    db.connect();
    db::Result result = db.callSimple(spname, params);
    if (result["@return_value"].toInt() != 0)
    {
        throw Exception(
            __FILE__, __LINE__,
            "Cannot retrive info for DVB-S channel with id '" +
            m_channel_id.getValue() + "'");
    }
    // parameters
    const int frequency = result["@frequency"].toInt();
    const int code_rate_hp = result["@code_rate_hp"].toInt();
    const std::string polarity = result["@polarity"].toString();
    BOOST_ASSERT(polarity.empty() == false);
    const int symbol_rate = result["@symbol_rate"].toInt();
    const int diseqc_src = result["@diseqc_source"].toInt();
    BOOST_ASSERT(m_dev == NULL);
    findWorkingDev(frequency);
    if (m_dev)
    {
        // found something
        // check it
        BOOST_ASSERT(frequency == m_dev->getIntParam(dev::FREQUENCY));
        BOOST_ASSERT(code_rate_hp == m_dev->getIntParam(dev::CODE_RATE_HP));
        BOOST_ASSERT(polarity == m_dev->getStringParam(dev::POLARITY));
        BOOST_ASSERT(symbol_rate == m_dev->getIntParam(dev::SYMBOL_RATE));
        BOOST_ASSERT(diseqc_src == m_dev->getIntParam(dev::DISEQC_SRC));
    }
    else
    {
        findFreeDev();
        if (m_dev)
        {
            m_dev->setParam(dev::FREQUENCY, frequency);
            m_dev->setParam(dev::CODE_RATE_HP, code_rate_hp);
            m_dev->setParam(dev::POLARITY, polarity);
            m_dev->setParam(dev::SYMBOL_RATE, symbol_rate);
            m_dev->setParam(dev::DISEQC_SRC, diseqc_src);
        }
    }

    if (!m_dev)
    {
        // we could not find the dev
        throw Exception(__FILE__, __LINE__,
                             "There is no any free DVB-S card for channel "
                             "with ID: " +
                             m_channel_id.getValue());
    }

    // final setup
    m_name = result["@name"].toString();
    BOOST_ASSERT(m_name.getValue().empty() == false);
    m_number = result["@channel_no"].toString();
    BOOST_ASSERT(m_number.getValue().empty() == false);
}

// tune DVB-T dev
void StreamChannel::setTune4DVBT()
{
    BOOST_ASSERT(m_channel_id.getValue().empty() == false);
    BOOST_ASSERT(m_type.getValue() == dev::DVB_T);

    // retrive info from DB
    db::Parameters params;
    params.add("@channel", m_channel_id);
    const std::string spname = "klk_dvb_t_channel_get";
    // IN channel VARCHAR(40),
    // OUT signal_source VARCHAR(40),
    // OUT name VARCHAR(50),
    // OUT provider VARCHAR(50),
    // OUT frequency INT,
    // OUT code_rate_hp TINYINT,
    // OUT code_rate_lp TINYINT,
    // OUT modulation TINYINT,
    // OUT transmode TINYINT,
    // OUT channel_no INTEGER,
    // OUT guard TINYINT,
    // OUT hierarchy TINYINT,
    // OUT bandwidth TINYINT,
    // OUT return_value INT
    params.add("@signal_source");
    params.add("@name");
    params.add("@provider");
    params.add("@frequency");
    params.add("@code_rate_hp");
    params.add("@code_rate_lp");
    params.add("@modulation");
    params.add("@transmode");
    params.add("@channel_no");
    params.add("@guard");
    params.add("@hierarchy");
    params.add("@bandwidth");
    params.add("@return_value");

    db::DB db(m_factory);
    db.connect();
    db::Result result = db.callSimple(spname, params);
    if (result["@return_value"].toInt() != 0)
    {
        throw Exception(
            __FILE__, __LINE__,
            "Cannot retrive info for DVB-T channel with id '" +
            m_channel_id.getValue() + "'");
    }
    // parameters
    const int frequency = result["@frequency"].toInt();
    const int code_rate_hp = result["@code_rate_hp"].toInt();
    const int code_rate_lp = result["@code_rate_lp"].toInt();
    const int modulation = result["@modulation"].toInt();
    const int transmode = result["@transmode"].toInt();
    const int guard = result["@guard"].toInt();
    const int hierarchy = result["@hierarchy"].toInt();
    const int bandwidth = result["@bandwidth"].toInt();

    BOOST_ASSERT(m_dev == NULL);
    findWorkingDev(frequency);
    if (m_dev)
    {
        // found something
        // check it
        BOOST_ASSERT(frequency == m_dev->getIntParam(dev::FREQUENCY));
        BOOST_ASSERT(code_rate_hp == m_dev->getIntParam(dev::CODE_RATE_HP));
        BOOST_ASSERT(code_rate_lp == m_dev->getIntParam(dev::CODE_RATE_LP));
        BOOST_ASSERT(modulation == m_dev->getIntParam(dev::MODULATION));
        BOOST_ASSERT(transmode == m_dev->getIntParam(dev::TRANSMODE));
        BOOST_ASSERT(guard == m_dev->getIntParam(dev::GUARD));
        BOOST_ASSERT(hierarchy == m_dev->getIntParam(dev::HIERARCHY));
        BOOST_ASSERT(bandwidth == m_dev->getIntParam(dev::DVBBANDWIDTH));
    }
    else
    {
        findFreeDev();
        if (m_dev)
        {
            m_dev->setParam(dev::FREQUENCY, frequency);
            m_dev->setParam(dev::CODE_RATE_HP, code_rate_hp);
            m_dev->setParam(dev::CODE_RATE_LP, code_rate_lp);
            m_dev->setParam(dev::MODULATION, modulation);
            m_dev->setParam(dev::TRANSMODE, transmode);
            m_dev->setParam(dev::GUARD, guard);
            m_dev->setParam(dev::HIERARCHY, hierarchy);
            m_dev->setParam(dev::DVBBANDWIDTH, bandwidth);
        }
    }

    if (!m_dev)
    {
        // we could not find the dev
        throw Exception(__FILE__, __LINE__,
                             "There is no any free DVB-T card for channel "
                             "with ID: " +
                             m_channel_id.getValue());
    }

    // final setup
    m_name = result["@name"].toString();
    BOOST_ASSERT(m_name.getValue().empty() == false);
    m_number = result["@channel_no"].toString();
    BOOST_ASSERT(m_number.getValue().empty() == false);
}

// equlance functor
struct CheckDevByFrequency
{
    inline bool operator()(const IDevPtr dev, int freq,
                           std::string source)
    {
        if (dev->hasParam(dev::SOURCE) == false)
            return false;

        if (dev->getIntParam(dev::LOSTLOCK) != 0)
        {
            klk_log(KLKLOG_DEBUG, "DVB dev '%s' has lost lock "
                    "and was ignored for continue streaming",
                    dev->getStringParam(dev::NAME).c_str());
            return false; // the dev lost lock
        }

        // look only STREAMMING
        if (!dev->hasParam(dev::DVBACTIVITY))
            return false; // ignore without activity
        if (dev->getStringParam(dev::DVBACTIVITY) != dev::STREAMING)
            return false; // ignore not streaming
        // Frequency should be set
        BOOST_ASSERT(dev->hasParam(dev::FREQUENCY) == true);
        return (dev->getIntParam(dev::FREQUENCY) == freq &&
                dev->getStringParam(dev::SOURCE) == source);
    }
};

// Finds a dev by frequency
void StreamChannel::findWorkingDev(const int frequency)
{
    IDevList devs = m_factory->getResources()->getResourceByType(m_type);
    IDevList::iterator find =
        std::find_if(devs.begin(), devs.end(),
                     boost::bind<bool>(CheckDevByFrequency(), _1,
                                       frequency, m_source.getValue()));
    if (find != devs.end())
    {
        m_dev = *find;
    }
}

struct CheckFreeDev
{
    inline bool operator()(const IDevPtr dev, const std::string source)
    {
        if (dev->hasParam(dev::SOURCE) == false)
            return false;

        if (dev->getIntParam(dev::LOSTLOCK) != 0)
        {
            klk_log(KLKLOG_DEBUG, "DVB dev '%s' has lost lock "
                    "and was ignored for new streaming",
                    dev->getStringParam(dev::NAME).c_str());
            return false; // the dev lost lock
        }

        return (dev->getState() == dev::IDLE &&
                dev->getStringParam(dev::SOURCE) == source);
    }
};

// Finds a not used dev
void StreamChannel::findFreeDev()
{
    IDevList devs = m_factory->getResources()->getResourceByType(m_type);
    IDevList::iterator find =
        std::find_if(devs.begin(), devs.end(),
                     boost::bind<bool>(CheckFreeDev(), _1,
                                       m_source.getValue()));
    if (find != devs.end())
    {
        m_dev = *find;
    }
}

// Gets resource (device)
const IDevPtr StreamChannel::getDev() const
{
    BOOST_ASSERT(m_dev);
    return m_dev;
}

// Gets name
const std::string StreamChannel::getName() const throw()
{
    return m_name.getValue();
}

// Gets number
const std::string StreamChannel::getNumber() const throw()
{
    return m_number.getValue();
}

// Gets the channel id
const std::string StreamChannel::getID() const throw()
{
    return m_channel_id.getValue();
}

//
// StreamContainer class
//

/**
   Functor to find a channel by id
*/
struct FindChannelByChannel :
    public std::binary_function<StreamChannelPtr, StreamChannelPtr, bool>
{
    /**
       Finder itself by channel
    */
    inline bool operator()(const StreamChannelPtr channel1,
                           const StreamChannelPtr channel2)
    {
        BOOST_ASSERT(channel1);
        BOOST_ASSERT(channel2);

        return (channel1->getID() == channel2->getID());
    }
};

// Constructor
StreamContainer::StreamContainer() :
    m_lock(), m_list()
{
}

// Destructor
StreamContainer::~StreamContainer()
{
}

// Copy constructor
StreamContainer::StreamContainer(const StreamContainer& value) :
    m_lock(), m_list()
{
    Locker lock(&value.m_lock);
    m_list = value.m_list;
}

// Removes stream channel
void StreamContainer::removeStreamChannel(const StreamChannelPtr& channel)
{
    BOOST_ASSERT(channel);

    Locker lock(&m_lock);
    StreamChannelList::iterator find =
        std::find_if(m_list.begin(), m_list.end(),
                     boost::bind(FindChannelByChannel(), _1, channel));
    if (find != m_list.end())
        m_list.erase(find);
}

// Insert stream channel
void StreamContainer::insertStreamChannel(const StreamChannelPtr& channel)
{
    BOOST_ASSERT(channel);

    Locker lock(&m_lock);
    StreamChannelList::iterator find =
        std::find_if(m_list.begin(), m_list.end(),
                     boost::bind(FindChannelByChannel(), _1, channel));
    if (find != m_list.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "Channel '%s' with id '%s' "
                             "has been already added",
                             (*find)->getName().c_str(),
                             (*find)->getID().c_str());
    }

    m_list.push_back(channel);
}

// Retrives stream channels list for the specified device
const StreamChannelList
StreamContainer::getStreamChannels(const IDevPtr& dev) const
{
    BOOST_ASSERT(dev);

    StreamChannelList list;
    Locker lock(&m_lock);
    for (StreamChannelList::const_iterator i = m_list.begin();
         i != m_list.end(); i++)
    {
        if ((*i)->getDev() == dev)
        {
            // we should add it
            list.push_back(*i);
        }
    }

    return list;
}

// Clears the list
void StreamContainer::clear() throw()
{
    Locker lock(&m_lock);
    m_list.clear();
}

/**
   Functor to find a channel by id
*/
struct FindChannelByID :
    public std::binary_function<StreamChannelPtr, std::string, bool>
{
    /**
       Finder itself by channel
    */
    inline bool operator()(const StreamChannelPtr channel,
                           const std::string id)
    {
        BOOST_ASSERT(channel);

        return (channel->getID() == id);
    }
};


// Retrives stream channel
const StreamChannelPtr
StreamContainer::getStreamChannel(const std::string& id) const
{
    Locker lock(&m_lock);
    StreamChannelList::const_iterator find =
        std::find_if(m_list.begin(), m_list.end(),
                     boost::bind(FindChannelByID(), _1, id));

    if (find == m_list.end())
    {
        // nothing was find
        return StreamChannelPtr();
    }

    return *find;
}

// Check is there empty container or not
bool StreamContainer::empty() const throw()
{
    Locker lock(&m_lock);
    return m_list.empty();
}
