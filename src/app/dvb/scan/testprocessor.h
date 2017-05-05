/**
   @file testprocessor.h
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
   - 2009/07/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTPROCESSOR_H
#define KLK_TESTPROCESSOR_H

#include "iscanprocessor.h"
#include "exception.h"
#include "errors.h"

namespace klk
{
    namespace dvb
    {
        namespace scan
        {
            /**
               @brief The test processor for DVB scan

               The test processor for DVB scan

               @ingroup grDVBScan
            */
            class TestProcessor : public IProcessor
            {
            public:
                /**
                   Constructor
                */
                TestProcessor();

                /**
                   Destructor
                */
                virtual ~TestProcessor(){}

                /**
                   Checks the result

                   @exception klk::Exception
                */
                void checkResult(){BOOST_ASSERT(m_rc == klk::OK);}
            private:
                SDTFunction m_sdt; ///< SDT processor
                PMTFunction m_pmt; ///< PMT processor
                Result m_rc;

                /**
                   Starts processing

                   @param[in] dev - the assigned dev

                   @exception klk::Exception
                */
                virtual void start(const IDevPtr& dev);

                /**
                   Stops processing

                   @exception klk::Exception
                */
                virtual void stopProcessing() throw();

                /**
                   Register SDT and PMT savers

                   @param[in] sdt - the SDT processor
                   @param[in] pmt - the PMT processor

                   @exception klk::Exception
                */
                virtual void registerProcessors(SDTFunction sdt,
                                                PMTFunction pmt);

                /**
                   Do scan for DVB-S

                   @param[in] dev - the tuned dev
                */
                void doDVBSScan(const IDevPtr& dev);

                /**
                   Do scan for DVB-T

                   @param[in] dev - the tuned dev
                */
                void doDVBTScan(const IDevPtr& dev);
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                TestProcessor(const TestProcessor& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                TestProcessor& operator=(const TestProcessor& value);
            };

            /**
               Test processor smart pointer
            */
            typedef boost::shared_ptr<TestProcessor> TestProcessorPtr;
        }
    }
}

#endif //KLK_TESTPROCESSOR_H
