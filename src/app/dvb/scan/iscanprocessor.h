/**
   @file iscanprocessor.h
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
   - 2009/07/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_DVBSCANPROCESSOR_H
#define KLK_DVBSCANPROCESSOR_H

#include <boost/function/function4.hpp>
#include <boost/function/function3.hpp>
#include <boost/shared_ptr.hpp>

#include "iresources.h"
#include "exception.h"

namespace klk
{
    namespace dvb
    {
        namespace scan
        {
            /**
               Functor for SDT packets processing during DVB scan

               @ingroup grDVBScan
            */
            typedef boost::function4<void, u_int, std::string,
                std::string, bool>
                SDTFunction;

            /**
               Functor for PMT packets processing during DVB scan

               @ingroup grDVBScan
            */
            typedef boost::function3<void, u_int, u_int, std::string>
                PMTFunction;

            /**
               Default processor for SDT

               @ingroup grDVBScan
            */
            struct DefaultSDT
            {
                inline void operator()(u_int, std::string, std::string, bool)
                    {
                        NOTIMPLEMENTED;
                    }
            };

            /**
               Default processor for PMT

               @ingroup grDVBScan
            */
            struct DefaultPMT
            {
                inline void operator()(u_int, u_int, std::string)
                    {
                        NOTIMPLEMENTED;
                    }
            };

            /**
               @brief The DVB scan processor

               There is a base interface for DVB processing
               The processor put found info to scan thread for saving

               @ingroup grDVBScan
            */
            class IProcessor
            {
            public:
                /**
                   Destructor
                */
                virtual ~IProcessor(){}

                /**
                   Starts processing

                   @param[in] dev - the assigned dev

                   @exception klk::Exception
                */
                virtual void start(const IDevPtr& dev) = 0;

                /**
                   Stops processing

                   @exception klk::Exception
                */
                virtual void stopProcessing() throw() = 0;

                /**
                   Register SDT and PMT savers

                   @param[in] sdt - the SDT processor
                   @param[in] pmt - the PMT processor

                   @exception klk::Exception
                */
                virtual void registerProcessors(SDTFunction sdt,
                                                PMTFunction pmt) = 0;
            };

            /**
               Smart pointer for IProcessor
            */
            typedef boost::shared_ptr<IProcessor> IProcessorPtr;
        }
    }
}

#endif //KLK_DVBSCANPROCESSOR_H
