/**
   @file moduleinfo.cpp
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
   - 2009/05/03 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <boost/bind.hpp>

#include "moduleinfo.h"
#include "clientfactory.h"
#include "exception.h"
#include "adapter/messagesprotocol.h"
#include "cliutils.h"

// Modules specific
#include "service/messages.h"
#include "service/defines.h"

using namespace klk;
using namespace klk::cli;

// Processes a module's specific command
static int handle_module(int fd, int argc, char *argv[],
                         const char* command_name,
                         const char* msgid,
                         int modnameused)
{
    try
    {
        Factory::instance()->getModuleProcessor()->handleModule(fd, argc, argv,
                                                                command_name,
                                                                msgid,
                                                                modnameused);
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

// Processes completion for a module command's parameter
static char* handle_complete(const char *line, const char *word,
                             int pos, int state,
                             const char* msgid)
{
    char *ret = NULL;

    try
    {
        BOOST_ASSERT(line);
        const std::string strline(line);
        BOOST_ASSERT(word);
        const std::string strword(word);

        ModuleProcessor *processor = Factory::instance()->getModuleProcessor();
        ParameterVector completions =
            processor->getCompletion(strline, strword, pos, msgid);
        if (completions.empty())
        {
            // no completions
            return NULL;
        }

        size_t wordlen = strlen(word);
        int which = 0;

        for (ParameterVector::iterator i = completions.begin();
             i != completions.end(); i++)
        {
            if (!strncasecmp(word, i->c_str(), wordlen) &&
                ++which > state)
            {
                std::string found = *i;
                // is there any spaces
                if (found.find(' ') != std::string::npos)
                {
                    found = "\"" + found + "\"";
                }
                ret = strdup(found.c_str());
                break;
            }
        }
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Error in handle_complete(): %s",
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Unknown error in handle_complete()");
    }

    return ret;
}

//
// ModuleInfo class
//

// Constructor
ModuleInfo::ModuleInfo(IFactory* factory, const IModulePtr& module) :
    m_factory(factory), m_module(module), m_entries(), m_is_loaded(false)
{
    BOOST_ASSERT(m_factory);
    BOOST_ASSERT(m_module);
}

// Destructor
ModuleInfo::~ModuleInfo()
{
}

// Updates the info
void ModuleInfo::update()
{
    // if first call we should update
    if (m_entries.empty())
    {
        registerModule();
    }

    // if load status was changed
    if (isLoaded() != m_is_loaded)
    {
        // clear previouse
        std::for_each(m_entries.begin(), m_entries.end(),
                      boost::bind(&Garbage::freeEntry,
                                  Factory::instance()->getGarbage(), _1));
        m_entries.clear();

        // register now
        registerModule();
    }
}

// Registers a module specific info at the driver
void ModuleInfo::registerModule()
{
    m_is_loaded = isLoaded();

    cli::ICommandList list = m_module->getCLIInfo();
    for (cli::ICommandList::iterator i = list.begin();
         i != list.end(); i++)
    {
        if (m_is_loaded == false &&
            (*i)->isRequireModule() == true)
        {
            // not load the command yet
            continue;
        }

        struct ast_cli_entry* entry =
            Factory::instance()->getGarbage()->allocEntry();
        entry->klkmodnameused = 1;

        ParameterVector cmds = Utils::getCommands((*i)->getName());
        BOOST_ASSERT(cmds.size() > 0 &&
                      cmds.size() < (AST_MAX_CMD_LEN - 2));

        // ignore module name for service module
        if (m_module->getID() == srv::MODID)
        {
            entry->klkmodnameused = 0;
            size_t j = 0;
            for (j = 0; j < AST_MAX_CMD_LEN - 1; j++)
            {
                if (j >= cmds.size())
                    break;
                entry->cmda[j] =
                    Factory::instance()->getGarbage()->allocData(cmds[j]);
            }
            entry->cmda[j] = NULL;
        }
        else
        {
            /*! Null terminated list of the words of the command */
            const std::string modname = m_module->getName();
            entry->cmda[0] =
                Factory::instance()->getGarbage()->allocData(modname);
            size_t j = 1;
            for (j = 1; j < AST_MAX_CMD_LEN - 1; j++)
            {
                if (j > cmds.size())
                    break;
                entry->cmda[j] =
                    Factory::instance()->getGarbage()->allocData(cmds[j - 1]);
            }
            entry->cmda[j] = NULL;
        }

        /*! Handler for the command (fd for output, # of arguments,
          argument list).
          Returns RESULT_SHOWUSAGE for improper arguments */
        entry->handler = handle_module;
        /*! Summary of the command (< 60 characters) */
        entry->summary =
            Factory::instance()->getGarbage()->allocData((*i)->getSummary());
        /*! Detailed usage information */
        entry->usage =
            Factory::instance()->getGarbage()->allocData((*i)->getUsage());
        /*! Generate a list of possible completions for a given word */
        //char *(*generator)(char *line, char *word, int pos, int state);
        entry->generator = handle_complete;
        /*! For linking */
        entry->next = NULL;
        /*! For keeping track of usage */
        entry->inuse = 0;

        // KLK specific data
        entry->klkname =
            Factory::instance()->getGarbage()->allocData((*i)->getName());
        entry->klkmsgid =
            Factory::instance()->getGarbage()->allocData((*i)->getMessageID());

        m_entries.push_back(entry);

        ast_cli_register(entry);
    }


#if 0
    // linking
    for (EntryList::iterator i = m_entries.begin(); i != m_entries.end(); i++)
    {
        // do linking
        EntryList::iterator next = i;
        next++;
        if (next != m_entries.end())
        {
            (*i)->next = *next;
        }
    }
#endif
}

// Checks is the module really loaded
const bool ModuleInfo::isLoaded() const
{
    bool isLoaded = false;
    // check status
    try
    {
        IMessagePtr in =
            m_factory->getMessageFactory()->getMessage(msg::id::SRVMODCHECK);
        BOOST_ASSERT(in);
        in->setData(msg::key::SRVMODID, m_module->getID());
        adapter::MessagesProtocol proto(m_factory);
        IMessagePtr out = proto.sendSync(in);
        BOOST_ASSERT(out);
        if (out->getValue(msg::key::STATUS) == msg::key::OK)
        {
            isLoaded = (out->getValue(msg::key::SRVMODSTATUS) ==
                        msg::key::OK);
        }
    }
    catch(...)
    {
        isLoaded = false;
    }

    return isLoaded;
}
