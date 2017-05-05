/**
   @file iprocessor.h
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
   - 2009/11/04 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_IPROCESSOR_H
#define KLK_IPROCESSOR_H

#include <string>
#include <boost/shared_ptr.hpp>

namespace klk
{
    namespace gst
    {

        /** @defgroup grGST Working with GStreamer framework
            @brief Working with GStreamer framework

            There are classes and interfaces that are used for working
            with GStreamer framework

            @ingroup grCommon

            @{
        */

        /**
           Event types
        */
        typedef enum
        {
            ERROR = 0,
            EOS = 1
        } EventType;

        /**
           @brief The event info container

           The interface provides an info about event.
        */
        class IEvent
        {
        public:
            /**
               Destructor
            */
            virtual ~IEvent(){}

            /**
               Retrives event type assosiated with the holder
            */
            virtual EventType getType() const = 0;

            /**
               Retrives the gst thread uuid
            */
            virtual const std::string getThreadUUID() const = 0;

            /**
               Sets duration for the event

               @param[in] duration - the duration
               before the event processing
            */
            virtual void setDuration(time_t duration) = 0;

            /**
               Retrives duration for the event

               @return time interval left before execution
            */
            virtual time_t getTimeLeft() const = 0;
        };

        /**
           IEvent smart pointer
        */
        typedef boost::shared_ptr<IEvent> IEventPtr;

        /**
           @brief Processor for GST threads

           The class is used for callback processing
        */
        class IProcessor
        {
        public:
            /**
               Destructor
            */
            virtual ~IProcessor(){}

            /**
               Adds an event for processing

               @param[in] event - the event to be processed
            */
            virtual void addEvent(const IEventPtr& event) = 0;
        };

        /**
           IProcessor smart pointer
        */
        typedef boost::shared_ptr<IProcessor> IProcessorPtr;
        /** @} */
    }
}

#endif //KLK_IPROCESSOR_H
