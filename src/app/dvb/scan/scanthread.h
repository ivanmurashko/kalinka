/**
   @file scanthread.h
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
   - 2008/12/28 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SCANTHREAD_H
#define KLK_SCANTHREAD_H

#include "thread.h"
#include "scanchannel.h"
#include "options.h"
#include "iscanprocessor.h"

namespace klk
{
    namespace dvb
    {
        namespace scan
        {
            /**
               @brief Scan thread

               The scaner thread

               @ingroup grDVBScan
            */
            class ScanThread : public base::Thread
            {
            public:
                /**
                   Constructor
                */
                explicit ScanThread(IFactory* factory);

                /**
                   Destructor
                */
                virtual ~ScanThread(){}

                /**
                   Registers the scan processor
                */
                void registerProcessor(const IProcessorPtr& processor);

                /**
                   Starts scan procedure on the specified adapter

                   @param[in] uuid - the dev uuid
                   @param[in] data - the file name of the file with scan data

                   @exception
                   @ref klk::ERROR
                */
                void startScan(const std::string& uuid,
                               const std::string& data);

                /**
                   Stops scan procedure on the specified (before) adapter
                */
                void stopScan() throw();

                /**
                   Is there scan started or not
                */
                bool isStarted() const throw();
            private:
                /**
                   Vector with scan data
                */
                typedef std::vector<std::string> ScanData;

                IFactory* const m_factory; ///< factory
                klk::Event m_trigger; ///< start/stop event trigger
                ScanData m_data; ///< scan data

                ScanChannelList m_scan_channel; ///< scan channel data
                IDevPtr m_dev; ///< tuned dev

                OptionConverter m_conv; ///< converter

                IProcessorPtr m_processor; ///< processor

                /**
                   @copydoc IThread::start
                */
                virtual void start();

                /**
                   @copydoc IThread::stop
                */
                virtual void stop() throw();

                /**
                   Do the scan job

                   @exception klk::Exception
                */
                void doScan();

                /**
                   Sets scan data

                   @param[in] data - the file with scan data

                   @exception klk::Exception
                */
                void setData(const std::string& data);

                /**
                   Saves result to the DB

                   @exception klk::Exception
                */
                void save2DB();

                /**
                   Do scan stop
                */
                void clearScan();

                /**
                   Save SDT data

                   @param[in] no - the channel number
                   @param[in] name - the channel name
                   @param[in] prvname - the provider name
                   @param[in] scrambled - is the channel scrambled or not
                */
                void saveSDT(u_int no, const std::string& name,
                             const std::string& prvname,
                             bool scrambled);

                /**
                   Save PMT data

                   @param[in] no - the channel number
                   @param[in] pid - the pid
                   @param[in] type - the pid's type
                */
                void savePMT(u_int no, u_int pid, const std::string& type);

                /**
                   Do scan for DVB-S dev

                   @param[in] data - the scan data

                   @exception klk::Exception in the case of any error
                */
                void doScanDVBS(const ScanData& data);

                /**
                   Do scan for DVB-T dev

                   @param[in] data - the scan data

                   @exception klk::Exception in the case of any error
                */
                void doScanDVBT(const ScanData& data);

                /**
                   Do tune for DVB-S dev

                   @param[in] data - the scan data

                   @exception klk::Exception in the case of any error
                */
                void tuneDVBS(const ScanData& data);

                /**
                   Do tune for DVB-T dev

                   @param[in] data - the scan data

                   @exception klk::Exception in the case of any error
                */
                void tuneDVBT(const ScanData& data);

                /**
                   Checks was there something found during scan or not

                   @return
                   - true at least one channel was found
                   - false - cannot find something
                */
                bool wasFound() const throw();
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                ScanThread(const ScanThread& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                ScanThread& operator=(const ScanThread& value);
            };

            /**
               ScanThread smart pointer
            */
            typedef boost::shared_ptr<ScanThread> ScanThreadPtr;
        }
    }
}


#endif //KLK_SCANTHREAD_H
