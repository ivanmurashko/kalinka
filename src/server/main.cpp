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
   - 2008/07/26 created by ipp (Ivan Murashko)
   - 2009/04/04 some refactoring was made by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h> // for fork
#include <unistd.h> // for fork
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <execinfo.h>

#include <iostream>
#include <string>

#include "serverfactory.h"
#include "version.h"
#include "common.h"
#include "log.h"
#include "utils.h"
#include "paths.h"
#include "exception.h"
#include "daemon.h"

/**
   Server title
*/
const std::string SERVER_TITLE("Kalinka mediaserver daemon");


// Prints the usage help
static void usage()
{
    std::cerr << SERVER_TITLE << "\n" <<
        klk::VERSION_STR << "\n" << klk::COPYRIGHT_STR << "\n";
    std::cerr << klk::USAGE_STR << "\n";
    std::cerr << "\t-D starts as a daemon" << std::endl;
}

// Starts the server
static void start_server()
{
    klk::server::Factory* factory = klk::server::Factory::instance();
    BOOST_ASSERT(factory);

    klk::server::ServerDaemon* server = factory->getServer();
    BOOST_ASSERT(server);

    server->start();
}

// Signals handler
static void termh(int x)
{
    // it's safe to work with semaphores from the signal handlers
    klk::server::Factory::instance()->getEventTrigger()->stop();
}

// SIGPIPE handler
static void pipeh(int x)
{
    // just look for them now
    KLKASSERT(0);
}

// SIGSEGV handler
// we print stack into std::cerr
static void segvh(int sig)
{
    std::cerr << "SIGSEGV detected" << std::endl;

    void *trace[16];
    char **messages = static_cast< char **> ( NULL );
    int i, trace_size = 0;

    trace_size = backtrace(trace, 16);
    messages = backtrace_symbols(trace, trace_size);

    for ( i=0; i< trace_size; ++i )
    {
        std::cerr << i << ": " << messages[i] << std::endl;
    }

    free( messages );

    // terminate
    abort();
}


// Sets handlers
static void setsignals()
{
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

// starts a child
static void start_child()
{
    setsignals();
    start_server();
}

// starts daemon
static void start_daemon()
{
    pid_t p = fork();
    if (p != 0)
    {
        // nothing to do for parent
        return;
    }

    klk::server::Factory* factory = klk::server::Factory::instance();
    if (factory == NULL)
    {
        KLKASSERT(factory != NULL);
        exit(EXIT_FAILURE);
    }

    try
    {
        /* Change the file mode mask */
        umask(0);

        /* Create a new SID for the child process */
        int sid = setsid();
        if (sid < 0)
        {
            throw klk::Exception(__FILE__, __LINE__,
                                 "Error %d in setsid(): %s",
                                 errno, strerror(errno));
        }

        /* Change the current working directory */
        if (chdir("/") < 0)
        {
            throw klk::Exception(__FILE__, __LINE__,
                                 "Error %d in chdir(): %s",
                                 errno, strerror(errno));
        }

        klk::daemon::redefineStreams("klkd");

	start_child();
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Daemon got an exception: %s",
                err.what());
        exit(EXIT_FAILURE);
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Daemon got an unknown exception");
        exit(EXIT_FAILURE);
    }
}

// Function main
int main(int argc, char *argv[])
{
    //extern char *optarg;
    char opt;
    bool daemon = false; // not start as daemon

    // init factory for logging
    klk::server::Factory* factory = klk::server::Factory::instance();
    KLKASSERT(factory != NULL);

    // Command line
    while ((opt = getopt(argc, argv, "Dh?")) != -1)
    {
	switch (opt)
	{
        case 'D':
            daemon = true;
            break;
	case '?':
	case 'h':
	default:
	    usage();
            exit(EXIT_FAILURE);
        }
    }

    try
    {
        if (daemon)
        {
            start_daemon();
        }
        else
        {
            start_child();
        }
    }
    catch(const std::exception& err)
    {
        std::cerr << "Daemon got an exception: " <<
            err.what() << std::endl;
        KLKASSERT(daemon == false);
    }
    catch(...)
    {
        std::cerr << "Daemon got an unknown exception" << std::endl;
        KLKASSERT(daemon == false);
    }

    return 0;
}

