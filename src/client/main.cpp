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
   - 2007/Jun/30 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>

#include "version.h"
#include "clientfactory.h"
#include "exception.h"

#define CLIENT_TITLE "CLI client for Kalinka mediaserver"

// Prints the usage help
static void usage()
{
    std::cerr << CLIENT_TITLE << "\n" <<
        klk::VERSION_STR << "\n" << klk::COPYRIGHT_STR << "\n";
    std::cerr << klk::USAGE_STR << "\n";
}

// Starts the client
static void start_client()
{
    klk::cli::Factory* factory = klk::cli::Factory::instance();
    BOOST_ASSERT(factory);

    klk::cli::Client* client = factory->getClient();
    client->start();
}

// Stops the client
static void stop_client()
{
    klk::cli::Factory* factory = klk::cli::Factory::instance();
    BOOST_ASSERT(factory);

    klk::cli::Client* client = factory->getClient();
    client->stop();
}

// Signals handler
static void termh(int x)
{
    stop_client();
}

// Sets handlers
static void setsignals()
{
    struct sigaction sigterm;
    struct sigaction sigint;

    sigint.sa_handler = termh;
    sigterm.sa_handler = termh;

    sigint.sa_flags = 0;
    sigterm.sa_flags = 0;

    sigemptyset(&sigint.sa_mask);
    sigemptyset(&sigterm.sa_mask);

    sigaction(SIGINT, &sigint, NULL);
    sigaction(SIGTERM, &sigterm, NULL);
}

// starts the client
static void start()
{
    setsignals();
    start_client();
}


// Function main
int main(int argc, char *argv[])
{
    //extern char *optarg;
    char chOpt;

    // Command line
    while ((chOpt = getopt(argc, argv, "h?")) != -1)
    {
	switch (chOpt)
	{
	case '?':
	case 'h':
	default:
	    usage();
	    return(1);
	}
    }

    try
    {
        start();

        stop_client();
    }
    catch(const std::exception& err)
    {
        std::cerr << "\n\nGot error: " << err.what() << "\n"
                  << std::endl;
        klk::cli::Factory::destroy();

        exit(EXIT_FAILURE);
    }

    // free resources
    klk::cli::Factory::destroy();

    return 0;
}

