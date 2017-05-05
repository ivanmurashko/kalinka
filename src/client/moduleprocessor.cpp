/**
   @file moduleprocessor.cpp
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
   - 2009/05/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include "moduleprocessor.h"
#include "clientfactory.h"
#include "exception.h"
#include "adapter/messagesprotocol.h"
#include "cliutils.h"
#include "utils.h"

using namespace klk;
using namespace klk::cli;

//
// ModuleProcessor class
//

// Constructor
ModuleProcessor::ModuleProcessor(IFactory* factory) :
    m_factory(factory), m_list()
{
    BOOST_ASSERT(m_factory);

    // create list for the first time
    ModuleList modules =
        m_factory->getModuleFactory()->getModules();
    for (ModuleList::iterator i = modules.begin(); i != modules.end(); i++)
    {
        ModuleInfoPtr modinfo =
            ModuleInfoPtr(new ModuleInfo(m_factory, *i));
        m_list.push_back(modinfo);
    }
}

// Processes a module's specific command
void ModuleProcessor::handleModule(int fd, int argc, char *argv[],
                                   const char* command_name,
                                   const char* msgid,
                                   int modnameused)
{
    BOOST_ASSERT(command_name);
    BOOST_ASSERT(msgid);

    BOOST_ASSERT(m_factory->getMessageFactory());
    BOOST_ASSERT(m_factory->getModuleFactory());

    // retrives parameters
    ParameterVector commands = Utils::getCommands(command_name);
    cli::ParameterVector params;
    int startpos = static_cast<int>(commands.size());
    if (modnameused)
        startpos++;
    for (int i = startpos; i < argc; i++)
    {
        std::string value = argv[i];
        boost::trim_if(value, boost::is_any_of(" \r\n\t"));
        if (value.empty())
            break;
        params.push_back(value);
    }

    std::string cliresp;

    try
    {
        // check the module
        cli::ICommandPtr command = getCommandByMsgID(msgid);
        if (command->isRequireModule() == false)
        {
            cliresp = command->process(params);
        }
        else
        {
            IMessagePtr in = m_factory->getMessageFactory()->getMessage(msgid);
            BOOST_ASSERT(in);
            Utils::setProcessParams(in, params);

            const std::string receiver = getReceiverAppID(msgid);
            adapter::MessagesProtocol proto(m_factory, receiver);
            IMessagePtr out = proto.sendSync(in);
            BOOST_ASSERT(out);
            // check status
            if (out->getValue(msg::key::STATUS) != msg::key::OK)
            {
                cliresp = out->getValue(msg::key::ERROR);
            }
            else
            {
                cliresp = out->getValue(msg::key::CLIRESULT);
            }
        }
    }
    catch(Exception& err)
    {
        ast_cli(fd,
                "Failed to execute command\n"
		"No response from server or server not running.\n");
        throw err;
    }
    catch(boost::bad_lexical_cast&)
    {
        ast_cli(fd,
                "Failed to execute command\n"
		"Bad lexical cast.\n");
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }
    catch(...)
    {
        ast_cli(fd,
                "Failed to execute command\n"
		"Unknown error\n");
        throw Exception(__FILE__, __LINE__, "Unknown error");
    }

    ast_cli(fd, "%s", cliresp.c_str());
    ast_cli(fd, "\n");

    // reload module specific info
    // we should load new modules if the appears after execution
    // of the command
    // FIXME!!! add the code (see ticket #111)
    update();
}

// Processes a module's specific command
// (completion staff)
const ParameterVector ModuleProcessor::getCompletion(const std::string& line,
                                                     const std::string& word,
                                                     int pos,
                                                     const char* msgid)
{
    BOOST_ASSERT(msgid);
    ParameterVector result;

    cli::ICommandPtr command = getCommandByMsgID(msgid);

    // recalculate position
    ParameterVector command_names = base::Utils::split(command->getName(), " ");
    ParameterVector line_names = base::Utils::split(line, " ");

    ParameterVector setparams;
    BOOST_ASSERT(command_names.size() <= line_names.size());
    BOOST_ASSERT(static_cast<size_t>(pos) <= line_names.size());

    for (size_t i = 0; i < static_cast<size_t>(pos); i++)
    {
        // command names + module name
        if (i > command_names.size())
        {
            setparams.push_back(line_names[i]);
        }
    }

    if (command->isRequireModule() == false)
    {
        result = command->getCompletion(setparams);
    }
    else
    {
        IMessagePtr in = m_factory->getMessageFactory()->getMessage(msgid);
        BOOST_ASSERT(in);
        Utils::setCompletionRequest(in, setparams);

        const std::string receiver = getReceiverAppID(msgid);
        adapter::MessagesProtocol proto(m_factory, receiver);
        IMessagePtr out = proto.sendSync(in);
        BOOST_ASSERT(out);
        // check status
        if (out->getValue(msg::key::STATUS) != msg::key::OK)
        {
            throw Exception(__FILE__, __LINE__,
                            "Got a completion error: " +
                            out->getValue(msg::key::ERROR));
        }
        result = Utils::getCompletionResponse(out);
    }

    return result;
}


// Reloads module specific commands
void ModuleProcessor::update()
{
    std::for_each(m_list.begin(), m_list.end(),
                  boost::bind(&ModuleInfo::update, _1));
}

// Retrives command by its message id
const cli::ICommandPtr
ModuleProcessor::getCommandByMsgID(const std::string& msgid)
{
    ModuleList modules =
        m_factory->getModuleFactory()->getModules();
    for (ModuleList::iterator mod = modules.begin();
         mod != modules.end(); mod++)
    {
        cli::ICommandList list = (*mod)->getCLIInfo();
        for (cli::ICommandList::iterator j = list.begin();
             j != list.end(); j++)
        {
            if ((*j)->getMessageID() == msgid)
            {
                return *j;
            }
        }
    }

    // FIXME!!! bad code
    BOOST_ASSERT(false);
    return cli::ICommandPtr();
}

// Retrives the destination module id (main application module)
// the CLI command to be sent
const std::string ModuleProcessor::getReceiverAppID(const std::string& msgid) const
{
    ModuleList modules =
        m_factory->getModuleFactory()->getModules();
    for (ModuleList::iterator mod = modules.begin();
         mod != modules.end(); mod++)
    {
        cli::ICommandList list = (*mod)->getCLIInfo();
        for (cli::ICommandList::iterator j = list.begin();
             j != list.end(); j++)
        {
            if ((*j)->getMessageID() == msgid)
            {
                return (*mod)->getType() == klk::mod::APP ? (*mod)->getID() :
                    MODULE_COMMON_ID;
            }
        }
    }

    // FIXME!!! bad code
    BOOST_ASSERT(false);
    return MODULE_COMMON_ID;
}
