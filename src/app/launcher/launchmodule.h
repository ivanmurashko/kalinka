/**
   @file launchmodule.h
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
   - 2010/09/30 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_LAUNCHMODULE_H
#define KLK_LAUNCHMODULE_H

#include "appmodule.h"
#include "launchbase.h"
#include "adapter/messagesprotocol.h"

namespace klk
{
    namespace launcher
    {
        /**
           @brief Application module

           There is an klk::app:Module implementation
           that is sutable for working as a remote application

           @ingroup grLauncher
        */
        class Module : public klk::app::Module, public klk::launcher::Base
        {
        public:
            /// @copydoc klk::app::Module::Module()
            Module(IFactory* factory,
                   const std::string& modid,
                   const std::string& setmsgid,
                   const std::string& showmsgid);

            /// Destructor
            virtual ~Module();

            /**
               @copydoc klk::IModule::sendSyncMessage

               @note sends a sync message via RPC
            */
            virtual void sendSyncMessage(const IMessagePtr& in,
                                         IMessagePtr& out);
        protected:
            /// @copydoc klk::IModule::registerProcessors()
            virtual void registerProcessors();
        private:
            adapter::MessagesProtocol* m_proto; ///< RPC communication protocol
            klk::Mutex m_proto_lock; ///< mutex for locking

            /// @return the protocol instance
            adapter::MessagesProtocol* getProtocol();

            /// sends CPU usage info to main service module
            void sendCPUUsageInfo();
        private:
            /// Fake copy constructor
            Module(const Module&);

            /// Fake assigment operator
            Module& operator=(const Module&);
        };
    }
}

#endif //KLK_LAUNCHMODULE_H
