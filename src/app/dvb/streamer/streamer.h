/**
   @file streamer.h
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
   - 2008/09/21 created by ipp (Ivan Murashko)
   - 2008/11/03 move code from DVB module here by ipp
   - 2008/01/04 add some excpetions here by ipp
   - 2009/07/04 DB was cardinally updated
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STREAMER_H
#define KLK_STREAMER_H

#include "launchmodule.h"
#include "errors.h"
#include "thread.h"
#include "dvbscheduler.h"
#include "station.h"
#include "mod/infocontainer.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {

            /** @defgroup grDVBStreamer DVB streamer application

                DVB streamer application. The application provides a possibility
                to stream DVB (T/S) channels via UDP (multicast/unicast)

                @note FIXME!!! add support for RTP. See ticket #203

                @ingroup grApp

                @{
            */

            /**
               @brief The Streamer module class implementation

               The Streamer module class implementation

               @ingroup grStreamer
            */
            class Streamer : public launcher::Module
            {
                friend class TestStreamer;
                friend class TestBase;
            public:
                /**
                   Constructor

                   @param[in] factory the module factory
                */
                Streamer(IFactory *factory);

                /**
                   Destructor
                */
                virtual ~Streamer();
            private:
                /// The info internal storage
                typedef mod::InfoContainer<Station>::InfoSet InfoSet;
                /// The list with stations
                typedef std::list<StationPtr> InfoList;

                mod::InfoContainer<Station> m_info; ///< the stream info
                Scheduler m_dvb_scheduler; ///< the plugins threads scheduler
                klk::SafeValue<time_t> m_last_dbupdate; ///< last DB update time

                /**
                   Register all processors

                   @exception klk::Exception
                */
                virtual void registerProcessors();

                /**
                   Do some actions before main loop

                   @exception klk::Exception
                */
                virtual void preMainLoop();

                /**
                   Do some actions after main loop
                */
                virtual void postMainLoop() throw();

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
                virtual void processDB(const IMessagePtr& msg);

                /**
                   Retrives an set with data from @ref grDB "database"

                   @return set
                */
                virtual const InfoSet getInfoFromDB();

                /**
                   Updates lock info
                */
                void updateLock();

                /**
                   Processes signal lost event

                   @param[in] msg - the message

                   @exception @ref klk::Exception
                */
                void processSignalLost(const IMessagePtr& msg);

                /**
                   Retrives a set with currently streamed stations
                   that are present at the DB

                   @return the set
                */
                const InfoSet getCurrentInfoFromDB();

                /**
                   Retrives a set with new stations to stream
                   that are present at the DB

                   @return the set
                */
                const InfoSet getNewInfoFromDB();

                /**
                   Retrives a list with new stations to stream
                   that are present at the DB

                   @param[in] source - the signal source uuid
                   @param[in] type - the DVB dev type

                   @return the list
                */
                const InfoList getNewInfoFromDB(const std::string& source,
                                                     const std::string& type);

                /**
                   Starts stream for specified module info

                   @param[in] station - the station that has to be started to stream
                */
                void startStream(const StationPtr& station);

                /**
                   Stops stream for specified module info

                   @param[in] station - the station that has to be stopped
                */
                void stopStream(const StationPtr& station) throw();

                /// Clears fail list
                void clearFailed() throw();

                /// Check failers
                void checkFailed();

                /**
                   Processes SNMP request

                   @param[in] req - the request

                   @return the response
                */
                const snmp::IDataPtr processSNMP(const snmp::IDataPtr& req);
            private:
                /**
                   Copy constructor
                */
                Streamer(const Streamer&);

                /**
                   Assignment opearator
                */
                Streamer& operator=(const Streamer&);
            };

            /** @} */

        }
    }
}

#endif //KLK_STREAMER_H

