/**
   @file msgfactory.h
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
   - 2008/07/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MSGFACTORY_H
#define KLK_MSGFACTORY_H

#include <map>
#include <string>

#include "ifactory.h"
#include "thread.h"
#include "stringlist.h"

namespace klk
{
    /**
       @brief IMessageFactory impl.

       message factory implementation

       @ingroup grMessage
    */
    class MessageFactory : public IMessageFactory, public klk::Mutex
    {
    public:
        /**
           Constructor
        */
        MessageFactory();

        /**
           Destructor
        */
        virtual ~MessageFactory();


        /**
           Gets message by its id

           @param[in] id the message's id

           @return the pointer to a message or NULL if there was an error
        */
        virtual IMessagePtr getMessage(const std::string& id);

        /**
           Gets response message

           construct it from the input arg

           @param[in] request - the request message

           @return the response message

           @exception klk::Exception - there was an error while retriving
        */
        virtual IMessagePtr getResponse(const IMessagePtr& request) const;

        /// @copydoc klk::IMessageFactory::registerMessage
        virtual void registerMessage(const std::string& msg_id,
                                     const std::string& mod_id,
                                     msg::Type msg_type);

        /**
           UnRegister a message

           @param[in] msg_id - the message's id to be unregistered
           @param[in] mod_id - the module's id that do the unregistration
        */
        virtual void unregisterMessage(const std::string& msg_id,
                                       const std::string& mod_id) throw();

        /**
           Checks if the message register or not

           @param[in] id - the message id

           @return
           - true
           - false
        */
        virtual bool hasMessage(const std::string& id);
    private:
        /**
           Sync messages info holder
        */
        typedef std::map<std::string, std::string> SyncMessageInfoMap;

        /**
           Sync messages info holder
        */
        typedef std::map<std::string, StringList> ASyncMessageInfoMap;

        u_int m_uuid; ///< uuid holder
        SyncMessageInfoMap m_sync_container; ///< sync info container
        ASyncMessageInfoMap m_async_container; ///< async info container

        ///< @copydoc klk::IMessageFactory::generateUUID()
        virtual msg::UUID generateUUID();
    private:
        /**
           Copy constructor
        */
        MessageFactory(const MessageFactory&);

        /**
           Assignment opearator
        */
        MessageFactory& operator=(const MessageFactory&);
    };
};

#endif //KLK_MSGFACTORY_H
