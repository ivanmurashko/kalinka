/**
   @file testthreadfactory.h
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
   - 2009/06/05 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTTHREADFACTORY_H
#define KLK_TESTTHREADFACTORY_H

#include "ithreadfactory.h"
#include "station.h"
#include "thread.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {

            /** @defgroup grDVBProcessorTest DVB processor (implementation for utests)

                Processing DVB for utests

                @ingroup grDVBStreamerTest

                @{
            */

            /**
               @brief The klk::dvb::stream::IThreadFactory impl.

               IThreadFactory implementation for utests
            */
            class TestThreadFactory : public IThreadFactory
            {
            public:
                /**
                   Constructor
                */
                TestThreadFactory();

                /**
                   Destructor
                */
                virtual ~TestThreadFactory();

                /**
                   Checks is there any streaming channels started

                   @return
                   - true
                   - false
                */
                bool empty() const;

                /**
                   Checks is there a channel with the specified name

                   @param[in] channel - the channel name

                   @return
                   - true
                   - false
                */
                bool isStream(const std::string& channel);

                /**
                   Adds a station

                   @param[in] station - the station to be added

                   @exception klk::Exception
                */
                void addStation(const IStationPtr& station);

                /**
                   Deletes a station

                   @param[in] station - the station to be stopped

                   @exception klk::Exception
                */
                void delStation(const IStationPtr& station);
            private:
                mutable klk::Mutex m_lock; ///< locker
                IStationList m_stations; ///< stream channels

                /**
                   Gets a StreamThread pointer

                   @param[in] dev - the dev info

                   @return pointer to the thread

                   @exception klk::Exception
                */
                virtual const IThreadInfoPtr
                    createStreamThread(const IDevPtr& dev);
            private:
                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                TestThreadFactory& operator=(const TestThreadFactory& value);

                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                TestThreadFactory(const TestThreadFactory& value);
            };

            /**
               Smart pointer
            */
            typedef boost::shared_ptr<TestThreadFactory> TestThreadFactoryPtr;

            /**
               @brief The test stream thread

               TestThread
            */
            class TestThread : public IStream, public base::Thread
            {
            public:
                /**
                   Constructor

                   @param[in] factory - the factory
                */
                TestThread(TestThreadFactory* factory);

                /**
                   Destructor
                */
                virtual ~TestThread();
            private:
                TestThreadFactory* const m_factory; ///< the factory
                IStationList m_stations; ///< stream channels

                /**
                   @copydoc IThread::start
                */
                virtual void start();

                /**
                   @copydoc IThread::stop
                */
                virtual void stop() throw();

                /**
                   Adds a station

                   @param[in] station - the station to be added

                   @exception klk::Exception
                */
                virtual void addStation(const IStationPtr& station);

                /**
                   Dels a station

                   @param[in] station - the station to be added

                   @exception klk::Exception
                */
                virtual void delStation(const IStationPtr& station);

                /**
                   Checks is there any striming activity
                   at the streaming thread

                   @return
                   - true - there is at least one channel for streaming
                   - false - there is no any streamin channels
                */
                virtual const bool isStream() const;
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                TestThread(const TestThread& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                TestThread& operator=(const TestThread& value);
            };

            /** @} */

        }
    }
}

#endif //KLK_TESTTHREADFACTORY_H
