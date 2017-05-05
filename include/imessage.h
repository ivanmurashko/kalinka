/**
   @file imessage.h
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
   - 2008/09/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_IMESSAGE_H
#define KLK_IMESSAGE_H

#include <sys/types.h>

#include <boost/shared_ptr.hpp>

#include "stringlist.h"
#include "common.h"
#include "msg.h"

namespace klk
{
    /** @defgroup grMessage Mediaserver messaging system

        Messaging system

        @{
    */
    namespace msg
    {
        /**
           Message types
           @ingroup grMessage
        */
        typedef enum
        {
            UNDEF = 0,
            ASYNC = 1,
            SYNC_REQ = 2,
            SYNC_RES = 3
        } Type;

        /** @defgroup grMessageID Messages IDs

            Messages IDs

            @{
        */

        /// Message UUID type
        typedef u_int UUID;

        namespace id
        {
            /** @addtogroup grMessageID
                @{
            */

            /// Special id for all messages
            const std::string ALL =
                "5a037874-d520-4ff8-9265-f7fc33450002";

            /// Message ID for DB change event (message)
            const std::string CHANGEDB =
                "1b2191e3-8286-4d76-b126-0353606432df";

            /// Message for module intialization with DB data
            const std::string INITFROMDB =
                "c1dccf2e-fa21-4b85-aa76-770bad501692";

            /** @} */
        }
        /** @} */
    }

    /**
       @brief Message interface

       The message interface
    */
    class IMessage
    {
    public:
        /**
           Destructor
        */
        virtual ~IMessage(){}

        /**
           Gets the message's id

           @return the id or NULL if there was an error
        */
        virtual const std::string getID() const = 0;

        /**
           Gets the list with modules IDs that should get the message

           @return the the list
        */
        virtual const StringList getReceiverList() const throw() = 0;

        /**
           Adds a new receiver to the list

           @param[in] id - the receiver's id
        */
        virtual void addReceiver(const std::string& id) = 0;

        /**
           Clears receiver list

           @note FIXME!!! is it really necessary think about removing it
        */
        virtual void clearReceiverList() throw() = 0;

        /**
           Checks message specific data

           @param[in] key - the key name (data id)

           @return
           - true - the key keeps data
           - false - no data for the key
        */
        virtual bool hasValue(const std::string& key) const = 0;

        /**
           Gets message specific data (simple value)

           @param[in] key - the key name (data id)

           @return the data
        */
        virtual const std::string getValue(const std::string& key) const = 0;

        /**
           Gets message specific data (list)

           @param[in] key - the key name (data id)

           @return the data
        */
        virtual const StringList getList(const std::string& key) const = 0;


        /**
           Sets message specific data (simple value)

           @param[in] data - the data to be set
           @param[in] key - the key name (data id)
        */
        virtual void
            setData(const std::string& key, const std::string& data) = 0;

        /**
           Sets message specific data (list data)

           @param[in] data - the data to be set
           @param[in] key - the key name (data id)
        */
        virtual void
            setData(const std::string& key, const StringList& data) = 0;


        /**
           Gets message type

           @return the type
        */
        virtual msg::Type getType() const = 0;

        /**
           Sets message type

           @param[in] type - the type to be set
        */
        virtual void setType(msg::Type type) = 0;

        /**
           Gets sender module ID

           @return the ID
        */
        virtual const std::string getSenderID() const = 0;

        /**
           Sets sender module ID

           @param[in] id - the id to be set
        */
        virtual void setSenderID(const std::string& id) = 0;

        /**
           Gets message UUID

           @return  the uuid
        */
        virtual msg::UUID getUUID() = 0;
    };

    /**
       Message smart pointer
    */
    typedef boost::shared_ptr<IMessage> IMessagePtr;

    /** @} */

}

#endif //KLK_IMESSAGE_H
