/**
   @file trans.h
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
   - 2009/03/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TRANS_H
#define KLK_TRANS_H

#include "launchmodule.h"
#include "sourcefactory.h"
#include "transscheduler.h"
#include "transinfo.h"
#include "mod/infocontainer.h"

namespace klk
{
    namespace  trans
    {

        /** @defgroup grTrans Transcode application

            @brief Converts multimedia data from one source to another

            The application operates with info stored at klk::trans::TaskInfo The info consists
            of the following:
            - Source info @see klk::trans::SourceInfo
            - Destination info @see klk::trans::SourceInfo
            - Schedule info (start time identifier and duration)  @see klk::trans::ScheduleInfo

            When the application needs to start a task it will create an instance of
            klk::trans::Task class and the instance is added into klk::trans::Scheduler class.

            The scheduler start the task and if there was a problem during startup
            the instnace will be removed as result the
            klk::trans::Task::~Task will be called and all resources assosiated with the task
            will be freed.

            Each klk::trans::SCHEDULE_INTERVAL the application find
            all task what should be stopped with klk::trans::Transcode::getStopList
            and stops them. After this one the application find all tasks that should be started
            with klk::trans::Transcode::getStartList and starts them.

            @ingroup grApp

            @{
        */


        /**
           @brief Transcode module

           Transcode module
        */
        class Transcode : public launcher::Module
        {
            friend class TestBase;
        public:
            /**
               Constructor

               @param[in] factory the module factory
            */
            Transcode(IFactory *factory);

            /**
               Destructor
            */
            virtual ~Transcode();

            /**
               Gets factory

               @note FIXME!!! may be mor better to make it as private

               @return the factory

               @exception @ref klk::Exception
            */
            IFactory* getFactory(){return Module::getFactory();}
        private:
            /// The TaskInfo storage
            typedef mod::InfoContainer<TaskInfo>::InfoSet InfoSet;

            mod::InfoContainer<TaskInfo> m_info; ///< the input info storage
            SourceFactory m_source_factory; ///< source factory
            Scheduler m_scheduler; ///< pipelines

            /// Do some actions before main loop
            virtual void preMainLoop();

            /// Do some actions after main loop
            virtual void postMainLoop() throw();

            /**
               Process changes at the DB

               @param[in] msg - the input message

               @exception @ref klk::Exception
            */
            virtual void processDB(const IMessagePtr& msg);

            /**
               Register all processors

               @exception klk::Exception
            */
            virtual void registerProcessors();

            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw();

            /**
               Retrives an set with data from @ref grDB "database"

               @return set
            */
            const InfoSet getTaskInfoFromDB();

            /**
               Does the task initialization

               @param[in] taskinfo - the task's info to be initialized

               @exception klk::Exception
            */
            void initTask(const TaskInfoPtr& taskinfo) throw();

            /**
               Does the task de-initialization
               Free resources allocated with the task

               @param[in] info - the info to be de-initialized

               @exception klk::Exception
            */
            void deinitInfo(const mod::InfoPtr& info) throw();

            /**
               Checks schedule playback info. It stops task with
               duration interval exceed and also start a task
               that should be started accordingly with the schedule

               It's called periodically with interval specified at
               klk::trans::SCHEDULE_INTERVAL variable

               @exception klk::Exception
            */
            void doSchedulePlayback();

            /**
               Retrives a list of tasks that should be stopped
               accordingly with scheduled playback settings

               @return the list with the tasks to be stopped

               @exception klk::Exception
            */
            const TaskInfoList getStopList() const;

            /**
               Retrives a list of tasks that should be started
               accordingly with scheduled playback settings

               @return the list with the tasks to be started

               @exception klk::Exception
            */
            const TaskInfoList getStartList() const;

            /**
               Processes SNMP request

               @param[in] req - the request

               @return the response
            */
            const snmp::IDataPtr processSNMP(const snmp::IDataPtr& req);

#ifdef LINUX
            /**
               @brief Processes info about IEEE1394 changes

               The method processes info about
               @ref grIEEE1394 "IEEE1394 devices"
               state changes.

               There are such events as a new device appearance or
               used device was switched off

               @param[in] msg - the message with info about the device

               @note FIXME!!! not a good place for the processing in the main module class

               @exception klk::Exception in the case of any error
             */
            void processIEEE1394(const IMessagePtr& msg);
#endif //LINUX
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Transcode(const Transcode& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Transcode& operator=(const Transcode& value);
        };

        /** @} */
    }
}


#endif //KLK_TRANS_H
