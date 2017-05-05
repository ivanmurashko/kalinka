/**
   @file clitest.cpp
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
   - 2009/11/22 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clitest.h"
#include "clitable.h"

using namespace klk;
using namespace klk::cli;

//
// Test class
//

// Constructor
Test::Test()
{
}

// Destructor
Test::~Test()
{
}

// Sets up data for the utest
void Test::setUp()
{
}

// Clears utest data
void Test::tearDown()
{
}

// Tests klk::cli::Command::formatOutput
void Test::testFormat()
{
    /*
      first              second third
      description 2            third_description
     */

    Table table;

    StringList row1, row2;
    row1.push_back("first");
    row1.push_back("second");
    row1.push_back("third");

    table.addRow(row1);

    row2.push_back("description");
    row2.push_back("2");
    row2.push_back("third_description");

    table.addRow(row2);

    const std::string sample = ""
        "first       second third\n"
        "description 2      third_description\n";

    const std::string test = table.formatOutput();

#if 0
    std::cout << test << std::endl;
    std::cout << sample << std::endl;
#endif
    CPPUNIT_ASSERT(test == sample);
}
