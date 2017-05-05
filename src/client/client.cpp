/**
   @file client.cpp
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
   - 2007/06/30 created by ipp (Ivan Murashko)
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
#include <sys/types.h>
#include <unistd.h>

#include <memory>

#include "clientfactory.h"
#include "client.h"
#include "clidriver.h"
#include "paths.h"
#include "log.h"
#include "utils.h"
#include "adapter/messagesprotocol.h"
#include "version.h"
#include "exception.h"

using namespace klk;
using namespace klk::cli;

int continuation;
EditLine *el;

// gets client
Client* get_client()
{
    Factory *factory =
        Factory::instance();
    KLKASSERT(factory != NULL);
    Client *client = factory->getClient();
    KLKASSERT(client != NULL);

    return client;
}

// Version handling
static int handle_version(int fd, int argc, char *argv[],
                          const char*, const char*, int)
{
    ast_cli(fd, "%s\n", VERSION_STR.c_str());
    return 0;
}

#if 0
/// test complete
char* test_complete(char *line, char *word, int pos, int state)
{
    char *ret = NULL;
    int which = 0;
    int wordlen;

    /* we are do completion of [exten@]context on second position only */
    if (pos != 2 && pos != 3)
        return NULL;

    wordlen = strlen(word);

    for (int i = 0; i < 2; i++)
    {
        std::string testword = "aaaaaa";
        if (i != 0)
        {
            testword = "bbbbbb";
        }

        if (pos == 2)
        {
            testword += "1";
        }
        else if (pos == 3)
        {
            testword += "2";
        }


        if (!strncasecmp(word, testword.c_str(), wordlen) &&
            ++which > state)
        {
            ret = strdup(testword.c_str());
            break;
        }
    }

    if (ret)
        klk_log(KLKLOG_DEBUG, "Word: '%s'. Result: '%s'", word, ret);
    else
        klk_log(KLKLOG_DEBUG, "Word: '%s'. Result: NULL", word);

    return ret;
}
#endif

// Processes the start command
static int handle_start(int fd, int argc, char *argv[],
                        const char* command_name,
                        const char* msgid,
                        int modnameused)
{
    try
    {
        get_client()->handleStart(fd, argc, argv);
    }
    catch(const std::exception& err)
    {
        ast_cli(fd, "\nAdditional info about error:\n%s\n",
                err.what());
        return -1;
    }
    catch(...)
    {
        KLKASSERT(0);
        return -1;
    }

    return 0;
}

// Processes the stop command
static int handle_stop(int fd, int argc, char *argv[],
                       const char* command_name,
                       const char* msgid,
                       int modnameused)
{
    try
    {
        get_client()->handleStop(fd, argc, argv);
    }
    catch(const std::exception& err)
    {
        ast_cli(fd, "\nAdditional info about error:\n%s\n",
                err.what());
        return -1;
    }
    catch(...)
    {
        KLKASSERT(0);
        return -1;
    }

    return 0;
}

static struct ast_cli_entry core_cli[] =
{
    { { "stop", NULL},
      handle_stop,
      "Stops the mediaserver",
      "Usage: system stop\n"
      "       stops the server\n", NULL, NULL, 0,
      NULL, NULL, 0
    },
    { { "start", NULL },
      handle_start,
      "Starts the mediaserver",
      "Usage: system start \n"
      "       starts the server\n", NULL, NULL, 0,
      NULL, NULL, 0
    },
    {
        { "version", NULL },
        handle_version,
        "Display version info",
        "Usage: version\n"
        "       Shows version information.\n",NULL, NULL, 0,
        NULL,NULL, 0
    },
    {
        {NULL}, NULL, NULL,
        NULL,
        NULL, NULL, 0, NULL, NULL, 0
    }
};

//
// Client
//

// Constructor
// @param[in] factory factory
Client::Client(IFactory* factory) :
    m_factory(factory), m_tok(NULL), m_hist(NULL)
{
}

// Destructor
Client::~Client()
{
}

// Starts the application
void Client::start()
{
    // load config
    m_factory->getConfig()->load();

    // init CLI
    HistEvent ev;

    continuation = 0;
    el = NULL;

    /* Init the builtin history */
    m_hist = history_init();
    /* Initialize the tokenizer */
    m_tok  = tok_init(NULL);
    /* Set the size of the history */
    history(m_hist, &ev, H_SETSIZE, 800);
    /* Initialize editline */
    el = ast_el_init(m_hist, "client module", stdin, stdout, stderr);
    ast_cli_register_multiple(core_cli,
                              sizeof(core_cli)/sizeof(core_cli[0]));

    // module specific
    Factory::instance()->getModuleProcessor()->update();

    /* main loop */
    ast_cli_worker(el, m_hist, &ev, m_tok);
}

// Stops the application
void Client::stop()
{
    el_end(el);
    tok_end(m_tok);
    history_end(m_hist);
}

// Processes the start command
void Client::handleStart(int fd, int argc, char *argv[])
{
    if (argc > 2)
    {
        assert(0);
    }

    if (testConnection() == OK)
    {
        ast_cli(fd, "Mediaserver is already running\n");
        throw Exception(__FILE__, __LINE__,
                             "Mediaserver is already running");
    }

    // start the mediaserver
    try
    {
        base::Utils::runScript(base::Utils::SHELL,
                          "etc/init.d/klk", "start");
    }
    catch(const std::exception&)
    {
        ast_cli(fd, "The mediaserver startup has been failed, "
                "for more info see logs\n");
        throw;
    }

    // FIXME!!!
    // allow server adapter module to be started
    sleep(2);
    if (testConnection() != OK)
    {
        ast_cli(fd, "Mediaserver does not respond\n");
        ast_cli(fd, "Mediaserver startup failed\n");
        throw Exception(__FILE__, __LINE__, "startup failed");
    }

    ast_cli(fd, "Mediaserver has started\n");

    // reload module specific info
    Factory::instance()->getModuleProcessor()->update();
}

// Processes the stop command
void Client::handleStop(int fd, int argc, char *argv[])
{
    if (argc > 2)
    {
        assert(0);
    }

    if (testConnection() != OK)
    {
        ast_cli(fd, "Mediaserver is not running\n");
        throw Exception(__FILE__, __LINE__, "not running");
    }

    // stop the mediaserver
    // start the mediaserver
    try
    {
        base::Utils::runScript(base::Utils::SHELL,
                          "etc/init.d/klk", "stop");
    }
    catch(const std::exception&)
    {
        ast_cli(fd, "The mediaserver stop has been failed, "
                "for more info see logs\n");
        throw;
    }

    // FIXME!!!
    // allow server adapter module to be stopped
    sleep(2);
    // check that connection is not present
    if (testConnection() != ERROR)
    {
        ast_cli(fd,
                "The mediaserver stop has been failed, "
                "for more info see logs\n");
        throw Exception(__FILE__, __LINE__, "Stop failed");
    }

    // reload module specific info
    Factory::instance()->getModuleProcessor()->update();

    ast_cli(fd, "The mediaserver has stopped\n");
}

// Tests connection
Result Client::testConnection()
try
{
    adapter::MessagesProtocol proto(m_factory);
    return proto.checkConnection();
}
catch(...)
{
    // no connection here
    return klk::ERROR;
}

