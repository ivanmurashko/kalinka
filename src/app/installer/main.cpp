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
   - 2009/03/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <iostream>

#include "version.h"
#include "common.h"
#include "instfactory.h"
#include "exception.h"
#include "options.h"

/**
   Installer title
*/
const std::string CLIENT_TITLE("Installer application for Kalinka mediaserver");


// Prints the usage help
static void usage()
{
    std::cerr << CLIENT_TITLE << "\n" <<
        klk::VERSION_STR << "\n" << klk::COPYRIGHT_STR << "\nOptions:\n";
    std::cerr << "\t-i run in interactive mode\n";
    std::cerr << "\t-s [server name] sets the server name at the config\n";
    std::cerr << "\t-d [db address] sets the DB address at the config\n";
    std::cerr << "\t-r [SNMP receiver] sets the SNMP receiver\n";
    std::cerr << "\t-c [SNMP community] sets the SNMP communit\n";
    std::cerr << "\t-a [sets adapter host name]\n";
    std::cerr << "\t-w detect resources or not\n";
    std::cerr << "\t-h prints this text\n";
}

// Function main
int main(int argc, char *argv[])
{
    extern char *optarg;
    //extern char *optarg;
    char opt;

    try
    {
        klk::inst::Installer*
            inst = klk::inst::Factory::instance()->getInstaller();

        // Command line
        while ((opt = getopt(argc, argv, "ihws:d:r:c:a:?")) != -1)
        {
            switch (opt)
            {
            case 'i':
                inst->processInteractive();
                break;
            case 'w':
                inst->detectResources();
                break;
            case 's':
                inst->setOption(klk::conf::HOSTNAME, optarg);
                break;
            case 'd':
                inst->setOption(klk::conf::DBADDR, optarg);
                break;
            case 'r':
                inst->setOption(klk::conf::SNMPRECEIVER, optarg);
                break;
            case 'c':
                inst->setOption(klk::conf::SNMPCOMMUNITY, optarg);
                break;
            case 'h':
            default:
                usage();
	    return(1);
            }
        }
    }
    catch(const std::exception& err)
    {
        std::cerr << err.what() << "\n";
    }
    catch(...)
    {
        std::cerr << "Got unknown exception\n";
    }


    // free resources
    klk::inst::Factory::destroy();
    return 0;
}

