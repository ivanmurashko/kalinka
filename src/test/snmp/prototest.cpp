/**
   @file prototest.cpp
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
   - 2009/05/30 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "prototest.h"
#include "exception.h"
#include "utils.h"
#include "testutils.h"

#include "snmp/protocol.h"
#include "snmp/table.h"
#include "snmp/scalar.h"

using namespace klk;
using namespace snmp;

/**
   Socket file for test
*/
const std::string SOCKTEST_NAME("/tmp/testsnmpproto");

//
// ProtocolTestThread class
//

// Constructor
ProtocolTestThread::ProtocolTestThread() :
    test::Thread(), m_listener()
{
}

// Destructor
ProtocolTestThread::~ProtocolTestThread()
{
}

// The main body of the thread
void ProtocolTestThread::mainLoop()
{
    BOOST_ASSERT(m_listener == NULL);

    sock::RouteInfo testroute(SOCKTEST_NAME, 0,
                              sock::UNIXDOMAIN, sock::UNICAST);

    m_listener = sock::Factory::getListener(testroute);
    BOOST_ASSERT(m_listener);
    while (!isStopped())
    {
        ISocketPtr sock = m_listener->accept();
        BOOST_ASSERT(sock);

        // we just an echo server
        Protocol proto(sock);
        IDataPtr req = proto.recvData();
        BOOST_ASSERT(req);
        IDataPtr res = req;
        // emulate processing
        sleep(2);
        proto.sendData(res);
    }
}

// stops the thread
void ProtocolTestThread::stop() throw()
{
    test::Thread::stop();
    if (m_listener)
    {
        m_listener->stop();
    }
}


//
// ProtocolTest class
//

// Constructor
ProtocolTest::ProtocolTest() :
    CppUnit::TestFixture (), m_scheduler()
{
}

// Constructor
void ProtocolTest::setUp()
{
    m_scheduler.clear();
    base::Utils::unlink(SOCKTEST_NAME);
}

// Destructor
void ProtocolTest::tearDown()
{
    m_scheduler.stop();
    m_scheduler.clear();
    base::Utils::unlink(SOCKTEST_NAME);
}

// Do the test of Protocol traps
void ProtocolTest::test()
{
    test::printOut("\nSNMP protocol test ... ");

    CPPUNIT_ASSERT(!base::Utils::fileExist(SOCKTEST_NAME));

    test::ThreadPtr thread(new ProtocolTestThread());
    m_scheduler.addTestThread(thread);
    m_scheduler.start();

    // be sure that it has been started
    sleep(3);

    CPPUNIT_ASSERT(base::Utils::fileExist(SOCKTEST_NAME));

    // different tests
    testScalar();
    testTable();
    testScalar();
    testTable();
    testTable();

    sleep(2);
    m_scheduler.stop();
    m_scheduler.checkResult();
}

// Tests a scalar
void ProtocolTest::testScalar()
{
    ScalarPtr initial(new Scalar());
    initial->setValue(1);
    Protocol proto(SOCKTEST_NAME);
    proto.sendData(initial);
    IDataPtr fin = proto.recvData();
    CPPUNIT_ASSERT(fin);
    ScalarPtr final = boost::dynamic_pointer_cast<Scalar, IData>(fin);
    CPPUNIT_ASSERT(final);
    CPPUNIT_ASSERT(final->getValue().toString() == "1");
    CPPUNIT_ASSERT(final->getValue().toInt() == 1);
}

// Tests a table
void ProtocolTest::testTable()
{
    // create test data
    TableRow row1;
    row1.push_back(StringWrapper("first"));
    row1.push_back(StringWrapper(1));
    TableRow row2;
    row2.push_back(StringWrapper("second"));
    row2.push_back(StringWrapper(2));

    TablePtr table(new Table());
    table->addRow(row1);
    table->addRow(row2);

    Protocol proto(SOCKTEST_NAME);
    proto.sendData(table);
    IDataPtr fin = proto.recvData();
    CPPUNIT_ASSERT(fin);
    TablePtr final = boost::dynamic_pointer_cast<Table, IData>(fin);

    TableRowContainer data = final->getData();

    CPPUNIT_ASSERT(data.size() == 2);
    bool was1 = false, was2 = false;
    for (TableRowContainer::iterator i = data.begin(); i != data.end(); i++)
    {
        TableRow row = *i;
        if (was1 == false)
        {
            was1 = true;
            CPPUNIT_ASSERT(was2 == false);
            CPPUNIT_ASSERT(row[0].toString() == row1[0].toString());
            CPPUNIT_ASSERT(row[1].toInt() == row1[1].toInt());
        }
        else if (was2 == false)
        {
            was2 = true;
            CPPUNIT_ASSERT(row[0].toString() == row2[0].toString());
            CPPUNIT_ASSERT(row[1].toInt() == row2[1].toInt());
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
    }

    CPPUNIT_ASSERT(was1);
    CPPUNIT_ASSERT(was2);
}
