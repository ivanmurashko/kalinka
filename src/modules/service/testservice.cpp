/**
   @file testservice.cpp
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
   - 2008/07/27 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include <boost/bind.hpp>

#include "testservice.h"
#include "defines.h"
#include "testid.h"
#include "service.h"
#include "testutils.h"
#include "utils.h"
#include "test/testdefines.h"
#include "paths.h"
#include "utils.h"

#include "testcli.h"

using namespace klk;
using namespace klk::srv;

// The function inits module's unit test
void klk_module_test_init()
{
    const std::string TESTMAIN = MODNAME + "/main";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestService, TESTMAIN);
    CPPUNIT_REGISTRY_ADD(TESTMAIN, MODNAME);

    const std::string TESTCLI = MODNAME + "/cli";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCLI, TESTCLI);
    CPPUNIT_REGISTRY_ADD(TESTCLI, MODNAME);

    CPPUNIT_REGISTRY_ADD(MODNAME, klk::test::ALL);
}

//
// TestService class
//


// Constructor
TestService::TestService() :
    klk::test::TestModuleWithDB("create_system_test_data.sql")
{
}

// Loads all necessary modules at setUp()
void TestService::loadModules()
{
    // remove prev. results
    base::Utils::unlink(srv::test::RESULTDIR);
    // create the empty folder for future tests
    base::Utils::mkdir(srv::test::RESULTDIR);

    // to be able use RPC for different objects
    loadAdapter();
    sleep(3);

    CPPUNIT_ASSERT(m_modfactory);
    m_modfactory->load(MODID);
}

// The unit test for CLI
void TestService::testApps()
{
    klk::test::printOut("\nService Apps test ... ");

    ServicePtr system = this->getModule<Service>(MODID);

    // we have 3 modules
    // first two apps are loaded during startup
    // at the second test we remove one (1st) app and add another one 3d

    sleep(5); // wait for awhile until the module to be really started

#if 0
    // check sys info first
    const time_t uptime = system->getUptime();
    CPPUNIT_ASSERT(uptime > 3);
    const double cpu = system->getTotalCPUUsage();
    if (cpu < 0 || cpu > 1)
    {
        throw Exception(__FILE__, __LINE__, "Incorrect CPU usage: %f",
                        cpu);
    }
    const double cpu_internal = system->getCPUUsage();
    CPPUNIT_ASSERT(cpu_internal > 0 && cpu_internal < 1);
    const long mem = system->getTotalMemUsage();
    CPPUNIT_ASSERT(mem > 0);

    // check first two
    CPPUNIT_ASSERT(isStarted(srv::test::APP1_ID) == true);
    // check cpu usage
    const double
        app1_cpu = m_modfactory->getModule(srv::test::APP1_ID)->getCPUUsage();
    if (app1_cpu < 0 || app1_cpu > 1)
    {
        throw Exception(__FILE__, __LINE__, "Incorrect CPU usage: %f",
                        app1_cpu);
    }
#endif
    CPPUNIT_ASSERT(isStarted(srv::test::APP2_ID) == true);
    CPPUNIT_ASSERT(isStarted(srv::test::APP3_ID) == false);
    CPPUNIT_ASSERT(system->m_apps.size() == 2);

    CPPUNIT_ASSERT(system->getAppInfo(srv::test::APP1_ID));
    CPPUNIT_ASSERT(system->getAppInfo(srv::test::APP2_ID));
    CPPUNIT_ASSERT(!system->getAppInfo(srv::test::APP3_ID));

    CPPUNIT_ASSERT(getRunCount(srv::test::APP1_NAME) == 1);
    CPPUNIT_ASSERT(getRunCount(srv::test::APP2_NAME) == 1);
    CPPUNIT_ASSERT(getRunCount(srv::test::APP3_NAME) == 0);


    // second test
    CPPUNIT_ASSERT(m_launcher);
    m_launcher->executeScript("update_system_test_data.sql");
    // send changed DB event
    // FIXME!!! replace with db listener module
    IMessagePtr start_msg = m_msgfactory->getMessage(msg::id::CHANGEDB);
    CPPUNIT_ASSERT(start_msg);

    m_modfactory->sendMessage(start_msg);

    sleep(5);

    CPPUNIT_ASSERT(system->m_apps.size() == 2);

    CPPUNIT_ASSERT(!system->getAppInfo(srv::test::APP1_ID));
    CPPUNIT_ASSERT(system->getAppInfo(srv::test::APP2_ID));
    CPPUNIT_ASSERT(system->getAppInfo(srv::test::APP3_ID));

    CPPUNIT_ASSERT(isStarted(srv::test::APP1_ID) == false);
    CPPUNIT_ASSERT(isStarted(srv::test::APP2_ID) == true);
    CPPUNIT_ASSERT(isStarted(srv::test::APP3_ID) == true);

    CPPUNIT_ASSERT(getRunCount(srv::test::APP1_NAME) == 1);
    CPPUNIT_ASSERT(getRunCount(srv::test::APP2_NAME) == 1);
    CPPUNIT_ASSERT(getRunCount(srv::test::APP3_NAME) == 1);

    // third test
    // APP2 should got SIGSEGV signal and should be restarted
    sendSigSegv(srv::test::APP2_ID);
    // wait for awhile
    sleep(25);

    CPPUNIT_ASSERT(!system->getAppInfo(srv::test::APP1_ID));
    CPPUNIT_ASSERT(system->getAppInfo(srv::test::APP2_ID));
    CPPUNIT_ASSERT(system->getAppInfo(srv::test::APP3_ID));

    // test uptime
    // we slept more than 20 second (25) before
    CPPUNIT_ASSERT(system->getAppInfo(srv::test::APP2_ID)->getUptime() > 20);
    CPPUNIT_ASSERT(system->getAppInfo(srv::test::APP3_ID)->getUptime() > 20);


    CPPUNIT_ASSERT(getRunCount(srv::test::APP1_NAME) == 1);
    CPPUNIT_ASSERT(getRunCount(srv::test::APP2_NAME) == 2);
    CPPUNIT_ASSERT(getRunCount(srv::test::APP3_NAME) == 1);
    // and again
    // APP2 should got SIGSEGV signal and should be restarted
    sendSigSegv(srv::test::APP2_ID);
    // wait for awhile
    sleep(25);
    CPPUNIT_ASSERT(isStarted(srv::test::APP2_ID) == true);
    CPPUNIT_ASSERT(getRunCount(srv::test::APP2_NAME) == 3);
}

// Sends SIGSEGV to the application with id specified at
// the argument
void TestService::sendSigSegv(const std::string& app_id)
{
    ServicePtr system = this->getModule<Service>(MODID);
    AppInfoList list = system->m_apps.getAll();
    for (AppInfoList::iterator item = list.begin();
         item != list.end(); item++)
    {
        if ((*item)->getAppUUID() == app_id)
        {
            // send SIGSEGV
            kill((*item)->getPid(), SIGSEGV);
            break;
        }
    }
}

// @return how many times the applicatiopn was run
int TestService::getRunCount(const std::string& appname)
{
    const std::string file = srv::test::RESULTDIR + "/" + appname;
    if (base::Utils::fileExist(file))
    {
        BinaryData data = base::Utils::readWholeDataFromFile(file);
        return boost::lexical_cast<int>(data.toString());
    }
    return 0;
}

// Tests is the application was started or not
//static
bool TestService::isStarted(const std::string& id)
{
    // FIXME!!! A lot of code dublicate with Utils::runScript

    std::string real_path = dir::INST + "/bin/klktestservice.sh";
    if (base::Utils::fileExist(real_path) == false)
    {
        throw Exception(__FILE__, __LINE__,
                             "Path '" + real_path + "' does not exist");
    }

    std::string prg = "/bin/sh", prg_name = "sh";
    if (base::Utils::fileExist(prg) == false)
    {
        throw Exception(__FILE__, __LINE__,
                             "Path '" + std::string(prg) +
                             "' does not exist");
    }

    pid_t pid = 0;
    int status = 0;

    switch (pid = fork())
    {
    case -1:
        throw Exception(__FILE__, __LINE__, "Error %d in fork(): %s\n",
                             errno, strerror(errno));
    case 0:
        execl(prg.c_str(), prg_name.c_str(), real_path.c_str(),
              id.c_str(), NULL);
        exit(errno);
    }

    // wait for child end
    if ((waitpid(pid, &status, 0) == pid) < 0)
    {
        throw Exception(__FILE__, __LINE__, "Error %d in waitpid(): %s",
                        errno, strerror(errno));
    }

    if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status) == 0)
            return true;
    }

    return false;
}
