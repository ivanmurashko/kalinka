/**
   @file gstscan.h
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

#ifndef KLK_GSTSCAN_H
#define KLK_GSTSCAN_H

#include <gst/gst.h>

#include "ifactory.h"
#include "iscanprocessor.h"
#include "gst/gstthread.h"

namespace klk
{
    namespace dvb
    {
        namespace scan
        {
            /**
               @brief Scan processor

               Scan processor based on Gstreamer framework

               @note The code is called from ONLY ONE thrad thus no need any
               thread safety for it

               @ingroup grScan
            */
            class GSTProcessor : public IProcessor, private gst::Thread
            {
            public:
                /**
                   Constructor

                   @param[in] factory - the factory
                */
                GSTProcessor(IFactory* factory);

                /**
                   Destructor
                */
                virtual ~GSTProcessor();
            private:
                IFactory* const m_factory; ///< the factory
                GstElement* m_mpegtsparse; ///< muxer
                SDTFunction m_dumpSDT; ///< SDT dumper
                PMTFunction m_dumpPMT; ///< PMT dumper

                /**
                   Stops processing

                   @exception klk::Exception
                */
                virtual void stopProcessing() throw();

                /**
                   @copydoc klk::gst::Thread::callBus
                */
                virtual gboolean callBus(GstMessage* msg);

                /**
                   @copydoc klkdvbscan::IProcessor::start
                */
                virtual void start(const IDevPtr& dev);

                /**
                   @copydoc klkdvbscan::IProcessor::registerProcessors
                */
                virtual void registerProcessors(SDTFunction sdt,
                                                PMTFunction pmt);

                /**
                   Do pipeline initialization

                   @param[in] dev - the assigned dev

                   @exception klk::Exception
                */
                void init(const IDevPtr& dev);

                /**
                   Calls when we receive SDT message

                   @param[in] structure - the message was gotten

                   @exception klk::Exception
                */
                void processSDT(const GstStructure *structure);

                /**
                   Calls when we receive PMT message

                   @param[in] structure - the message was gotten

                   @exception klk::Exception
                */
                void processPMT(const GstStructure *structure);

                /**
                   cleans the processor internals
                */
                virtual void clean() throw();
            private:
                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                GSTProcessor& operator=(const GSTProcessor& value);

                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                GSTProcessor(const GSTProcessor& value);
            };
        }
    }
}

#endif //KLK_GSTSCAN_H
