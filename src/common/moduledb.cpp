/**
   @file moduledb.cpp
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
   - 2008/09/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "moduledb.h"
#include "common.h"
#include "log.h"
#include "exception.h"

// modules specific info
#include "checkdb/defines.h"

using namespace klk;

// Constructor
ModuleWithDB::ModuleWithDB(IFactory *factory,
                           const std::string& id) :
    Module(factory, id)
{
    // dependences on db checker
    addDependency(db::MODID);
}

// Destructor
ModuleWithDB::~ModuleWithDB()
{
}

// Register all processors
void ModuleWithDB::registerProcessors()
{
    registerASync(
        msg::id::CHANGEDB,
        boost::bind(&ModuleWithDB::processDB, this, _1));

    registerASync(
        msg::id::INITFROMDB,
        boost::bind(&ModuleWithDB::initFromDB, this, _1));
}

// Do some actions before main loop
void ModuleWithDB::preMainLoop()
{
    Module::preMainLoop();

    // add change db event to be processed during start
    IMessagePtr msg =
        getFactory()->getMessageFactory()->getMessage(msg::id::INITFROMDB);
    addMessage(msg);
}

// Register a message that is used for DB update.
void ModuleWithDB::registerDBUpdateMessage(const std::string&  msgid)
{
    registerSync(
        msgid,
        boost::bind(&ModuleWithDB::processDBUpdate, this, _1, _2));
}

//  The processor for DB update message
void ModuleWithDB::processDBUpdate(const IMessagePtr& in,
                                   const IMessagePtr& out)
{
    klk_log(KLKLOG_DEBUG, "Processing change db sync message. Module: %s",
            getName().c_str());

    BOOST_ASSERT(in);
    BOOST_ASSERT(out);
    processDB(in);
}

// @copydoc klk::Module::init
void ModuleWithDB::init()
{
    Module::init();
    registerStartupCheckpoint();
}

// Reads first data portion from DB
void ModuleWithDB::initFromDB(const IMessagePtr& in)
{
    klk_log(KLKLOG_DEBUG, "Processing intialization from db message. Module: %s",
            getName().c_str());
    try
    {
        processDB(in);
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Exception got during DB initialization "
                "at '%s' module: %s", getName().c_str(), err.what());
        // we assuned that we passed the checkpoint
        // because some data can be not finaly sett (httpstreamer) and produce an exception
        // during load data from db
        passStartupCheckpoint();
        throw;
    }

    passStartupCheckpoint();
}
