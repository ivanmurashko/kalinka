/**
   @file processor.h
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
   - 2009/11/05 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_PROCESSOR_H
#define KLK_PROCESSOR_H

#include <list>
#include <map>

#include <boost/function/function1.hpp>

#include "gst/iprocessor.h"
#include "thread.h"
#include "transscheduler.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief Processor thread

           There is a thread that process GST events from run gst pipelines

           @ingroup grTrans
        */
        class ProcessorThread : public base::Thread, public gst::IProcessor
        {
        public:
            /**
               Constructor

               @param[in] scheduler - the scheduler
            */
            explicit ProcessorThread(Scheduler* scheduler);

            /**
               Destructor
            */
            virtual ~ProcessorThread();
        private:
            /**
               Events storage
            */
            typedef std::list<gst::IEventPtr> Storage;

            /**
               Event processor functor
            */
            typedef boost::function1<void, const gst::IEventPtr&>
                ProcessorFunctor;

            /**
               Functors map
            */
            typedef std::map<gst::EventType, ProcessorFunctor> Processors;

            Processors m_processors; ///< processors storage
            Storage m_storage; ///< events storage
            Event m_event; ///< event notification

            /**
               @copydoc klk::IThread::init
            */
            virtual void init();

            /**
               @copydoc klk::IThread::start
            */
            virtual void start();

            /**
               @copydoc klk::IThread::stop
            */
            virtual void stop() throw();

            /**
               @copydoc klk::gst::IProcessor::addEvent
            */
            virtual void addEvent(const gst::IEventPtr& event);

            /**
               Processes all events
            */
            void processEvents();

            /**
               Retrives an event from the storage

               The retrived event is removed from the storage

               @return pointer to the event or NULL if there is any event at
               the storage
            */
            const gst::IEventPtr getEvent();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ProcessorThread(const ProcessorThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ProcessorThread& operator=(const ProcessorThread& value);
        };

        /**
           Processor thread smart pointer
        */
        typedef boost::shared_ptr<ProcessorThread> ProcessorThreadPtr;
    }
}

#endif //KLK_PROCESSOR_H
