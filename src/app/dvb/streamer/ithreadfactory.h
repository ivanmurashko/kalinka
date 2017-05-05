/**
   @file ithreadfactory.h
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
   - 2009/06/03 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ITHREADFACTORY_H
#define KLK_ITHREADFACTORY_H

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include "ithread.h"
#include "iresources.h"
#include "exception.h"

#include <socket/routeinfo.h>

namespace klk
{
    namespace dvb
    {
        namespace stream
        {

            /** @defgroup grDVBProcessor DVB processor

                Processing DVB job

                @ingroup grDVBStreamer

                @{
            */
            /**
               @brief Station info container

               The station info container
            */
            class IStation
            {
            public:
                /**
                   Destructor
                */
                virtual ~IStation(){}

                /**
                   Retrives the channel name

                   @return the name
                */
                virtual const std::string getChannelName() const throw() = 0;

                /**
                   Retrives station number

                   @return the channel number

                   @exception klk::Exception if a channel has not been set
                */
                virtual u_int getChannelNumber() const = 0;

                /**
                   Retrives route

                   @return the route pointer

                   @exception klk::Exception if a route has not been set
                */
                virtual const sock::RouteInfo getRoute() const = 0;

                /**
                   This one retrives data rate for the station

                   @return the rate
                */
                virtual const int getRate() const throw() = 0;

                /**
                   Sets rate for the station

                   @param[in] rate - the value to be set

                   @exception klk::Exception
                */
                virtual void setRate(const int rate) = 0;
            };

            /**
               Smart pointer for the station interface

               @ingroup grDVBProcessor
            */
            typedef boost::shared_ptr<IStation> IStationPtr;

            /**
               Stations list
            */
            typedef std::list<IStationPtr> IStationList;

            /**
               @brief Stream related info

               The interface provides an additional info for
               stream thread
            */
            class IStream
            {
            public:
                /**
                   Destructor
                */
                virtual ~IStream(){}

                /**
                   Adds a station

                   @param[in] station - the station to be added

                   @exception klk::Exception
                */
                virtual void addStation(const IStationPtr& station) = 0;

                /**
                   Dels a station

                   @param[in] station - the station to be stopped

                   @exception klk::Exception
                */
                virtual void delStation(const IStationPtr& station) = 0;

                /**
                   Checks is there any striming activity at
                   the streaming thread

                   @return
                   - true - there is at least one station for streaming
                   - false - there is no any streamin stations
                */
                virtual const bool isStream() const = 0;
            };

            /**
               IStream smart pointer

               @ingroup grDVBProcessor
            */
            typedef boost::shared_ptr<IStream> IStreamPtr;

            /**
               @brief Process thread

               Process thread
            */
            class IThreadInfo
            {
            public:
                /**
                   Destructor
                */
                virtual ~IThreadInfo(){}

                /**
                   Retrives IThread pointer

                   @return the thread pointer
                */
                virtual const IThreadPtr getThread() const throw() = 0;

                /**
                   Checks is the thread correspond to the specified
                   device or not

                   @param[in] dev - the dev to be checked

                   @return
                   - true
                   - false
                */
                virtual const bool
                    matchDev(const IDevPtr& dev) const throw() = 0;

                /**
                   Retrives IStream pointer

                   @return the stream pointer

                   @exception klk::Exception
                */
                virtual const IStreamPtr getStream() const = 0;
            };

            /**
               Process thread smart pointer
            */
            typedef boost::shared_ptr<IThreadInfo> IThreadInfoPtr;


            /**
               @brief The DVB processor factory

               The class creates specific threads for processing
               DVB tasks

               There are 2 tasks
               - streaming
               - scaning
            */
            class IThreadFactory
            {
            public:
                /**
                   Destructor
                */
                virtual ~IThreadFactory(){}

                /**
                   Gets a StreamThread pointer

                   @param[in] dev - the dev info

                   @return pointer to the thread

                   @exception klk::Exception
                */
                virtual const IThreadInfoPtr
                    createStreamThread(const IDevPtr& dev) = 0;
            };

            /**
               Factory smart pointer
            */
            typedef boost::shared_ptr<IThreadFactory> IThreadFactoryPtr;

            /** @} */
        }
    }
}

#endif //KLK_ITHREADFACTORY_H
