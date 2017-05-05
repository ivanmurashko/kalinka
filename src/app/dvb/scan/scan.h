/**
   @file scan.h
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
   - 2009/06/15 created by ipp (Ivan Murashko)
   - 2009/07/17 some refactoring by ipp
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SCAN_H
#define KLK_SCAN_H

#include "scanthread.h"
#include "launchmodule.h"

namespace klk
{
    namespace dvb
    {
        namespace scan
        {

            /** @defgroup grDVBScan DVB scan application
                @{

                The DVB scan application. The application scans for available DVB channels.
                It uses input data in the format used by dvb-apps:
                http://www.linuxtv.org/wiki/index.php/LinuxTV_dvb-apps

                @ingroup grApp
            */

            /**
               @brief The scan application main module

               Scan application module
            */
            class Scan : public klk::launcher::Module
            {
                friend class TestScan;
            public:
                /**
                   Constructor

                   @param[in] factory - the factory
                */
                Scan(IFactory *factory);

                /**
                   Destructor
                */
                virtual ~Scan();

                /**
                   Starts scan procedure for specified source
                   and tune table

                   @param[in] source - the source UUID
                   @param[in] data - the tune data file

                   @exception klk::Exception
                */
                void startScan(const std::string& source,
                               const std::string& data);

                /**
                   Stops scan
                */
                void stopScan() throw();

                /**
                   Checks is there scan started or not

                   @return
                   - true
                   - false
                */
                bool isScanStarted() const throw();
            private:
                boost::shared_ptr<ScanThread> m_thread; ///< the scan thread

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
                   Process changes at the DB

                   @param[in] msg - the input message

                   @exception @ref klk::Exception
                */
                virtual void processDB(const IMessagePtr& msg)
                {
                    // FIXME!!! should not be called
                }
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                Scan(const Scan& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                Scan& operator=(const Scan& value);
            };

            /** @} */

        }
    }
}

#endif //KLK_SCAN_H
