/**
   @file scan.cpp
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
   - 2009/06/15 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "scan.h"
#include "exception.h"
#include "defines.h"
#include "scancommand.h"
#include "cliapp.h"


#include "gst/gstscan.h"

// modules specific
#include "dvb/defines.h"
#include "dvb/messages.h"
#include "dvb/dvbdev.h"

using namespace klk;
using namespace klk::dvb::scan;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new Scan(factory));
}

//
// Scan class
//

namespace
{
    // application specific commands
    const std::string SETCLI_MSGID = "764d8ccf-2229-4b90-b62b-eda1e76c8162";
    const std::string SHOWCLI_MSGID = "13157e74-f817-4016-8fb7-342345cee341";
}


// Constructor
Scan::Scan(IFactory *factory) :
    klk::launcher::Module(factory, MODID, SETCLI_MSGID, SHOWCLI_MSGID),
    m_thread(new ScanThread(factory))
{
    BOOST_ASSERT(m_thread);
    addDependency(dvb::MODID);
}

// Destructor
Scan::~Scan()
{
}

// Starts scan procedure for specified source
// and tune table
void Scan::startScan(const std::string& source, const std::string& data)
{
    // alloc a dev for scan
    IMessageFactory* msgfactory = getFactory()->getMessageFactory();
    BOOST_ASSERT(msgfactory);
    IMessagePtr req = msgfactory->getMessage(msg::id::DVBSTART);
    BOOST_ASSERT(req);
    req->setData(msg::key::DVBACTIVITY, dev::SCANING);
    req->setData(msg::key::DVBSOURCE, source);
    IMessagePtr res;
    sendSyncMessage(req, res);
    // test result
    BOOST_ASSERT(res);

    if (res->getValue(msg::key::STATUS) != msg::key::OK)
    {
        // dvb module rejected the channel
        throw Exception(__FILE__, __LINE__,
                        "DVB module rejected DVB scanning");
    }

    try
    {
        m_thread->startScan(res->getValue(msg::key::RESOURCE), data);
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "There was an error during scan: %s",
                err.what());
        stopScan();
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "There was an unknwon error during scan");
        stopScan();
    }
}

// Stops scan
void Scan::stopScan() throw()
{
    m_thread->stopScan();
}

// Checks is there scan started or not
bool Scan::isScanStarted() const throw()
{
    return m_thread->isStarted();
}


// Register all processors
void Scan::registerProcessors()
{
    ModuleWithDB::registerProcessors();

    registerCLI(cli::ICommandPtr(new ScanStart()));
    registerCLI(cli::ICommandPtr(new ScanStop()));

    // register default scan processors
    const IProcessorPtr processor(new GSTProcessor(getFactory()));
    m_thread->registerProcessor(processor);
    // register the thread
    registerThread(m_thread);
}

// Gets a human readable module name
const std::string Scan::getName() const throw()
{
    return MODNAME;
}
