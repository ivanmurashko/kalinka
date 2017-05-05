/**
   @file processor.h
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
   - 2009/06/17 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_DVBPROCESSOR_H
#define KLK_DVBPROCESSOR_H

#include <boost/shared_ptr.hpp>

#include "ifactory.h"
#include "streamchannel.h"
#include "thread.h"

namespace klk
{
    namespace dvb
    {
        /**
           @brief Processes requests to the DVB module

           Processes requests to the DVB module

           @ingroup grDVB
        */
        class Processor
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            Processor(IFactory* factory);

            /**
               Destructor
            */
            virtual ~Processor();

            /**
               Does cleanup
            */
            void clean();

            /**
               Retrives a channel list for the specified device

               @param[in] dev - the dev itself

               @return the list
            */
            const StringList getChannelNames(const IDevPtr& dev) const;

            /**
               Do the tune startup

               @param[in] in - the input message
               @param[out] out - the output message

               @exception @ref klk::Exception
            */
            void doStart(const IMessagePtr& in,
                         const IMessagePtr& out);

            /**
               Do the tune stop

               @param[in] in - the input message

               @exception @ref klk::Exception
            */
            void doStop(const IMessagePtr& in);

            /**
               Processes signal lost event

               @param[in] dev - the dev produced the signal lost event
            */
            void doSignalLost(const IDevPtr& dev) throw();
        private:
            IFactory* const m_factory; ///< factory
            StreamContainer m_channels; ///< channels holder
            klk::Mutex m_tune_lock; ///< tune lock

            /**
               Do the tune startup for streaming activity

               @param[in] in - the input message
               @param[out] out - the output message

               @exception @ref klk::Exception
            */
            void doStartStream(const IMessagePtr& in,
                               const IMessagePtr& out);

            /**
               Do the streaming tune stop

               @param[in] in - the input message

               @exception @ref klk::Exception
            */
            void doStopStream(const IMessagePtr& in);

            /**
               Do the tune startup for scaning activity

               @param[in] in - the input message
               @param[out] out - the output message

               @exception @ref klk::Exception
            */
            void doStartScan(const IMessagePtr& in,
                             const IMessagePtr& out);

            /**
               Do the scaning tune stop

               @param[in] in - the input message

               @exception @ref klk::Exception
            */
            void doStopScan(const IMessagePtr& in);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Processor(const Processor& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Processor& operator=(const Processor& value);
        };

        /**
           Processor smart pointer

           @ingroup grDVB
        */
        typedef boost::shared_ptr<Processor> ProcessorPtr;
    }
}

#endif //KLK_DVBPROCESSOR_H
