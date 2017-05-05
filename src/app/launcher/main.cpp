/**
   @file main.cpp
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
   - 2010/07/11 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <execinfo.h>

#include <iostream>

#include <boost/assert.hpp>
#include <boost/program_options.hpp>

#include "exception.h"
#include "daemon.h"
#include "launchfactory.h"

using namespace klk::launcher;

//
// FIXME!!! a lot of dublicates with src/server/main.cpp
//

// Signals handler
static void termh(int x)
{
    std::cerr << "SIGTERM at launcher application detected" << std::endl;
    // it's safe to work with semaphores from the signal handlers
    Factory::instance()->getLauncher()->stop();
}

// SIGPIPE handler
static void pipeh(int x)
{
    // just look for them now
    KLKASSERT(0);
}

// SIGSEGV handler
// we prit stack into std::cerr
static void segvh(int sig)
{
    std::cerr << "SIGSEGV at launcher application detected" << std::endl;

    void *trace[16];
    char **messages = (char **)NULL;
    int i, trace_size = 0;

    trace_size = backtrace(trace, 16);
    messages = backtrace_symbols(trace, trace_size);

    for ( i=0; i< trace_size; ++i )
    {
        std::cerr << i << ": " << messages[i] << std::endl;
    }

    // terminate
    abort();
}

// Sets handlers
static void setsignals()
{
    // Signals should not be ignored
    sigset_t sigset;
    sigemptyset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset, NULL);

    struct sigaction sigterm;
    struct sigaction sigint;
    struct sigaction sigpipe;
    struct sigaction sigsegv;


    sigint.sa_handler = termh;
    sigterm.sa_handler = termh;
    sigpipe.sa_handler = pipeh;
    sigsegv.sa_handler = segvh;

    sigint.sa_flags = 0;
    sigterm.sa_flags = 0;
    sigpipe.sa_flags = 0;
    sigsegv.sa_flags = 0;

    sigemptyset(&sigint.sa_mask);
    sigemptyset(&sigterm.sa_mask);
    sigemptyset(&sigpipe.sa_mask);
    sigemptyset(&sigsegv.sa_mask);

    sigaction(SIGINT, &sigint, NULL);
    sigaction(SIGTERM, &sigterm, NULL);
    sigaction(SIGPIPE, &sigpipe, NULL);
    sigaction(SIGSEGV, &sigsegv, NULL);
}

// main
int main(int argc, char* argv[])
{
    int rc = 0;

    setsid();

    // signal handlers setup
    setsignals();

    try
    {
        using namespace boost::program_options;

        // command line options
        // Declare the supported options.
        options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("name,n", value<std::string>(),
             "application name to be launched")
            ("module,m", value<std::string>(),
             "application module id to be used")
            ("config,c", value<std::string>(),
             "config file to be used");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help") || !vm.count("name") ||
            !vm.count("module") ||!vm.count("config"))
        {
            std::cerr << desc << std::endl;
            return 1;
        }

        // initial preparation
        // transfer std::cout, std::cerr into custom files
        std::string appname = vm["name"].as<std::string>();
        klk::daemon::redefineStreams(appname);

        try
        {
            // first the factory initialization and load config
            // this is necessary because the application can be started from
            // unit tests that have custom config and this config should be used
            const std::string config = vm["config"].as<std::string>();
            std::string modid = vm["module"].as<std::string>();
            Factory::instance(appname, modid)->getConfig()->setPath(config);
            Factory::instance()->getConfig()->load();

            // start the application
            Factory::instance()->getLauncher()->start();
        }
        catch(std::exception& err)
        {
            std::cerr << "Exception: " << err.what() << std::endl;
            rc = 1;
        }
        // Free allocated resources
        Factory::destroy();

    }
    catch(std::exception& err)
    {
        std::cerr << "Exception: " << err.what() << std::endl;
        rc = 1;
    }
    return rc;
}
