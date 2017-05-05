/**
   @file messagesprotocol.h
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
   - 2010/09/19 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MESSAGESPROTOCOL_H
#define KLK_MESSAGESPROTOCOL_H

#include "imessage.h"
#include "iproxy.h"
#include "converter.h"

#include "protocol.h"

namespace klk
{
    namespace adapter
    {
        /**
           @brief Messages RPC protocol

           Provides an acces to klk::adapter::obj::MESSAGES object

           @ingroup grAdapterModuleObjects
        */
        class MessagesProtocol : public Protocol<ipc::IMessagesProxyPrx>
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory instance
               @param[in] main_module_id - the id of main module. The default
               module is klk::MODULE_COMMON_ID but sometimes
               we need to send a message to a not main application
            */
            explicit MessagesProtocol(IFactory* factory,
                                      const std::string& main_module_id =
                                      MODULE_COMMON_ID);

            /// Destructor
            virtual ~MessagesProtocol(){}

            /**
               Sends an async message via the protocol

               @param[in] msg - the message to be sent

               @exception klk::Exception
            */
            void sendASync(const IMessagePtr& msg);

            /**
               Sends a sync message via the protocol

               @param[in] msg - the message to be sent

               @return the message

               @exception @ref klk::Exception
            */
            const IMessagePtr sendSync(const IMessagePtr& msg);

            /**
               Creates a message by id

               @param[in] id - the message id to be created

               @return the message
            */
            IMessagePtr getMessage(const std::string& id);

            /**
               Register a message

               @param[in] msg_id - the message's id to be registered
               @param[in] mod_id - the module's id that do the registration
               @param[in] msg_type - the message's type

               @exception klk::Exception
            */
            void registerMessage(const std::string& msg_id,
                                 const std::string& mod_id,
                                 msg::Type msg_type);
        private:
            Converter m_converter; /// ICE <-> IMessagePtr converter
        private:
            /// Fake copy constructor
            MessagesProtocol(const MessagesProtocol&);

            /// Fake assigment operator
            MessagesProtocol& operator=(const MessagesProtocol&);
        };
    }
}

#endif //KLK_MESSAGESPROTOCOL_H
