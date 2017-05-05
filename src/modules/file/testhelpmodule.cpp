/**
   @file testhelpmodule.cpp
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
   - 2008/03/25 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include "testhelpmodule.h"
#include "defines.h"
#include "testdefines.h"
#include "exception.h"
#include "messages.h"
#include "utils.h"
#include "testutils.h"
#include "db.h"

using namespace klk;
using namespace klk::file;

//
// TestHelpModule class
//

// Constructor
TestHelpModule::TestHelpModule(IFactory *factory) :
    Module(factory, TEST_MOD_ID), m_rc(OK)
{
}


// Process an async message
// @param[in] msg the message to be processed
void TestHelpModule::processASync(const IMessagePtr& msg)

{
    m_rc = OK;
    try
    {
        BOOST_ASSERT(msg->getID() == TEST_MSG_ID);
        // do inital test
        doTest();
        // restart NET module
        getFactory()->getModuleFactory()->unload(MODID);
        getFactory()->getModuleFactory()->load(MODID);
        // wait unitil db info will be read
        // FIXME!!!
        sleep(2);
        // do the rest test
        doTestCleanup();
        // do the folder test
        doTestFolder();
    }
    catch(const std::exception& err)
    {
            test::printOut(std::string("\nException: ")  + err.what() + "\n");
            m_rc = ERROR;
    }
    catch(...)
    {
        test::printOut("\nUnknown exception\n");
        m_rc = ERROR;
    }
}

// Do the test
void TestHelpModule::doTest()
{
    IMessageFactory* msgfactory = getFactory()->getMessageFactory();
    BOOST_ASSERT(msgfactory);

    klk_log(KLKLOG_DEBUG, "Start file test 1");
    IMessagePtr req = msgfactory->getMessage(msg::id::FILESTART);
    BOOST_ASSERT(req);
    req->setData(msg::key::MODINFOUUID, TESTUUID1);
    IMessagePtr res1;
    sendSyncMessage(req, res1);
    // test response
    BOOST_ASSERT(res1);
    // no mode field
    BOOST_ASSERT(res1->getValue(msg::key::STATUS) == msg::key::FAILED);

    req = msgfactory->getMessage(msg::id::FILESTART);
    BOOST_ASSERT(req);
    req->setData(msg::key::MODINFOUUID, TESTUUID1);
    req->setData(msg::key::FILEMODE, msg::key::FILEWRITE);
    res1.reset();
    sendSyncMessage(req, res1);
    // test response
    BOOST_ASSERT(res1);
    BOOST_ASSERT(res1->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(res1->getValue(msg::key::FILEPATH) == TESTPATH1);
    BOOST_ASSERT(res1->getValue(msg::key::MODINFONAME) == TESTNAME1);

    IMessagePtr req_del = msgfactory->getMessage(msg::id::FILESTOP);
    BOOST_ASSERT(req_del);
    res1.reset();
    req_del->setData(msg::key::MODINFOUUID, TESTUUID1);
    sendSyncMessage(req_del, res1);
    // test response
    BOOST_ASSERT(res1);
    BOOST_ASSERT(res1->getValue(msg::key::STATUS) == msg::key::OK);

    base::Utils::unlink(TESTPATH1);

    req = msgfactory->getMessage(msg::id::FILESTART);
    BOOST_ASSERT(req);
    req->setData(msg::key::MODINFOUUID, TESTUUID1);
    req->setData(msg::key::FILEMODE, msg::key::FILEREAD);
    res1.reset();
    sendSyncMessage(req, res1);
    // test response
    BOOST_ASSERT(res1);
    // no such file
    BOOST_ASSERT(res1->getValue(msg::key::STATUS) == msg::key::FAILED);

    // create the file
    BinaryData data("test");
    base::Utils::saveData2File(TESTPATH1, data);

    req = msgfactory->getMessage(msg::id::FILESTART);
    BOOST_ASSERT(req);
    req->setData(msg::key::MODINFOUUID, TESTUUID1);
    req->setData(msg::key::FILEMODE, msg::key::FILEREAD);
    res1.reset();
    sendSyncMessage(req, res1);
    // test response
    BOOST_ASSERT(res1);
    BOOST_ASSERT(res1->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(res1->getValue(msg::key::FILEPATH) == TESTPATH1);
    BOOST_ASSERT(res1->getValue(msg::key::MODINFONAME) == TESTNAME1);


    klk_log(KLKLOG_DEBUG, "Start file test 2");
    IMessagePtr res2;
    req->setData(msg::key::MODINFOUUID, TESTUUID2);
    req->setData(msg::key::FILEMODE, msg::key::FILEWRITE);

    sendSyncMessage(req, res2);
    // test response
    BOOST_ASSERT(res2);
    BOOST_ASSERT(res2->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(res2->getValue(msg::key::FILEPATH) == TESTPATH2);
    BOOST_ASSERT(res2->getValue(msg::key::MODINFONAME) == TESTNAME2);


    klk_log(KLKLOG_DEBUG, "Start file test 3");

    // wrong request
    IMessagePtr res3;
    req->setData(msg::key::MODINFOUUID,
                 "2c12b8ff-ce5f-4eb3-9ffa-658c1612d02d");
    sendSyncMessage(req, res3);
    // test response
    BOOST_ASSERT(res3);
    BOOST_ASSERT(res3->getValue(msg::key::STATUS) == msg::key::FAILED);

    // wrong request
    IMessagePtr res4;
    req->setData(msg::key::MODINFOUUID, TESTUUID1);
    sendSyncMessage(req, res4);
    // test response
    BOOST_ASSERT(res4);
    BOOST_ASSERT(res4->getValue(msg::key::STATUS) == msg::key::FAILED);

    // del it
    IMessagePtr res5;
    req_del = msgfactory->getMessage(msg::id::FILESTOP);
    BOOST_ASSERT(req_del);
    req_del->setData(msg::key::MODINFOUUID, TESTUUID1);
    sendSyncMessage(req_del, res5);
    // test response
    BOOST_ASSERT(res5);
    BOOST_ASSERT(res5->getValue(msg::key::STATUS) == msg::key::OK);

    // NOW WE CAN ADD IT AGAIN
    IMessagePtr res6;
    req->setData(msg::key::MODINFOUUID, TESTUUID1);
    sendSyncMessage(req, res6);
    // test response
    BOOST_ASSERT(res6);
    BOOST_ASSERT(res6->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(res6->getValue(msg::key::FILEPATH) == TESTPATH1);
    BOOST_ASSERT(res6->getValue(msg::key::MODINFONAME) == TESTNAME1);
}

// Do the cleanup test
void TestHelpModule::doTestCleanup()
{
    klk_log(KLKLOG_DEBUG, "Start file test 4 (after restart)");

    IMessageFactory* msgfactory = getFactory()->getMessageFactory();
    BOOST_ASSERT(msgfactory);

    IMessagePtr req = msgfactory->getMessage(msg::id::FILESTART);
    BOOST_ASSERT(req);
    IMessagePtr res;
    req->setData(msg::key::MODINFOUUID, TESTUUID2);
    req->setData(msg::key::FILEMODE, msg::key::FILEWRITE);
    sendSyncMessage(req, res);
    // test response
    BOOST_ASSERT(res);
    BOOST_ASSERT(res->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(res->getValue(msg::key::FILEPATH) == TESTPATH2);
    BOOST_ASSERT(res->getValue(msg::key::MODINFONAME) == TESTNAME2);
}

// Register all processors
void TestHelpModule::registerProcessors()
{
    registerASync(
        TEST_MSG_ID,
        boost::bind(&TestHelpModule::processASync, this, _1));
}

// Do the folder test
void TestHelpModule::doTestFolder()
{
    // delete the container folder
    base::Utils::unlink(TESTPATH3);

    // should not be any children here
    TestInfoList children = getChildren(TESTUUID3);
    BOOST_ASSERT(children.empty());

    IMessageFactory* msgfactory = getFactory()->getMessageFactory();
    BOOST_ASSERT(msgfactory);

    IMessagePtr req = msgfactory->getMessage(msg::id::FILESTART);
    BOOST_ASSERT(req);
    IMessagePtr res;
    req->setData(msg::key::MODINFOUUID, TESTUUID3);
    req->setData(msg::key::FILEMODE, msg::key::FILEWRITE);
    sendSyncMessage(req, res);
    // test response
    BOOST_ASSERT(res);
    BOOST_ASSERT(res->getValue(msg::key::STATUS) == msg::key::OK);
    std::string path1 = res->getValue(msg::key::FILEPATH);
    // there should be an unique path
    BOOST_ASSERT(path1 != TESTPATH3);
    // but should contain 3d path as a part of it
    BOOST_ASSERT(strncmp(path1.c_str(), TESTPATH3.c_str(), TESTPATH3.size()) == 0);

    // the containing folder should exist
    boost::filesystem::path test_path1(path1);
    BOOST_ASSERT(boost::filesystem::exists(test_path1.branch_path()) == true);
    // one child here
    children = getChildren(TESTUUID3);
    BOOST_ASSERT(children.size() == 1);
    TestInfoList::iterator child = std::find(children.begin(), children.end(), path1);
    BOOST_ASSERT( child != children.end());
    // start time should be set
    BOOST_ASSERT( child->getStart() > 0);
    BOOST_ASSERT( child->getStart() > time(NULL) - 10);
    BOOST_ASSERT( child->getStart() <= time(NULL));
    // end time should not be set
    // the item is active now
    BOOST_ASSERT( child->getStop() == 0);

    // cleanup
    res.reset();
    IMessagePtr req_del, res_del;
    req_del = msgfactory->getMessage(msg::id::FILESTOP);
    BOOST_ASSERT(req_del);
    req_del->setData(msg::key::MODINFOUUID, TESTUUID3);
    sendSyncMessage(req_del, res_del);
    // test response
    BOOST_ASSERT(res_del);
    BOOST_ASSERT(res_del->getValue(msg::key::STATUS) == msg::key::OK);

    // try to retrive new path
    sendSyncMessage(req, res);
    // test response
    BOOST_ASSERT(res);
    BOOST_ASSERT(res->getValue(msg::key::STATUS) == msg::key::OK);
    std::string path2 = res->getValue(msg::key::FILEPATH);
    // should not be qual to path1
    BOOST_ASSERT(path1 != path2);
    // there should be an unique path
    BOOST_ASSERT(path2 != TESTPATH3);
    // but should contain 3d path as a part of it
    BOOST_ASSERT(strncmp(path2.c_str(), TESTPATH3.c_str(),
                          TESTPATH3.size()) == 0);

    // the containing folder should exist
    boost::filesystem::path test_path2(path2);
    BOOST_ASSERT(boost::filesystem::exists(test_path2.branch_path()) == true);

    // two children now
    children = getChildren(TESTUUID3);
    BOOST_ASSERT(children.size() == 2);
    // both path1 and path2 should be included
    TestInfoList::iterator child1 = std::find(children.begin(), children.end(), path1);
    BOOST_ASSERT( child1 != children.end());
    TestInfoList::iterator child2 = std::find(children.begin(), children.end(), path2);
    BOOST_ASSERT( child2 != children.end());

    // start time should be set for bot
    BOOST_ASSERT( child1->getStart() > 0);
    BOOST_ASSERT( child1->getStart() > time(NULL) - 2);
    BOOST_ASSERT( child1->getStart() <= time(NULL));
    BOOST_ASSERT( child2->getStart() > 0);
    BOOST_ASSERT( child2->getStart() > time(NULL) - 2);
    BOOST_ASSERT( child2->getStart() <= time(NULL));
    // end time should be set for first child only
    BOOST_ASSERT( child1->getStop() > 0);
    BOOST_ASSERT( child1->getStop() > time(NULL) - 2);
    BOOST_ASSERT( child1->getStop() <= time(NULL));
    BOOST_ASSERT( child1->getStop() <= child2->getStart());
    BOOST_ASSERT( child2->getStop() == 0);
}

// Retrives a list with paths for the test folder
const TestHelpModule::TestInfoList
TestHelpModule::getChildren(const std::string& uuid)
{
    db::DB db(getFactory());
    db.connect();
    //CREATE PROCEDURE `klk_file_test_get_children` (
    // IN parent_uuid VARCHAR(40)
    db::Parameters params;
    params.add("@parent_uuid", uuid);
    db::ResultVector rv = db.callSelect("klk_file_test_get_children",
                                        params, NULL);

    TestInfoList list;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT klk_file.file_path AS file_path,
	// klk_file.start_time AS start_time,
	// klk_file.end_time AS end_time
        const std::string path = (*i)["file_path"].toString();
        time_t start_time = 0;
        if (!(*i)["start_time"].isNull())
            start_time = (*i)["start_time"].toInt();
        time_t end_time = 0;
        if (!(*i)["end_time"].isNull())
            end_time = (*i)["end_time"].toInt();
        list.push_back(TestInfo(path, start_time, end_time));
    }

    return list;
}
