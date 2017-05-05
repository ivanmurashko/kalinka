/**
   @file dvb.cpp
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/07/26 created by ipp (Ivan Murashko)
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

#include "dvb.h"
#include "dvbdev.h"
#include "common.h"
#include "log.h"
#include "defines.h"
#include "infocommand.h"
#include "resourcecommand.h"

#include "traps.h"
#include "utils.h"
#include "exception.h"
#include "messages.h"

#include "snmp/factory.h"
#include "snmp/scalar.h"
#include "snmp/table.h"

using namespace klk;
using namespace klk::dvb;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new DVB(factory));
}


//
// DVB
//

// Constructor
// @param[in] factory the module factory
DVB::DVB(IFactory *factory) :
    ModuleWithDB(factory, MODID),
    m_processor(new Processor(factory))
{
    BOOST_ASSERT(m_processor);
}

// Destructor
DVB::~DVB()
{
}

// Gets a human readable module name
const std::string DVB::getName() const throw()
{
    return MODNAME;
}

// Does pre actions before start main loop
void DVB::preMainLoop()
{
    // preparation staff
    ModuleWithDB::preMainLoop();
    m_processor->clean();
}

// Does post actions after main loop
void DVB::postMainLoop() throw()
{
    m_processor->clean();
    ModuleWithDB::postMainLoop();
}

// Register all processors
void DVB::registerProcessors()
{
    ModuleWithDB::registerProcessors();

    // processors
    registerSync(
        msg::id::DVBSTART,
        boost::bind(&Processor::doStart, m_processor, _1, _2));
    registerASync(
        msg::id::DVBSTOP,
        boost::bind(&Processor::doStop, m_processor, _1));

    registerCLI(cli::ICommandPtr(new SetSourceCommand()));
    registerCLI(cli::ICommandPtr(new InfoCommand()));

    // register actions in a separate threads
    registerTimer(boost::bind(&DVB::checkDVBDevs, this),
                              CHECKINTERVAL);

    registerSNMP(boost::bind(&DVB::processSNMP, this, _1), MODID);
}

// Process changes at the DB
// @param[in] msg - the input message
void DVB::processDB(const IMessagePtr& msg)

{
    BOOST_ASSERT(msg);

    // update DVB devs info from DB
    IDevList devs =
        getFactory()->getResources()->getResourceByType(dev::DVB_ALL);
    std::for_each(devs.begin(), devs.end(),
                  boost::bind(&IDev::update, _1));
}

// Checks DVB devs state
void DVB::checkDVBDevs()
{
    IDevList devs =
        getFactory()->getResources()->getResourceByType(dev::DVB_ALL);
    std::for_each(devs.begin(), devs.end(),
                  boost::bind(&DVB::checkDVBDev, this, _1));
}

// Checks a DVB dev state
void DVB::checkDVBDev(const IDevPtr& dev)
{
    BOOST_ASSERT(dev);

    // check last update time
    bool old = (static_cast<u_long>(time(NULL)) >
                dev->getLastUpdateTime() + CHECKINTERVAL);

    // first of all to clear lost lock flag
    // to be able to use the dev
    // dont update last update time
    dev->setParam(dev::LOSTLOCK, 0, true);

    if (dev->getState() == dev::IDLE)
    {
        // just clear lost lock flag
        return;
    }

    // check update time
    if (old)
    {
        getFactory()->getSNMP()->sendTrap(
            TRAP_TIMEOUT,
            dev->getStringParam(dev::UUID));
        klk_log(KLKLOG_ERROR,
                "Cannot retrive DVB device state within %d seconds. "
                "Device UUID: '%s'", CHECKINTERVAL,
                dev->getStringParam(dev::UUID).c_str());

        // signal lost
        m_processor->doSignalLost(dev);

        return;
    }

    // check has lock
    if (dev->getIntParam(dev::HASLOCK) == 0)
    {
        getFactory()->getSNMP()->sendTrap(
            TRAP_NOSIGNAL,
            dev->getStringParam(dev::UUID));
        klk_log(KLKLOG_ERROR,
                "DVB dev does not get a lock "
                "Device name: '%s'",
                dev->getStringParam(dev::NAME).c_str());

        // signal lost
        m_processor->doSignalLost(dev);

        return;
    }

    // check signal
    if (dev->getIntParam(dev::SIGNAL) < SIGNAL_THRESHOLD)
    {
        getFactory()->getSNMP()->sendTrap(
            TRAP_BADSIGNAL,
            dev->getStringParam(dev::UUID));
        klk_log(KLKLOG_ERROR,
                "Bad signal strength: Failed %d < %d. "
                "Device name: '%s'",
                signal, SIGNAL_THRESHOLD,
                dev->getStringParam(dev::NAME).c_str());
#if 0 //FIXME!!! sometime can really have signal
        if (signal == 0)
        {
            // signal lost
            m_processor->doSignalLost(dev);
        }
#endif
    }

    // check snr
    int snr = dev->getIntParam(dev::SNR);
    if (snr < SNR_THRESHOLD)
    {
        getFactory()->getSNMP()->sendTrap(
            TRAP_BADSNR,
            dev->getStringParam(dev::UUID));
        klk_log(KLKLOG_ERROR,
                "Bad SNR: %d < %d. "
                "Device name: '%s'",
                snr, SNR_THRESHOLD,
                dev->getStringParam(dev::NAME).c_str());
    }

    // check ber
    int ber = dev->getIntParam(dev::BER);
    if (ber > BER_THRESHOLD)
    {
        getFactory()->getSNMP()->sendTrap(
            TRAP_BADBER,
            dev->getStringParam(dev::UUID));
        klk_log(KLKLOG_ERROR,
                "Bad BER: %d > %d. "
                "Device name: '%s'",
                ber, BER_THRESHOLD,
                dev->getStringParam(dev::NAME).c_str());
    }

    // check unc
    int unc = dev->getIntParam(dev::UNC);
    if (unc > UNC_THRESHOLD)
    {
        getFactory()->getSNMP()->sendTrap(
            TRAP_BADUNC,
            dev->getStringParam(dev::UUID));
        klk_log(KLKLOG_ERROR,
                "Bad UNC: %d > %d. "
                "Device name: '%s'",
                unc, UNC_THRESHOLD,
                dev->getStringParam(dev::NAME).c_str());
    }
}

// Processes SNMP request
const snmp::IDataPtr DVB::processSNMP(const snmp::IDataPtr& req)
{
    BOOST_ASSERT(req);
    snmp::ScalarPtr reqreal =
        boost::dynamic_pointer_cast<snmp::Scalar, snmp::IData>(req);
    BOOST_ASSERT(reqreal);

    const std::string reqstr = reqreal->getValue().toString();
    // support only snmp::GETSTATUSTABLE
    if (reqstr != snmp::GETSTATUSTABLE)
    {
        throw Exception(__FILE__, __LINE__,
                             "Unknown SNMP request: " + reqstr);

    }

    // create the response
    // table with data
    snmp::TablePtr table(new snmp::Table());

    IDevList devs =
        getFactory()->getResources()->getResourceByType(dev::DVB_ALL);
    u_int count = 0;
    for (IDevList::iterator dev = devs.begin(); dev != devs.end();
         dev++, count++)
    {
        // klkIndex      Counter32
        // klkCardName   DisplayString,
        // klkCardType   DisplayString,
        // klkAdapter    Integer32,
        // klkFrontend   Integer32,
        // klkHasLock    TruthValue,
        // klkSignal     Integer32,
        // klkSNR        Integer32,
        // klkBER        Integer32,
        // klkUNC        Counter32,
        // klkRate       Counter32


        snmp::TableRow row;
        try
        {
            row.push_back(count);
            row.push_back((*dev)->getStringParam(dev::NAME));
            const std::string type = (*dev)->getStringParam(dev::TYPE);
            if (type == dev::DVB_S)
            {
                row.push_back(DVB_S_NAME);
            }
            else if (type == dev::DVB_T)
            {
                row.push_back(DVB_T_NAME);
            }
            else if (type == dev::DVB_C)
            {
                row.push_back(DVB_C_NAME);
            }
            else
            {
                row.push_back(NOTAVAILABLE);
            }

            row.push_back((*dev)->getIntParam(dev::ADAPTER));
            row.push_back((*dev)->getIntParam(dev::FRONTEND));
            if ((*dev)->hasParam(dev::HASLOCK))
            {
                row.push_back((*dev)->getIntParam(dev::HASLOCK));
                row.push_back((*dev)->getIntParam(dev::SIGNAL));
                row.push_back((*dev)->getIntParam(dev::SNR));
                row.push_back((*dev)->getIntParam(dev::BER));
                row.push_back((*dev)->getIntParam(dev::UNC));
            }
            else
            {
                row.push_back(0);
                row.push_back(0);
                row.push_back(0);
                row.push_back(0);
                row.push_back(0);
            }

            if ((*dev)->hasParam(dev::RATE))
                row.push_back((*dev)->getIntParam(dev::RATE));
            else
                row.push_back(0);
        }
        catch(...)
        {
            row.clear();
            row.push_back(count);
            row.push_back(NOTAVAILABLE);
            row.push_back(NOTAVAILABLE);
            row.push_back(0);
            row.push_back(0);
            row.push_back(0);
            row.push_back(0);
            row.push_back(0);
            row.push_back(0);
            row.push_back(0);
            row.push_back(0);
        }
        table->addRow(row);
    }

    return table;
}
