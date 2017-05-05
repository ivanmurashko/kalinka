/**
   @file processor.h
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

#ifndef KLK_PROCESSOR_H
#define KLK_PROCESSOR_H

#include <map>
#include <string>

#include <boost/function/function1.hpp>
#include <boost/function/function2.hpp>

#include "imessage.h"
#include "thread.h"
#include "ifactory.h"
#include "icli.h"


namespace klk
{

    /** @defgroup grProcessor Messages processors
        @{

        The group keeps defenitions for messages processors

        @ingroup grModule
    */

    /**
       Base processor
    */
    typedef boost::function1<void,
        const IMessagePtr&> BaseFunction;

    /**
       Processor for Sync messages
    */
    typedef boost::function2<void,
        const IMessagePtr&, const IMessagePtr&> SyncFunction;

    /**
       Processor map
    */
    typedef std::map<std::string, BaseFunction> ProcessorMap;

    /**
       @brief Messages processor

       Messages processor
    */
    class Processor : public Mutex
    {
    public:
        /**
           Constructor

           @param[in] factory - the factory object
           @param[in] modid - the module.id

        */
        Processor(IFactory* factory,
                  const std::string& modid);

        /**
           Destructor
        */
        virtual ~Processor();

        /**
           Register a sync message

           @param[in] msgid - message id
           @param[in] func - the processor

           @exception klk::Exception
        */
        void registerASync(const std::string& msgid,
                           BaseFunction func);

        /**
           Register a sync message

           @param[in] msgid - message id
           @param[in] func - the processor

           @exception klk::Exception
        */
        void registerSync(const std::string& msgid,
                          SyncFunction func);


        /**
           Register a CLI command for processing

           @param[in] command - the command to be registred

           @exception klk::Exception
        */
        void registerCLI(const cli::ICommandPtr& command);

        /**
           Processes a message

           @param[in] msg - the message to be processed

           @exception @ref klk::Exception
        */
        void process(const IMessagePtr& msg);

        /**
           Gets info for CLI

           @return the list with known CLI commands
        */
        const cli::ICommandList getCLIInfo() const throw()
        {
            return m_clicommands;
        }
    private:
        IFactory* const m_factory; ///< factory
        std::string m_modid; ///< module id
        cli::ICommandList m_clicommands; ///< cli commands

        ProcessorMap m_processors; ///< processors

        /**
           Process a sync message

           @param[in] msg - the message to be processed
           @param[in] func - the func to be called for processing

           @exception @ref klk::Exception
        */
        void processSync(const IMessagePtr& msg, SyncFunction func);

        /**
           Processes a CLI command

           @param[in] in - the message to be processed
           @param[in] out - the result

           @exception @ref klk::Exception
        */
        void processCLI(const IMessagePtr& in,
                        const IMessagePtr& out);

        /**
           Unregister all processors
        */
        void unregisterAll() throw();
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        Processor(const Processor& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        Processor& operator=(const Processor& value);
    };
    /** @} */
}

#endif //KLK_PROCESSOR_H
