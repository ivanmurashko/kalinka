/**
   @file station.h
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
   - 2008/10/04 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STATION_H
#define KLK_STATION_H

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include "db.h"
#include "mod/info.h"
#include "ithreadfactory.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {
            /**
               @brief Base class for stations

               Base class for stations

               @ingroup grDVB
            */
            class Station : public mod::Info, public IStation
            {
                friend class TestStreamer;
            public:
                /**
                   Constructor

                   @param[in] uuid - the station uuid
                   @param[in] name - the station name
                   @param[in] type - the resource type
                   @param[in] source - the signal source uuid for the station
                   @param[in] factory - the factory
                */
                explicit Station(const std::string& uuid,
                                 const std::string& name,
                                 const std::string& type,
                                 const std::string& source,
                                 IFactory* factory);

                /**
                   Destructor
                */
                virtual ~Station();

                /**
                   Sets the station as failed to stream
                */
                void setFailed() throw();

                /**
                   Updates inuse info at the db
                */
                void updateInUse() throw();

                /**
                   Tests if the info is used by somebody

                   Checks DB to retrive the state

                   @return
                   - true
                   - false
                */
                virtual bool isInUse() throw();

                /**
                   Sets channel

                   @param[in] channel - the TV channel id to be set

                   @exception klk::Exception
                */
                void setChannel(const std::string& channel);

                /**
                   Sets route

                   @param[in] route - the route to be set

                   @exception klk::Exception
                */
                void setRoute(const std::string& route);

                /**
                   Retrives tuned dev

                   @return the dev pointer
                */
                const IDevPtr getDev() const;

                /**
                   Retrives route

                   @return the route pointer

                   @exception Exception if a route has not been set
                */
                virtual const sock::RouteInfo getRoute() const;

                /**
                   This one retrives data rate for the station

                   @return the rate
                */
                virtual const int getRate() const throw();

                /**
                   Checks is there any streamin activity for the station

                   @return
                   - true
                   - false
                */
                bool isStream() const throw()
                {
                    Locker lock(&m_lock);
                    return (m_dev != NULL);
                }
            private:
                IFactory* const m_factory; ///< the factory
                IModulePtr m_module; ///< module for sending messages
                SafeValue<sock::RouteInfo> m_route; ///< the route info
                SafeValue<std::string> m_type; ///< resource type DVB-S/T/C
                SafeValue<std::string> m_source; ///< source uuid
                SafeValue<std::string> m_channel; ///< the channel guid
                IDevPtr m_dev; ///< dev
                SafeValue<std::string> m_route_uuid; ///< route uuid
                SafeValue<u_int> m_no; ///< channel number
                SafeValue<int> m_rate; ///< data rate

                /**
                   frees assosiated and locked resources
                   route and channel
                */
                void freeResources() throw();

                /**
                   Sets in use flag

                   Updates DB to set the state

                   @param[in] value - the value to be set
                */
                virtual void setInUse(bool value);

                /**
                   Retrives the channel name

                   @return the name
                */
                virtual const std::string getChannelName() const throw()
                {
                    return getName();
                }


                /**
                   Retrives station number

                   @return the channel number

                   @exception klk::Exception if a channel has not been set
                */
                virtual u_int getChannelNumber() const;

                /**
                   Sets rate for the station

                   @param[in] rate - the value to be set

                   @exception klk::Exception
                */
                virtual void setRate(const int rate);

                /**
                   Unsets station
                */
                void unsetChannel() throw();

                /**
                   Unsets route
                */
                void unsetRoute() throw();
            };

            /**
               Station smart pointer that can be used in the containers
            */
            typedef boost::shared_ptr<Station> StationPtr;
        }
    }
}

#endif //KLK_STATION_H
