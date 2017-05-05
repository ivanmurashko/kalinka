/**
   @file dbscriptlauncher.cpp
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
   - 2008/10/14 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <errno.h>

#include <cppunit/extensions/HelperMacros.h>

#include <fstream>

#include "dbscriptlauncher.h"
#include "paths.h"
#include "utils.h"
#include "log.h"
#include "exception.h"

using namespace klk;
using namespace klk::test;

// Constructor
DBScriptLauncher::DBScriptLauncher(IConfig* config) :
    m_config(config)
{
    CPPUNIT_ASSERT(m_config != NULL);
}


// Destructor
DBScriptLauncher::~DBScriptLauncher()
{
}

// Recreates DB
void DBScriptLauncher::recreate()
{
    // main data
    executeScript("server.sql");

    // modules
    executeScript("checkdb.sql");
    executeScript("adapter.sql");
    executeScript("network.sql");
    executeScript("file.sql");
    executeScript("httpsrc.sql");
    executeScript("ieee1394.sql");

    // Linux specific modules
#ifdef LINUX
    executeScript("dvb.sql");
#endif //LINUX

    // applications
    executeScript("transcode.sql");
    executeScript("http-streamer.sql");

    // Linux specific applications
#ifdef LINUX
    executeScript("dvb-streamer.sql");
#endif //LINUX

}

// Executes a SQL script
// @param[in] script - the script to be executed
void DBScriptLauncher::executeScript(const std::string& script)
{
    CPPUNIT_ASSERT(m_config != NULL);
    std::string path = dir::SQL + "/" + script;
    CPPUNIT_ASSERT(base::Utils::fileExist(path.c_str()));

    // should be root
    CPPUNIT_ASSERT(m_config->getDBInfo()->getUserName() == "root");

    std::string host = "-h" + m_config->getDBInfo()->getHost();
    std::string user = "-u" + m_config->getDBInfo()->getUserName();
    std::string pwd = "-p" + m_config->getDBInfo()->getUserPwd();
    std::string db = m_config->getDBInfo()->getDBName();
    std::string exec = "< " + path;

    // create shell script
    std::string shell_script_fn = "/tmp/klkscriptlauncher.sh";
    base::Utils::unlink(shell_script_fn.c_str());
    std::ofstream shell_script_file(shell_script_fn.c_str(), std::ios::out);

    shell_script_file << "#!/bin/sh\n" << dir::MYSQL_UTIL << " " <<
        host << " " << user << " " << pwd << " " << db << " < " <<
        path << " > /dev/null || exit 1\n" <<
        "wait && exit 0";

    shell_script_file.close();

    base::Utils::runScript(base::Utils::SHELL, shell_script_fn, "");
    base::Utils::unlink(shell_script_fn.c_str());
}
