/**
   @file testplugin.h
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
   - 2008/10/15 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#include <cppunit/extensions/HelperMacros.h>

#ifndef KLK_TESTPLUGIN_H
#define KLK_TESTPLUGIN_H

#include <cppunit/extensions/HelperMacros.h>

#include <string>

#include "dvbscheduler.h"
#include "station.h"
#include "exception.h"

#include "dvb/utils/testhelper.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {
            /** @addtogroup grDVBStreamerTest

                @{
            */

            /**
               @brief Test stream channel

               There is a test channle info container for plugin unit test
            */
            class TestStation : public IStation
            {
            public:
                /**
                   Constructor

                   @param[in] name - the channel's name
                   @param[in] number - the channel's number
                   @param[in] host - the host
                   @param[in] port - the port
                */
                TestStation(const std::string& name,
                            const std::string& number,
                            const std::string& host,
                            const std::string& port);

                /**
                   Destructor
                */
                virtual ~TestStation(){}
            private:
                const std::string m_name; ///< the name
                const std::string m_number; ///< the number
                const std::string m_host; ///< host
                const std::string m_port; ///< port
                klk::SafeValue<int> m_rate; ///< data rate

                /**
                   Retrives the channel name

                   @return the name
                */
                virtual const std::string getChannelName() const throw();

                /**
                   Retrives station number

                   @return the channel number

                   @exception klk::Exception if a channel has not been set
                */
                virtual u_int getChannelNumber() const;

                /**
                   Retrives route

                   @return the route pointer

                   @exception klk::Exception if a route has not been set
                */
                virtual const sock::RouteInfo getRoute() const;

                /**
                   This one retrives data rate for the station

                   @return the rate
                */
                virtual const int getRate() const throw();

                /**
                   Sets rate for the station

                   @param[in] rate - the value to be set

                   @exception klk::Exception
                */
                virtual void setRate(const int rate);
            private:
                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                TestStation& operator=(const TestStation& value);

                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                TestStation(const TestStation& value);
            };

            /**
               Smart pointer for the TestStation
            */
            typedef boost::shared_ptr<TestStation> TestStationPtr;

            /**
               @brief The getstream2 plugin utest

               It tests getstream2 plugin. There are 2 tests. The first one is for correct tuning
               and the second one for incorrect.
            */
            class TestPlugin : public CppUnit::TestFixture, private TestHelper
            {
                CPPUNIT_TEST_SUITE(TestPlugin);
                CPPUNIT_TEST_EXCEPTION(testWrong, klk::Exception);
                CPPUNIT_TEST(testOk);
                CPPUNIT_TEST_SUITE_END();
            public:
                /**
                   Constructor
                */
                TestPlugin();

                /**
                   Destructor
                */
                virtual ~TestPlugin(){}

                /**
                   The unit test for OK requests
                */
                void testOk();

                /**
                   The unit test for wrong requests
                */
                void testWrong();

                /**
                   Set up context before running a test.
                */
                virtual void setUp();

                /**
                   Clean up after the test run.
                */
                virtual void tearDown();
            private:
                /**
                   Test channel list container
                */
                typedef std::list<TestStationPtr> TestStationList;
            private:
                Scheduler m_dvbscheduler; ///< dvb scheduler
                TestStationList m_stations; ///< channels list

                /**
                   Create dev for tests
                */
                void createDev();

                /**
                   Checks dev state
                */
                void checkDev();

                /**
                   Fill channels list
                */
                void fillChannels();

                /**
                   Starts channels

                   Each channel started with a small delay
                */
                void startChannels();

                /// Stops stream for all channels
                void stopChannels();

                /// Checks channles for the data actually run on them
                void checkChannels();
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                TestPlugin(const TestPlugin& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                TestPlugin& operator=(const TestPlugin& value);
            };

            /** @} */

        }
    }
}

#endif //KLK_TESTPLUGIN_H
