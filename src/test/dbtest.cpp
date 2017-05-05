/**
   @file dbtest.cpp
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
   - 2008/09/01 created by ipp (Ivan Murashko)
   - 2008/01/04 DB class interface was rewriten by ipp
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <memory>

#include "dbtest.h"
#include "common.h"
#include "testfactory.h"
#include "db.h"
#include "klkconfig.h"
#include "log.h"
#include "testid.h"
#include "paths.h"
#include "exception.h"
#include "testutils.h"

using namespace klk::db;

//
// DBTest class
//

// Constructor
void DBTest::setUp()
{
    m_factory = test::Factory::instance();
    CPPUNIT_ASSERT(m_factory != NULL);

    IConfig* config = m_factory->getConfig();
    CPPUNIT_ASSERT(config != NULL);
    config->load();

    m_launcher = new klk::test::DBScriptLauncher(config);
    CPPUNIT_ASSERT(m_launcher != NULL);
    m_launcher->recreate();
}

// Destructor
void DBTest::tearDown()
{
    m_launcher->recreate();
    KLKDELETE(m_launcher);
}

// Tests all DB
void DBTest::test()
{
    test::printOut("\nDB test ... ");

    CPPUNIT_ASSERT(m_factory != NULL);


    DB db(m_factory);
    db.connect();

    // clear
    const char *testhost = "db_unittest_host";
    Parameters params_delete;
    params_delete.add("@host", testhost);
    params_delete.add("@return", Parameters::null);
    dynamic_cast<klk::Config*>(m_factory->getConfig())->setHostName(testhost);
    // ignore result code here
    db::Result rv_del =
        db.callSimple("klk_host_delete", params_delete);

    // add the host
    Parameters params_add;
    params_add.add("@uuid", Parameters::null); // hostguid
    params_add.add("@host", testhost); // hostname
    params_add.add("@cpu_index_typ", 0); // cpu_index_typ
    params_add.add("@cpu_index_max", 0); // cpu_index_max
    params_add.add("@return", Parameters::null);
    db::Result rv_add =
        db.callSimple("klk_host_add", params_add);
    CPPUNIT_ASSERT(rv_add["@return"].toInt() == 0);

    // check the host
    Parameters params_select;
    ResultVector rv = db.callSelect("klk_host_list",
                                    params_select, NULL);
    CPPUNIT_ASSERT(rv.size() == 1);

    // SELECT host, host_name cpu_index_typ, cpu_index_max
    // FROM klk_hosts;
    const std::string db_uuid = rv[0]["host"].toString();
    const std::string db_host = rv[0]["host_name"].toString();
    CPPUNIT_ASSERT(db_host == testhost);

    // check the get uuid
    const std::string testuuid = db.getHostUUID();
    CPPUNIT_ASSERT(testuuid == db_uuid);

    // clear at the end
    db.callSimple("klk_host_delete", params_delete);

    db.disconnect();
}


