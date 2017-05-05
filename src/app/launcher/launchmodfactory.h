/**
   @file launchmodfactory.h
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
   - 2010/09/25 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_LAUNCHMODFACTORY_H
#define KLK_LAUNCHMODFACTORY_H

#include "modfactory.h"
#include "launchbase.h"
#include "adapter/modulesprotocol.h"
#include "adapter/messagesprotocol.h"

namespace klk
{
    namespace launcher
    {
        /**
           @brief Module factory

           The module factory for launcher application. Some part of actions
           are performed via RPC.

           @ingroup grLauncher
        */
        class ModuleFactory : public klk::ModuleFactory,
            private klk::launcher::Base
        {
            friend class TestModules;
        public:
            /**
               Constructor

               @param[in] factory - the main factory instance
            */
            explicit ModuleFactory(IFactory* factory);

            /// Destructor
            virtual ~ModuleFactory(){}
        private:
            adapter::ModulesProtocol m_proto; ///< RPC communication protocol
            adapter::MessagesProtocol m_msgproto; ///< RPC com. protocol

            /// @copydoc klk::IModuleFactory::load
            virtual void load(const std::string& id);

            /// @copydoc klk::IModuleFactory::unload
            virtual void unload(const std::string& id);

            /// @copydoc klk::IModuleFactory::isLoaded
            virtual bool isLoaded(const std::string& id);

            /// @copydoc klk::IModuleFactory::sendMessage
            virtual void sendMessage(const IMessagePtr& msg);

            /**
               @copydoc klk::IModuleFactory::getResources

               @note the application module should not have any resources
               assosiated with it. Thus the method always return NULL
             */
            virtual const IResourcesPtr getResources(const std::string& id)
            {
                return IResourcesPtr();
            }
        private:
            /// Fake copy constructor
            ModuleFactory(const ModuleFactory&);

            /// Fake assigment operator
            ModuleFactory& operator=(const ModuleFactory&);
        };
    }
}

#endif //KLK_LAUNCHMODFACTORY_H
