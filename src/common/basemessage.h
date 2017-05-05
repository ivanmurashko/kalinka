/**
   @file basemessage.h
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
   - 2010/05/02 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_BASEMESSAGE_H
#define KLK_BASEMESSAGE_H

#include "imessage.h"
#include "thread.h"

namespace klk
{
    /**
       @brief Base class for klk::IMessage realizations

       There are implementation common for different klk::IMessage impl.

       @ingroup grModules
    */
    class BaseMessage : public IMessage
    {
    public:
        /**
           Constructor

           @param[in] id - the message id
           @param[in] uuid - the message uuid
           @param[in] sender_id - the sender id
        */
        BaseMessage(const std::string& id, const msg::UUID& uuid,
                    const std::string& sender_id = std::string());

        /**
           Destructor
        */
        virtual ~BaseMessage(){}
    protected:
        mutable Mutex m_lock; ///< sync object to make the class reentrant

        /// @copydoc klk::IMessage::getID()
        virtual const std::string getID() const {return m_id;}

        /// @copydoc klk::IMessage::getSenderID()
        virtual const std::string getSenderID() const;
    private:
        const std::string m_id; ///< message id
        const msg::UUID m_uuid; ///< message UUID

        msg::Type m_type; ///< message type
        std::string m_sender_id; ///< sender's id

        /// @copydoc klk::IMessage::getUUID()
        virtual msg::UUID getUUID(){return m_uuid;}

        /// @copydoc klk::IMessage::getType()
        virtual msg::Type getType() const;

        /// @copydoc klk::IMessage::setType()
        virtual void setType(msg::Type type);

        /// @copydoc klk::IMessage::setSenderID()
        virtual void setSenderID(const std::string& id);
    };
}

#endif //KLK_BASEMESSAGE_H
