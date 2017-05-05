/**
   @file launchmsgfactory.h
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
   - 2010/09/27 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_LAUNCHMSGFACTORY_H
#define KLK_LAUNCHMSGFACTORY_H

#include "msgfactory.h"
#include "launchbase.h"
#include "adapter/messagesprotocol.h"

namespace klk
{
    namespace launcher
    {
        /**
           @brief Message factory

           The messages factory for launcher application. Some part of actions
           are performed via RPC.

           @ingroup grLauncher
        */
        class MessageFactory : public klk::MessageFactory,
            private klk::launcher::Base
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory instance
            */
            explicit MessageFactory(IFactory* factory);

            /// Destructor
            virtual ~MessageFactory(){}
        private:
            /// list with messages that should be considered as local
            std::list<std::string> m_local_messages;

            adapter::MessagesProtocol m_proto; ///< RPC proxy to make request remotelly

            /// @copydoc klk::IMessageFactory::getMessage
            virtual IMessagePtr getMessage(const std::string& id);

            /// @copydoc klk::IMessageFactory::registerMessage
            virtual void registerMessage(const std::string& msg_id,
                                         const std::string& mod_id,
                                         msg::Type msg_type);

            /**
               Checks is the requested message should be considered as local or not

               @param[in] msgid - the tested message id

               @return
               - true - the message is local
               - false - the message is not local
            */
            bool isMessageLocal(const std::string& msgid) const
            {
                return (std::find(m_local_messages.begin(), m_local_messages.end(), msgid) !=
                        m_local_messages.end());
            }
        private:
            /// Fake copy constructor
            MessageFactory(const MessageFactory&);

            /// Fake assigment operator
            MessageFactory& operator=(const MessageFactory&);
        };
    }
}

#endif //KLK_LAUNCHMSGFACTORY_H
