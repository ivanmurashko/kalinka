/**
   @file cliapptest.cpp
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
   - 2009/02/09 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cliapptest.h"
#include "testfactory.h"
#include "cliapp.h"
#include "module.h"
#include "testmodfactory.h"
#include "exception.h"
#include "testutils.h"

const std::string TEST_APP_ID = "testappid";
const std::string TEST_APP_NAME = "testappname";

namespace klk
{
    /**
       @brief Test helpmodule for messaging core tests

       Test helpmodule for messaging core tests

       @ingroup grTest
    */
    class TestHelpModule : public Module
    {
    public:
        /**
           Constructor
        */
        TestHelpModule(IFactory *factory) : Module(factory, TEST_APP_ID){}

        /**
           Destructor
        */
        virtual ~TestHelpModule(){}
    private:
        /**
           Gets a human readable helpmodule name

           @return the name
        */
        virtual const std::string getName() const throw()
            {return TEST_APP_NAME;}

        /**
           @copydoc IModule::registerProcessors
        */
        virtual void registerProcessors(){};

        /**
           @copydoc IModule::getType
        */
        virtual mod::Type getType() const throw() {return mod::APP;}
    private:
        /**
           Copy constructor
        */
        TestHelpModule(const TestHelpModule&);

        /**
           Assignment opearator
        */
        TestHelpModule& operator=(const TestHelpModule&);
    };
}

using namespace klk;
using namespace klk::test;

//
// CLIAppTest class
//

// Constructor
void CLIAppTest::setUp()
{
    m_factory = test::Factory::instance();
    CPPUNIT_ASSERT(m_factory != NULL);

    IConfig* config = m_factory->getConfig();
    CPPUNIT_ASSERT(config != NULL);
    config->load();

    m_launcher = new DBScriptLauncher(config);
    CPPUNIT_ASSERT(m_launcher != NULL);
    m_launcher->recreate();

    m_db = new db::DB(m_factory);
    CPPUNIT_ASSERT(m_db);
    m_db->connect();

    test::ModuleFactory* modfactory =
        dynamic_cast<test::ModuleFactory*>(
            m_factory->getModuleFactory());
    modfactory->addDebug(IModulePtr(new TestHelpModule(m_factory)));
}

// Destructor
void CLIAppTest::tearDown()
{
    KLKDELETE(m_launcher);
    KLKDELETE(m_db);
    test::Factory::instance()->reset();
}

// Tests all CLIApp
void CLIAppTest::testProcess()
{
    printOut("\nCLIApp processing test ... ");

    CPPUNIT_ASSERT(m_factory != NULL);

    StringBoolMap map;
    cli::ICommandPtr setcmd(new cli::AutostartSet("testmsgid1"));
    setcmd->setFactory(m_factory);
    setcmd->setModuleID(TEST_APP_ID);
    cli::ICommandPtr showcmd(new cli::AutostartShow("testmsgid2"));
    showcmd->setFactory(m_factory);
    showcmd->setModuleID(TEST_APP_ID);
    cli::ParameterVector params;

    // test that there is no any records at the db
    map = getApps();
    CPPUNIT_ASSERT(map.size() == 0);
    params.clear();
    std::string response = showcmd->process(params);
    CPPUNIT_ASSERT(response.find("'on'") == std::string::npos);
    CPPUNIT_ASSERT(response.find("'off'") != std::string::npos);

    // enable an application
    params.clear();
    params.push_back(cli::AUTOSTARTSET_ON);
    setcmd->process(params);

    // test that we have a record and it was enabled
    map = getApps();
    CPPUNIT_ASSERT(map.size() == 2); // main application + our test
    StringBoolMap::iterator find = map.find(TEST_APP_NAME + "@testserver");
    CPPUNIT_ASSERT(find != map.end());
    CPPUNIT_ASSERT(find->first == TEST_APP_NAME + "@testserver");
    CPPUNIT_ASSERT(find->second == true);
    params.clear();
    response = showcmd->process(params);
    CPPUNIT_ASSERT(response.find("'on'") != std::string::npos);
    CPPUNIT_ASSERT(response.find("'off'") == std::string::npos);

    // disable the application
    params.clear();
    params.push_back(cli::AUTOSTARTSET_OFF);
    setcmd->process(params);

    // test that we have a record and it was disabled
    map = getApps();
    CPPUNIT_ASSERT(map.size() == 2); // main application + our test
    find = map.find(TEST_APP_NAME + "@testserver");
    CPPUNIT_ASSERT(find != map.end());
    CPPUNIT_ASSERT(find->first == TEST_APP_NAME + "@testserver");
    CPPUNIT_ASSERT(find->second == false);
    params.clear();
    response = showcmd->process(params);
    CPPUNIT_ASSERT(response.find("'on'") == std::string::npos);
    CPPUNIT_ASSERT(response.find("'off'") != std::string::npos);
}

// Retrives application list with enable/disable info
const CLIAppTest::StringBoolMap CLIAppTest::getApps()
{
    db::Parameters params_select;
    params_select.add("@host");
    db::ResultVector rv =
        m_db->callSelect("klk_application_list", params_select, NULL);
    StringBoolMap map;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        //SELECT application, module, name, enabled, status, priority,
        // host, description
        const std::string app = (*i)["name"].toString();
        bool enabled = ((*i)["enabled"].toInt() != 0);
        map[app] = enabled;
    }
    return map;
}

// The unit test itself for command completions
void CLIAppTest::testComplete()
{
    printOut("\nCLIApp completion test ... ");

    CPPUNIT_ASSERT(m_factory != NULL);

    cli::ICommandPtr setcmd(new cli::AutostartSet("testmsgid1"));
    setcmd->setFactory(m_factory);
    setcmd->setModuleID(TEST_APP_ID);
    cli::ICommandPtr showcmd(new cli::AutostartShow("testmsgid2"));
    showcmd->setFactory(m_factory);
    showcmd->setModuleID(TEST_APP_ID);
    cli::ParameterVector params;

    params.clear();
    params.push_back(cli::AUTOSTARTSET_ON);
    cli::ParameterVector setparams;
    params = setcmd->getCompletion(setparams);
    CPPUNIT_ASSERT(params.size() == 2);
    setparams.push_back("aaa");
    params = setcmd->getCompletion(setparams);
    CPPUNIT_ASSERT(params.size() == 0);

    params.clear();
    setparams.clear();
    params = showcmd->getCompletion(setparams);
    CPPUNIT_ASSERT(params.size() == 0);
}
