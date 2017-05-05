/**
   @file streamchannel.h
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
   - 2008/11/03 created by ipp (Ivan Murashko)
   - 2009/01/04 some exceptions were added by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STREAMCHANNEL_H
#define KLK_STREAMCHANNEL_H

#include <map>
#include <list>

#include <boost/shared_ptr.hpp>

#include "ifactory.h"
#include "istreamchannel.h"
#include "db.h"
#include "thread.h"

namespace klk
{
    namespace dvb
    {
        /**
           @brief The channel info holder

           StreamChannel

           @ingroup grDVB
        */
        class StreamChannel : public IStreamChannel
        {
        public:
            /**
               Constructor

               @param[in] type - the dev type
               @param[in] source - the dev source
               @param[in] factory - the factory

            */
            StreamChannel(const std::string& type,
                          const std::string& source,
                          IFactory* factory);

            /**
               Destructor
            */
            virtual ~StreamChannel();

            /**
               Sets the data

               @param[in] channel_id - the id for data retrieving

               @exception @ref klk::Exception
            */
            void setData(const std::string& channel_id);

            /**
               Gets resource (device) assigned to the channel

               @return the resource
            */
            const IDevPtr getDev() const;

            /**
               Gets name

               @return the name
            */
            virtual const std::string getName() const throw();

            /**
               Gets number

               @return the number
            */
            virtual const std::string getNumber() const throw();

            /**
               Gets the channel id
            */
            const std::string getID() const throw();
        private:
            IFactory* const m_factory; ///< factory
            klk::SafeValue<std::string> m_channel_id; ///< the channel id
            klk::SafeValue<std::string> m_type; ///< dev type
            klk::SafeValue<std::string> m_source; ///< dev source
            klk::SafeValue<std::string> m_name; ///< channel name
            klk::SafeValue<std::string> m_number; ///< channel's number
            IDevPtr m_dev; ///< the dev

            /**
               tune DVB-S dev

               @exception klk::Exception
            */
            void setTune4DVBS();

            /**
               tune DVB-T dev

               @exception klk::Exception
            */
            void setTune4DVBT();

            /**
               Finds a dev by frequency
               Fills m_dev member

               @param[in] frequency - the used frequency
            */
            void findWorkingDev(const int frequency);

            /**
               Finds a not used dev
               Fills m_dev member
            */
            void findFreeDev();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            StreamChannel(const StreamChannel& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            StreamChannel& operator=(const StreamChannel& value);

        };

        /**
           StreamChannel smart pointer that can be used in the containers
        */
        typedef boost::shared_ptr<StreamChannel> StreamChannelPtr;

        /**
           Stream channel list defenition
        */
        typedef std::list<StreamChannelPtr> StreamChannelList;

        /**
           @brief The stream channels container

           The stream channles container

           StreamContainer
        */
        class StreamContainer
        {
        public:
            /**
               Constructor
            */
            StreamContainer();

            /**
               Destructor
            */
            virtual ~StreamContainer();

            /**
               Copy constructor
               @param[in] value - the copy param

               @note FIXME!!! used in utests only
            */
            StreamContainer(const StreamContainer& value);

            /**
               Removes stream channel

               @param[in] channel - the channel to be removed
            */
            void removeStreamChannel(const StreamChannelPtr& channel);

            /**
               Insert stream channel

               @param[in] channel - the channel to be inserted
            */
            void insertStreamChannel(const StreamChannelPtr& channel);

            /**
               Retrives stream channels list for the specified device

               @param[in] dev - the dev

               @return the list
            */
            const StreamChannelList getStreamChannels(const IDevPtr& dev) const;

            /**
               Clears the list
            */
            void clear() throw();

            /**
               Retrives stream channel

               @param[in] id - the channel id

               @return the stream channel or NULL if no such channel

               @exception klk::Exception
            */
            const StreamChannelPtr getStreamChannel(const std::string& id) const;

            /**
               Check is there empty container or not

               @note FIXME!!! used in utests only

               @return
               - true no any channels in the container
               - false at least one channel is in the container
            */
            bool empty() const throw();
        private:
            mutable klk::Mutex m_lock; ///< locker
            StreamChannelList m_list; ///< list with the channels
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            StreamContainer& operator=(const StreamContainer& value);
        };
    }
}

#endif //KLK_STREAMCHANNEL_H
