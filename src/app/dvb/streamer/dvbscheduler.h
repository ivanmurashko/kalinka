/**
   @file dvbscheduler.h
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
   - 2009/06/04 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_DVBSCHEDULER_H
#define KLK_DVBSCHEDULER_H

#include "scheduler.h"
#include "ithreadfactory.h"
#include "ifactory.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {
            /**
               @brief DVB threads scheduler

               DVB streamer threads scheduler

               @ingroup grStreamer
            */
            class Scheduler : public base::Scheduler
            {
            public:
                /**
                   Constructor
                */
                Scheduler(IFactory* factory);

                /**
                   Destructor
                */
                virtual ~Scheduler();

                /**
                   Sets new thread factory

                   @note FIXME!!!! used mainly in utests
                */
                void setThreadFactory(const IThreadFactoryPtr& factory);

                /**
                   Stops thread by dev

                   @param[in] dev - the dev
                */
                void stopThread(const IDevPtr& dev);

                /**
                   Creates a stream thread pointer
                   The thread will be created and
                   started if it was not created before

                   @param[in] dev - the dev info

                   @exception klk::Exception
                */
                void createStreamThread(const IDevPtr& dev);

                /**
                   Retrives stream info container for the specified dev

                   @param[in] dev - the dev

                   @return the pointer or null if
                   there is no any streaming activity for the dev

                   @exception klk::Exception
                */
                const IStreamPtr getStream(const IDevPtr& dev) const;

                /**
                   Stops all threads
                */
                virtual void stop() throw();
            private:
                /**
                   DVB thread info list
                */
                typedef std::list<IThreadInfoPtr> DVBThreadInfoList;

                SafeValue<IThreadFactoryPtr> m_factory; ///< processor factory
                mutable Mutex m_dvb_lock; ///< dvb threads lock
                DVBThreadInfoList m_dvb_threadinfo; ///< dvb related thread info

                /**
                   Retrives factory

                   @exception klk::Exception

                   @return the factory
                */
                const IThreadFactoryPtr getThreadFactory() const;

                /**
                   Checks is any thread started for the current
                   dev

                   @param[in] dev - the dev

                   @note NOT THREAD SAFE CODE

                   @return
                   - true
                   - false
                */
                bool isStarted(const IDevPtr& dev) const;

                /**
                   Stops a thread at the scheduler

                   @param[in] thread - the thread to be stoped

                   @exception klk::Exception
                */
                virtual void stopThread(const IThreadPtr& thread);
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                Scheduler(const Scheduler& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                Scheduler& operator=(const Scheduler& value);
            };
        }
    }
}

#endif //KLK_DVBSCHEDULER_H
