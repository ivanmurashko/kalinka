/**
   @file main.cpp
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
   - 2007/Mar/17 created by ipp
   - 2008/Sep/13 converted to cppunit usage
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

#include "testfactory.h"
#include "common.h"
#include "log.h"

// gets the test factory
klk::test::Factory* get_factory()
{
    klk::test::Factory* factory = klk::test::Factory::instance();
    assert(factory != NULL);

    return factory;
}

// prints the usage string
void usage()
{
    klk::test::Factory* factory = get_factory();
    assert(factory != NULL);
    std::string ids = factory->getTestIDs();
    std::string usage = "USAGE:\ntest -t <test_name>\ntest";

    klk_log(KLKLOG_TERMINAL, "%s\n", usage.c_str());
    klk_log(KLKLOG_TERMINAL,
            "\twhere <test_name> a module id or one from the following list:\n\t\t%s\n",

            ids.c_str());
    factory->destroy();

    exit(EXIT_FAILURE);
}

// Runs the unit tests
static void run_test(const char* test_id)
{
    CPPUNIT_NS::stdCOut() << "Running " << test_id << "\n";
    CppUnit::TextTestRunner runner;
    CppUnit::TestFactoryRegistry &registry =
        CppUnit::TestFactoryRegistry::getRegistry(test_id);

    CppUnit::Test *test = registry.makeTest();
    runner.addTest(test);

    runner.run("", false, true, false);
    CPPUNIT_NS::stdCOut() << "\n";
}


// main() function for the test
int main(int argc, char *argv[])
{
    extern char *optarg;
    char opt = '\x0';
    const char* test_id = NULL;

    const char* opt_args = "t:h?";

    klk::Result rc = klk::OK;

    // we should get factory as the first step to using logs
    klk::test::Factory* factory = get_factory();
    assert(factory != NULL);

    if (argc == 1)
    {
        usage();
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, opt_args)) != -1)
    {
	switch (opt)
	{
        case 't':
            test_id = optarg;
            break;
        case '?':
        case 'h':
        default:
            usage();
            exit(EXIT_FAILURE);
	}
    }

    if (rc == klk::OK)
    {
        KLKASSERT(test_id != NULL);
        run_test(test_id);
    }

    factory->destroy();

    if (rc != klk::OK)
        return -1;

    return 0;
}
