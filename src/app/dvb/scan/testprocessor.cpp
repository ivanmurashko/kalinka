/**
   @file testprocessor.cpp
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
   - 2009/07/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include "testprocessor.h"
#include "exception.h"
#include "testfactory.h"
#include "testdefines.h"
#include "testutils.h"

// modules specific
#include "dvb/dvbdev.h"
#include "dvb/defines.h"

using namespace klk;
using namespace klk::dvb::scan;

//
// TestProcessor class
//

// Constructor
TestProcessor::TestProcessor() :
    m_sdt(boost::bind<void>(DefaultSDT(), _1, _2, _3, _4)),
    m_pmt(boost::bind<void>(DefaultPMT(), _1, _2, _3)), m_rc(OK)
{
}

// Starts processing
void TestProcessor::start(const IDevPtr& dev)
{
    try
    {
        if (dev->getStringParam(dev::TYPE) == dev::DVB_S)
        {
            doDVBSScan(dev);
        }
        else if (dev->getStringParam(dev::TYPE) == dev::DVB_T)
        {
            doDVBTScan(dev);
        }
        else
        {
            NOTIMPLEMENTED;
        }
    }
    catch (const std::exception& err)
    {
        test::printOut(std::string("\n\tERROR detected at scan thread: ") +
                 err.what() + "\n");
        m_rc = ERROR;
    }
}

// Stops processing
void TestProcessor::stopProcessing() throw()
{
    // nothing to do here yet FIXME!!! add the test
}

// Register SDT and PMT savers
void TestProcessor::registerProcessors(SDTFunction sdt, PMTFunction pmt)
{
    m_sdt = sdt;
    m_pmt = pmt;
}

// Do scan for DVB-S
void TestProcessor::doDVBSScan(const IDevPtr& dev)
{
    // # freq pol sr fec
    // S 12578000 H 19850000 3/4
    // S 12584000 V 27500000 4/5

    klk_log(KLKLOG_DEBUG, "DVB-S scan started at test thread");

    IFactory* factory = test::Factory::instance();
    BOOST_ASSERT(factory);

    // type DVB-S
    BOOST_ASSERT(dev->getStringParam(dev::TYPE) == dev::DVB_S);

    u_int adapter = dev->getIntParam(dev::ADAPTER);
    BOOST_ASSERT(adapter == 2);
    u_int frontend = dev->getIntParam(dev::FRONTEND);
    BOOST_ASSERT(frontend == 0);

    // frequency
    u_int frequency = dev->getIntParam(dev::FREQUENCY);
    u_int symbol_rate = dev->getIntParam(dev::SYMBOL_RATE);
    u_int code_rate_hp = dev->getIntParam(dev::CODE_RATE_HP);
    u_int diseqc_src = dev->getIntParam(dev::DISEQC_SRC);
    BOOST_ASSERT(diseqc_src >=0 && diseqc_src <= 4);
    if (frequency == 12578000)
    {
        BOOST_ASSERT(dev->getStringParam(dev::POLARITY) == "H");
        BOOST_ASSERT(symbol_rate == 19850);
        BOOST_ASSERT(code_rate_hp == 3 /*3/4 - see gst-inspect dvbsrc*/);

        // we got result on 2 diseqc
        if (diseqc_src == 0)
        {
            // send std
            m_sdt(1, TEST_CHANNEL_S_1, TEST_PRV_S_1, false);
            // send pmt
            m_pmt(1, 101, dvb::PID_VIDEO);
            m_pmt(1, 102, dvb::PID_AUDIO);

            // wrong (without audio stream)
            m_sdt(3, "channel_wrong", "prv_wrong", false);
            m_pmt(3, 201, dvb::PID_VIDEO);
        }
        else if (diseqc_src == 3)
        {
            BOOST_ASSERT(diseqc_src == 0);

            // send std
            m_sdt(5, "wrong", "wrong", false);
            // send pmt
            m_pmt(5, 501, dvb::PID_VIDEO);
            m_pmt(5, 502, dvb::PID_AUDIO);
        }

    }
    else if (frequency == 12584000)
    {
        BOOST_ASSERT(dev->getStringParam(dev::POLARITY) == "V");
        BOOST_ASSERT(symbol_rate == 27500);
        BOOST_ASSERT(code_rate_hp == 4 /*4/5 - see gst-inspect dvbsrc*/);

        if (diseqc_src == 4)
        {
            // send pmt
            m_pmt(2, 202, dvb::PID_AUDIO);
            m_pmt(2, 201, dvb::PID_VIDEO);
            // send sdt
            m_sdt(2, TEST_CHANNEL_S_2, TEST_PRV_S_2, false);

            // wrong (scrambled)
            m_sdt(4, "channel_scrambled", "prv_scrampled", true);
            m_pmt(4, 401, dvb::PID_VIDEO);
            m_pmt(4, 402, dvb::PID_AUDIO);
        }
    }
    else
    {
        BOOST_ASSERT(false);
    }

}

// Do scan for DVB-S
void TestProcessor::doDVBTScan(const IDevPtr& dev)
{
    klk_log(KLKLOG_DEBUG, "DVB-T scan started at test thread");

    // type DVB-T
    BOOST_ASSERT(dev->getStringParam(dev::TYPE) == dev::DVB_T);

    IFactory* factory = test::Factory::instance();
    BOOST_ASSERT(factory);

    u_int adapter = dev->getIntParam(dev::ADAPTER);
    BOOST_ASSERT(adapter == 0);
    u_int frontend = dev->getIntParam(dev::FRONTEND);
    BOOST_ASSERT(frontend == 0);

    // frequency
    u_int frequency = dev->getIntParam(dev::FREQUENCY);
    if (frequency == 578000000)
    {
        // send std
        m_sdt(10, TEST_CHANNEL_T, TEST_PRV_T, false);
        // send pmt
        m_pmt(10, 1010, dvb::PID_VIDEO);
        m_pmt(10, 1020, dvb::PID_AUDIO);
    }
    else
    {
        BOOST_ASSERT(false);
    }
}
