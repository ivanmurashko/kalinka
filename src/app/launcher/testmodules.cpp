/**
   @file testmodules.cpp
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
   - 2010/09/25 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testmodules.h"
#include "testutils.h"
#include "launchfactory.h"
#include "testdefines.h"
#include "testmodfactory.h"
#include "launchmodfactory.h"

#include "maintestdefines.h"

#include "adapter/defines.h"

using namespace klk;
using namespace klk::launcher;

namespace
{
    /// Async message id to be received at the helper module (remotelly)
    const std::string ASYNC_HELP = "async_help";

    /// Async message id to be received at the main module (locally)
    const std::string ASYNC_MAIN = "async_main";

    /// Async message id to be received at the main module (locally) but sent by the helper
    const std::string ASYNC_REMOTE = "async_remote";

    /// Sync message id to be received at the helper module (remotelly)
    const std::string SYNC_HELP = "sync_help";

    /// Sync message id to be received at the main module (locally)
    const std::string SYNC_MAIN = "sync_main";

    /// Sync message id to be received at the main module (locally) but sent by the helper
    const std::string SYNC_REMOTE = "sync_remote";

    /// Result field at sync output message (key)
    const std::string RESULT_KEY = "testresult_key";

    /// Result field at sync output message (value)
    const std::string RESULT_VALUE = "testresult_value";

    /// Helper module id
    const std::string HELPMODID = klk::test::HELPPREFIX + "testhelper";
}

//
// TestBase class
//

/// Constructor
TestBase::TestBase() : m_get_sync(false), m_get_async(false)
{
}

// Do processing of a sync message
void TestBase::processSync(const IMessagePtr& in, const IMessagePtr& out)
{
    out->setData(RESULT_KEY, RESULT_VALUE);
    m_get_sync.push_back(in->getID());
}

// Do processing of an async message
void TestBase::processASync(const IMessagePtr& in)
{
    m_get_async.push_back(in->getID());
}


//
// TestHelpModule class
//

/// Constructor
TestHelpModule::TestHelpModule(IFactory* factory) :
    klk::Module(factory, HELPMODID), TestBase()
{
}

/// @copydoc klk::IModule::registerProcessors
void TestHelpModule::registerProcessors()
{
    // processors
    registerSync(SYNC_HELP, boost::bind(&TestBase::processSync, this, _1, _2));
    registerASync(ASYNC_HELP, boost::bind(&TestBase::processASync, this, _1));
}

//
// TestMainModule class
//

/// Constructor
TestMainModule::TestMainModule() :
    klk::launcher::Module(klk::launcher::Factory::instance(),
                           MAINMODID, "set_cmd_id", "show_cmd_id")
{
}

/// @copydoc klk::IModule::registerProcessors
void TestMainModule::registerProcessors()
{
    klk::launcher::Module::registerProcessors();
    // processors
    registerSync(SYNC_MAIN, boost::bind(&TestBase::processSync, this, _1, _2));
    registerASync(ASYNC_MAIN, boost::bind(&TestBase::processASync, this, _1));
    registerSync(SYNC_REMOTE, boost::bind(&TestBase::processSync, this, _1, _2));
    registerASync(ASYNC_REMOTE, boost::bind(&TestBase::processASync, this, _1));

}


//
// TestModules class
//
/// Constructor
TestModules::TestModules() :
    TestLauncher(), m_help_module(), m_main_module()

{
}

/// Destructor
TestModules::~TestModules()
{
}

/// @copydoc klk::launcher::TestLauncher::tearDown
void TestModules::tearDown()
{
    // clear modules first
    // because the parent's tearDown() will
    // free factory that is used in the modules
    m_help_module.reset();
    m_main_module.reset();

    TestLauncher::tearDown();
}

/// @copydoc klk::test::TestModule::loadModules()
void TestModules::loadModules()
{
    TestLauncher::loadModules();

    test::ModuleFactory* test_mod_factory =
        dynamic_cast<test::ModuleFactory*>(
            test::Factory::instance()->getModuleFactory());
    m_help_module =
        TestHelpModulePtr(new TestHelpModule(test::Factory::instance()));
    test_mod_factory->addDebug(m_help_module);


    ModuleFactory* modfactory =
        dynamic_cast<ModuleFactory*>(Factory::instance()->getModuleFactory());
    CPPUNIT_ASSERT(modfactory);

    m_main_module = TestMainModulePtr(new TestMainModule());
    modfactory->addModule(m_main_module);

    // start the main module
    modfactory->load(MAINMODID);

    // wait for awhile
    sleep(10);
}

/// The test itself
void TestModules::test()
{
    test::printOut("\nLauncher modules test ... ");

    IModuleFactory* remote_mod_factory =
        test::Factory::instance()->getModuleFactory();
    IModuleFactory* local_mod_factory =
        Factory::instance()->getModuleFactory();

    // the main module is loaded locally only
    CPPUNIT_ASSERT(local_mod_factory->isLoaded(MAINMODID) == true);
    CPPUNIT_ASSERT(remote_mod_factory->isLoaded(MAINMODID) == false);

    // load/unload test
    testLoad();

    // messages send
    testMessages();
}

/// test the module load/unload via RPC
void TestModules::testLoad()
{
    IModuleFactory* remote_mod_factory =
        test::Factory::instance()->getModuleFactory();
    IModuleFactory* local_mod_factory =
        Factory::instance()->getModuleFactory();

    // the main module is loaded locally only
    CPPUNIT_ASSERT(local_mod_factory->isLoaded(MAINMODID) == true);
    CPPUNIT_ASSERT(remote_mod_factory->isLoaded(MAINMODID) == false);

    // load the helper module via local factory
    local_mod_factory->load(HELPMODID);
    CPPUNIT_ASSERT(local_mod_factory->isLoaded(HELPMODID) == true);
    CPPUNIT_ASSERT(remote_mod_factory->isLoaded(HELPMODID) == true);

    // Test unload
    local_mod_factory->unload(HELPMODID);
    CPPUNIT_ASSERT(local_mod_factory->isLoaded(HELPMODID) == false);
    CPPUNIT_ASSERT(remote_mod_factory->isLoaded(HELPMODID) == false);

    // Load again
    local_mod_factory->load(HELPMODID);
    CPPUNIT_ASSERT(local_mod_factory->isLoaded(HELPMODID) == true);
    CPPUNIT_ASSERT(remote_mod_factory->isLoaded(HELPMODID) == true);

    // Unload remotely
    remote_mod_factory->unload(HELPMODID);
    CPPUNIT_ASSERT(local_mod_factory->isLoaded(HELPMODID) == false);
    CPPUNIT_ASSERT(remote_mod_factory->isLoaded(HELPMODID) == false);
}

/// test the messages sending via RPC
void TestModules::testMessages()
{
    IModuleFactory* remote_mod_factory =
        test::Factory::instance()->getModuleFactory();
    IModuleFactory* local_mod_factory =
        Factory::instance()->getModuleFactory();

    // the main module is loaded locally only
    CPPUNIT_ASSERT(local_mod_factory->isLoaded(MAINMODID) == true);
    CPPUNIT_ASSERT(remote_mod_factory->isLoaded(MAINMODID) == false);

    // load the helper module via local factory
    local_mod_factory->load(HELPMODID);
    CPPUNIT_ASSERT(local_mod_factory->isLoaded(HELPMODID) == true);
    CPPUNIT_ASSERT(remote_mod_factory->isLoaded(HELPMODID) == true);

    IFactory* local_factory = Factory::instance();
    IFactory* remote_factory = test::Factory::instance();

    // the message retrival test
    IMessagePtr async_msg_help =
        local_factory->getMessageFactory()->getMessage(ASYNC_HELP);
    CPPUNIT_ASSERT(async_msg_help);
    IMessagePtr async_msg_main =
        local_factory->getMessageFactory()->getMessage(ASYNC_MAIN);
    CPPUNIT_ASSERT(async_msg_main);
    IMessagePtr async_msg_remote =
        remote_factory->getMessageFactory()->getMessage(ASYNC_REMOTE);
    CPPUNIT_ASSERT(async_msg_remote);
    IMessagePtr sync_msg_help =
        local_factory->getMessageFactory()->getMessage(SYNC_HELP);
    CPPUNIT_ASSERT(sync_msg_help);
    IMessagePtr sync_msg_main =
        local_factory->getMessageFactory()->getMessage(SYNC_MAIN);
    CPPUNIT_ASSERT(sync_msg_main);
    IMessagePtr sync_msg_remote =
        remote_factory->getMessageFactory()->getMessage(SYNC_REMOTE);
    CPPUNIT_ASSERT(sync_msg_remote);

    // pre condition check
    CPPUNIT_ASSERT(m_help_module->checkASync(ASYNC_HELP) == false);
    CPPUNIT_ASSERT(m_main_module->checkASync(ASYNC_MAIN) == false);
    CPPUNIT_ASSERT(m_main_module->checkASync(ASYNC_REMOTE) == false);
    CPPUNIT_ASSERT(m_help_module->checkSync(SYNC_HELP) == false);
    CPPUNIT_ASSERT(m_main_module->checkSync(SYNC_MAIN) == false);
    CPPUNIT_ASSERT(m_main_module->checkSync(SYNC_REMOTE) == false);

    // async message
    Factory::instance()->getModuleFactory()->sendMessage(async_msg_help);
    sleep(2);
    CPPUNIT_ASSERT(m_help_module->checkASync(ASYNC_HELP) == true);
    Factory::instance()->getModuleFactory()->sendMessage(async_msg_main);
    sleep(2);
    CPPUNIT_ASSERT(m_main_module->checkASync(ASYNC_MAIN) == true);
    // sent remote async message from helper module (mediaserver app)
    // to the main (klklaunch)
    // Ticket #243
    remote_mod_factory->sendMessage(async_msg_remote);
    sleep(2);
    CPPUNIT_ASSERT(m_main_module->checkASync(ASYNC_REMOTE) == true);

    // sync messages

    // test that adapter module is loaded
    CPPUNIT_ASSERT(local_mod_factory->isLoaded(klk::adapter::MODID) == true);
    CPPUNIT_ASSERT(remote_mod_factory->isLoaded(klk::adapter::MODID) == true);

    IMessagePtr resp_msg_help, resp_msg_main, resp_msg_remote;
    m_main_module->sendSyncMessage(sync_msg_help, resp_msg_help);
    CPPUNIT_ASSERT(resp_msg_help);
    CPPUNIT_ASSERT(m_help_module->checkSync(SYNC_HELP) == true);
    CPPUNIT_ASSERT(m_main_module->checkSync(SYNC_HELP) == false);
    CPPUNIT_ASSERT(resp_msg_help->getValue(RESULT_KEY) == RESULT_VALUE);

    // ticket #243 sync messages test
    m_help_module->sendSyncMessage(sync_msg_remote, resp_msg_remote);
    CPPUNIT_ASSERT(resp_msg_remote);
    CPPUNIT_ASSERT(m_help_module->checkSync(SYNC_REMOTE) == false);
    CPPUNIT_ASSERT(m_main_module->checkSync(SYNC_REMOTE) == true);
    CPPUNIT_ASSERT(resp_msg_remote->getValue(RESULT_KEY) == RESULT_VALUE);

    // we send the message to the specified application
    adapter::MessagesProtocol msgproto(launcher::Factory::instance(), MAINMODID);
    resp_msg_main = msgproto.sendSync(sync_msg_main);
    CPPUNIT_ASSERT(resp_msg_main);
    CPPUNIT_ASSERT(m_main_module->checkSync(SYNC_MAIN) == true);
}
