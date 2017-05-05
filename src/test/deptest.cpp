/**
   @file deptest.cpp
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
   - 2007/Apr/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "deptest.h"
#include "log.h"
#include "testutils.h"

using namespace klk;
using namespace klk::mod;

//
// ModDepTest class
//

// Constructor
void DepTest::setUp()
{
    m_dep.clear();

    /*
     *               t1
     *                |
     *            t2-- ---t3----
     *            |       |    |
     *            t4     t5    t6
     *                         |
     *                         t7
     */
    Result rc = m_dep.addDependency("t1", "t3");
    CPPUNIT_ASSERT(rc == OK);
    rc = m_dep.addDependency("t3", "t5");
    CPPUNIT_ASSERT(rc == OK);
    rc = m_dep.addDependency("t2", "t4");
    CPPUNIT_ASSERT(rc == OK);
    rc = m_dep.addDependency("t1", "t2");
    CPPUNIT_ASSERT(rc == OK);
    rc = m_dep.addDependency("t6", "t7");
    CPPUNIT_ASSERT(rc == OK);
    rc = m_dep.addDependency("t3", "t6");
    CPPUNIT_ASSERT(rc == OK);

}

// Destructor
void DepTest::tearDown()
{
    m_dep.clear();
}

// Do the test of dependency creation
void DepTest::checkDep()
{
    test::printOut("\nModules dependency test 1 ... ");

    /*
     *               t1
     *                |
     *            t2-- ---t3----
     *            |       |    |
     *            t4     t5    t6
     *                         |
     *                         t7
     */

    StringList dep1 = m_dep.getDependency("t1");
    CPPUNIT_ASSERT(dep1.size() == 2);
    CPPUNIT_ASSERT(std::find(dep1.begin(), dep1.end(), "t2") != dep1.end());
    CPPUNIT_ASSERT(std::find(dep1.begin(), dep1.end(), "t3") != dep1.end());

    StringList dep2 = m_dep.getDependency("t3");
    CPPUNIT_ASSERT(dep2.size() == 2);
    CPPUNIT_ASSERT(std::find(dep2.begin(), dep2.end(), "t5") != dep2.end());
    CPPUNIT_ASSERT(std::find(dep2.begin(), dep2.end(), "t6") != dep2.end());

    // remove one edge
    m_dep.rmDependency("t3", "t6");
    StringList dep3 = m_dep.getDependency("t3");
    CPPUNIT_ASSERT(dep3.size() == 1);
    CPPUNIT_ASSERT(std::find(dep3.begin(), dep3.end(), "t5") != dep3.end());
    CPPUNIT_ASSERT(std::find(dep3.begin(), dep3.end(), "t6") == dep3.end());
    // restore it
    Result rc = m_dep.addDependency("t3", "t6");
    CPPUNIT_ASSERT(rc == OK);

    // add existent
    rc = m_dep.addDependency("t3", "t5");
    CPPUNIT_ASSERT(rc == OK);
    StringList dep4 = m_dep.getDependency("t3");
    CPPUNIT_ASSERT(dep4.size() == 2);
    CPPUNIT_ASSERT(std::find(dep4.begin(), dep4.end(), "t5") != dep4.end());
    CPPUNIT_ASSERT(std::find(dep4.begin(), dep4.end(), "t6") != dep4.end());

    // add self
    rc = m_dep.addDependency("t3", "t3");
    CPPUNIT_ASSERT(rc == OK);
    StringList dep5 = m_dep.getDependency("t3");
    CPPUNIT_ASSERT(dep5.size() == 2);
    CPPUNIT_ASSERT(std::find(dep5.begin(), dep5.end(), "t5") != dep5.end());
    CPPUNIT_ASSERT(std::find(dep5.begin(), dep5.end(), "t6") != dep5.end());


    // wrong dependency - cicle
    rc = m_dep.addDependency("t7", "t3");
    CPPUNIT_ASSERT(rc == ERROR);

    // ok dep for directed graph
    rc = m_dep.addDependency("t7", "t5");
    CPPUNIT_ASSERT(rc == OK);
    // restore
    m_dep.rmDependency("t7", "t5");
}

// Do the test of sorted list retriving
void DepTest::checkSort()
{
    test::printOut("\nModules dependency test 2 ... ");

    // get sorted list
    /*
     *               t1
     *                |
     *            t2-- ---t3----
     *            |       |    |
     *            t4     t5    t6
     *                         |
     *                         t7
     */
    StringList sorted = m_dep.getSortedList();
    CPPUNIT_ASSERT(sorted.size() == 7);
    StringList result;
    for (StringList::iterator i = sorted.begin(); i != sorted.end(); i++)
    {
        if (*i == "t1")
        {
            CPPUNIT_ASSERT(result.empty());
        }
        else if (*i == "t2")
        {
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t1") !=
                           result.end());
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t4") ==
                           result.end());
        }
        else if (*i == "t3")
        {
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t1") !=
                           result.end());
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t5") ==
                           result.end());
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t6") ==
                           result.end());
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t7") ==
                           result.end());
        }
        else if (*i == "t4")
        {
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t2") !=
                           result.end());
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t7") ==
                           result.end());
        }
        else if (*i == "t5")
        {
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t3") !=
                           result.end());
        }
        else if (*i == "t6")
        {

            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t3") !=
                           result.end());
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t7") ==
                           result.end());
        }
        else if (*i == "t7")
        {
            CPPUNIT_ASSERT(std::find(result.begin(), result.end(), "t6") !=
                           result.end());
        }
        result.push_back(*i);
    }

    CPPUNIT_ASSERT(sorted == result);
}
