/**
   @file module.h
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
   - 2008/10/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MODULE_H
#define KLK_MODULE_H

#include <sys/time.h>

#include <string>
#include <map>

#include "imodule.h"
#include "ifactory.h"
#include "messageholder.h"
#include "thread.h"
#include "processor.h"
#include "usage.h"
#include "modulescheduler.h"
#include "klksemaphore.h"

#include "snmp/processor.h"

namespace klk
{
    /**
       @brief  class for all modules

       class for all modules

       @ingroup grModule
    */
    class Module : public IModule, public base::Thread
    {
    public:
        /**
           Constructor

           @param[in] factory the factory for getting all necessary objects
           @param[in] id the module id
        */
        Module(IFactory* factory, const std::string& id);

        /// Destructor
        virtual ~Module(){}

        /// @copydoc klk::IModule::addMessage()
        virtual void addMessage(const IMessagePtr& msg);

        /// @copydoc klk::IModule::sendSyncMessage()
        virtual void sendSyncMessage(const IMessagePtr& in,
                                     IMessagePtr& out);

        /// @copydoc klk::IModule::getUptime
        virtual const time_t getUptime() const;

        /**
           Retrive CPU usage info in the following interval
           (0, 1)

           @return the CPU usage
        */
        virtual const double getCPUUsage() const;
    protected:
        /**
           Gets factory

           @return the factory

           @exception @ref klk::Exception
        */
        IFactory* getFactory();

        /**
           Register a CLI command for processing

           @param[in] command - the command to be registred

           @exception klk::Exception
        */
        void registerCLI(const cli::ICommandPtr& command);

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
           @brief Register a new wait checkpoint

           There are can be several checkpoints that module should pass
           until it can be considered as started. The method sets the new
           checkpoint ofr startup

           @see passStartupCheckpoint()
        */
        void registerStartupCheckpoint();

        /**
           @brief Startup checkpoint pass event

           The method is called to indicate that a startup
           checkpoint was passed

           @see registerStartupCheckpoint()
        */
        void passStartupCheckpoint();

        /**
           Gets the module's id

           @return the id or NULL if there was an error
        */
        virtual const std::string getID() const throw() {return m_id;}

        /**
           Do some actions before main loop

           @exception @ref klk::Exception
        */
        virtual void preMainLoop();

        /**
           Do some actions after main loop
        */
        virtual void postMainLoop() throw();

        /**
           Adds a dependency on another module

           @param[in] modid - the module that the current module depends on
        */
        void addDependency(const std::string& modid);

        /**
           @copydoc IModule::getVersion
        */
        virtual const std::string getVersion() const throw();

        /// @copydoc klk::IThread::init
        virtual void init();

        //// Scheduler related methods
        /**
           Register a thread

           @param[in] thread - the thread to be registered
        */
        void registerThread(const IThreadPtr& thread);

        /**
           Register a time

           @param[in] f - the function to be called
           @param[in] intrv - the timer interval
        */
        void registerTimer(TimerFunction f, const time_t intrv);

        /**
           Register an SNMP processor

           @param[in] f - the procesor functor
           @param[in] sockname - communication file socket name
           (file name)
        */
        void registerSNMP(snmp::DataProcessor f,
                          const std::string& sockname);
    private:
        IFactory * const m_factory; ///< module factory
        std::string m_id; ///< module id
        MessageHolder4Standard m_container; ///< messages containers
        MessageHolder4SyncRes m_sync_container; ///< sync msg container
        Processor m_processor; ///< processor
        time_t m_start_time; ///< start time
        mod::Scheduler m_scheduler; ///< module scheduler
        UsagePtr m_usage; ///< usage class
        Semaphore m_start_sem; ///< startup semaphore
        int m_checkpoint_count; ///< check point counter
        mutable Mutex m_checkpoint_mutex; ///< checkpoint mutex

        /// @copydoc klk::IThread::start
        virtual void start();

        /// @copydoc klk::IThread::stop
        virtual void stop() throw();

        /**
           Gets info for CLI

           @return the list with known CLI commands
        */
        virtual const cli::ICommandList getCLIInfo() const throw();

        /// @copydoc klk::IModule::waitStart
        virtual void waitStart(time_t timeout);

        /**
           Does the main action

           Processes a message
        */
        void processMessage() throw();

        /// @copydoc klk::IModule::isStarted
        virtual bool isStarted() const;
    private:
        /**
           Copy constructor
        */
        Module(const Module&);

        /**
           Assignment opearator
        */
        Module& operator=(const Module&);
    };
};

#endif //KLK_MODULE_H
