/**
   @file messagesproxy.h
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
   - 2010/04/24 created by ipp (Ivan Murashko)
   - 2010/09/12 class name was changed: Proxy -> MessagesProxy by ipp
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MESSAGESPROXY_H
#define KLK_MESSAGESPROXY_H

#include "iproxy.h"
#include "ifactory.h"
#include "converter.h"
#include "adapter.h"

namespace klk
{
    namespace adapter
    {
        namespace ipc
        {
            /**
               @brief The proxy class for messaging IPC

               ICE proxy object that is used to send/recv messages

               @ingroup grAdapterModule
            */
            class MessagesProxy : public IMessagesProxy
            {
            public:
                /**
                   Constructor

                   @param[in] factory - the factory instance
                   @param[in] module - the adapter module instance
                */
                MessagesProxy(IFactory* factory, Adapter* module);

                /**
                   Destructor
                */
                virtual ~MessagesProxy(){}
            private:
                IFactory* const m_factory; ///< the factory instance
                Adapter* m_module; ///< the adapter module instance
                Converter m_converter; ///< converter class

                /**
                   Sends an async message

                   @param[in] msg - the message info container to be send
                   @param[in] crn - Ice current object that provides access
                   to information about the currently executing request to
                   the implementation of an operation in the server
                 */
                virtual void sendASync(const SMessage& msg,
                                       const Ice::Current& crn);

                /**
                   Sends a sync message

                   @param[in] msg - the message info container to be send
                   @param[in] crn - Ice current object that provides access
                   to information about the currently executing request to
                   the implementation of an operation in the server

                   @return the result message
                 */
                virtual SMessage sendSync(const SMessage& msg,
                                         const Ice::Current& crn);

                /**
                   Creates a message by its id

                   @param[in] id - the message id to be created
                   @param[in] crn - Ice current object that provides access
                   to information about the currently executing request to
                   the implementation of an operation in the server

                   @return the result message
                 */
                virtual SMessage getMessage(const std::string& id,
                                            const Ice::Current& crn);

                /**
                   Register a message

                   @param[in] msg_id - the message's id to be registered
                   @param[in] mod_id - the module's id that do the registration
                   @param[in] msg_type - the message's type

                   @exception klk::Exception
                */
                virtual void registerMessage(const std::string& msg_id,
                                             const std::string& mod_id, int msg_type,
                                             const Ice::Current& crn);
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                MessagesProxy(const MessagesProxy& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                MessagesProxy& operator=(const MessagesProxy& value);
            };
        }
    }
}

#endif //KLK_MESSAGESPROXY_H
