/**
   @file processor.cpp
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
   - 2008/11/16 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "processor.h"
#include "log.h"
#include "exception.h"
#include "cliutils.h"

using namespace klk;

//
// Processor class
//

// Constructor
// @param[in] factory - the factory object
// @param[in] modid - the module.id
Processor::Processor(IFactory* factory, const std::string& modid) :
    Mutex(), m_factory(factory), m_modid(modid), m_clicommands()
{
}

// Destructor
Processor::~Processor()
{
    unregisterAll();
}

// Register a sync message
void Processor::registerASync(const std::string& msgid,
                              BaseFunction func)
{
    Locker lock(this); //FIXME!!! is it necessary?
    // check that there is no such message registere
    if (m_processors.find(msgid) != m_processors.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "Message with uuid '%s' "
                             "has been already registered",
                             msgid.c_str());
    }

    m_processors.insert(ProcessorMap::value_type(msgid, func));

    // register message
    m_factory->getMessageFactory()->registerMessage(
        msgid, m_modid, msg::ASYNC);

}

// Register a sync message
void Processor::registerSync(const std::string& msgid,
                                   SyncFunction func)
{
    Locker lock(this); //FIXME!!! is it necessary?
    // check that there is no such message registere
    if (m_processors.find(msgid) != m_processors.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "Message with uuid '%s' "
                             "has been already registered",
                             msgid.c_str());
    }

    BaseFunction base = boost::bind(&Processor::processSync, this, _1, func);
    m_processors.insert(ProcessorMap::value_type(msgid, base));

    // register message
    m_factory->getMessageFactory()->registerMessage(
        msgid, m_modid, msg::SYNC_REQ);
}

// Register a CLI command for processing
// @param[in] command - the command to be registred
void Processor::registerCLI(const cli::ICommandPtr& command)

{
    BOOST_ASSERT(command);

    Locker lock(this); //FIXME!!! is it necessary?

    // should be unique
    cli::ICommandList::iterator i =
        std::find_if(m_clicommands.begin(), m_clicommands.end(),
                     boost::bind(std::equal_to<std::string>(),
                                 boost::bind(&cli::ICommand::getName,
                                             command),
                                 boost::bind(&cli::ICommand::getName,
                                             _1)));
    if (i != m_clicommands.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "Command with name '%s' "
                             "has been already registered",
                             command->getName().c_str());

    }

    // insert at the end
    m_clicommands.push_back(command);

    // create a processor
    SyncFunction func = boost::bind(&Processor::processCLI,
                                    this, _1, _2);
    // register it
    registerSync(command->getMessageID(), func);
}


// Processes a message
// @param[in] msg - the message to be processed
void Processor::process(const IMessagePtr& msg)
{
    Locker lock(this); //FIXME!!! is it necessary?
    ProcessorMap::iterator i = m_processors.find(msg->getID());
    if (i == m_processors.end())
    {
        i = m_processors.find(msg::id::ALL);
        if (i == m_processors.end())
        {
            throw klk::Exception(__FILE__, __LINE__, "Cannot find processor for message '%s'",
                                 msg->getID().c_str());
        }
    }

    i->second(msg);
}

// Unregister all processors
void Processor::unregisterAll() throw()
{
    for (ProcessorMap::iterator i = m_processors.begin();
         i != m_processors.end(); i++)
    {
        m_factory->getMessageFactory()->unregisterMessage(
            i->first, m_modid);
    }

    m_processors.clear();
}

// Process a sync message
void Processor::processSync(const IMessagePtr& msg, SyncFunction func)

{
    // process the message
    IMessagePtr out  =
        m_factory->getMessageFactory()->getResponse(msg);
    BOOST_ASSERT(out);

    try
    {
        func(msg, out);
        // set OK status for the response
        out->setData(msg::key::STATUS, msg::key::OK);
    }
    catch(const std::exception& err)
    {
        klk_log(
            KLKLOG_ERROR,
            "Error while processing a sync message with id '%s': %s",
            msg->getID().c_str(), err.what());
        // set FAILED status for the response
        out->setData(msg::key::STATUS, msg::key::FAILED);
        out->setData(msg::key::ERROR, err.what());
    }
    catch(...)
    {
        klk_log(
            KLKLOG_ERROR,
            "Error while processing a sync message with id '%s': "
            "unspecified error",
            msg->getID().c_str());
        // set FAILED status for the response
        out->setData(msg::key::STATUS, msg::key::FAILED);
        out->setData(msg::key::ERROR, err::UNSPECIFIED);
    }

    // send the message back
    IModuleFactory* modfactory = m_factory->getModuleFactory();
    IModulePtr
        receiver = modfactory->getModule(msg->getSenderID());
    BOOST_ASSERT(receiver);
    receiver->addMessage(out);
}

// Processes a CLI command
// @param[in] in - the message to be processed
// @param[in] out - the result
void Processor::processCLI(const IMessagePtr& in,
                           const IMessagePtr& out)
{
    BOOST_ASSERT(in);
    BOOST_ASSERT(out);

    Locker lock(this); //FIXME!!! is it necessary?
    // get the command
    const std::string msgid = in->getID();
    cli::ICommandList::iterator i =
        std::find_if(m_clicommands.begin(), m_clicommands.end(),
                     boost::bind(std::equal_to<std::string>(),
                                 msgid,
                                 boost::bind(&cli::ICommand::getMessageID,
                                             _1)));
    BOOST_ASSERT(i != m_clicommands.end());

    // determine mode
    switch (cli::Utils::getCommandMode(in))
    {
    case cli::PROCESS:
    {
        // retrive parameters
        cli::ParameterVector params = cli::Utils::getProcessParams(in);
        // process it
        const std::string result = (*i)->process(params);
        out->setData(msg::key::CLIRESULT, result);
        break;
    }
    case cli::GETCOMPLETION:
    {
        const cli::ParameterVector setparams =
            cli::Utils::getCompletionRequest(in);
        cli::ParameterVector completions =
            (*i)->getCompletion(setparams);
        cli::Utils::setCompletionResponse(out, completions);
        break;
    }
    default:
        throw Exception(__FILE__, __LINE__, "Unsupported CLI mode");
        break;
    }
}
