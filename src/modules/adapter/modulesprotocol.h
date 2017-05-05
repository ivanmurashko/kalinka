/**
   @file modulesprotocol.h
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
   - 2010/09/26 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MODULESPROTOCOL_H
#define KLK_MODULESPROTOCOL_H

#include "iproxy.h"
#include "protocol.h"

namespace klk
{
    namespace adapter
    {
        /**
           @brief Modules RPC protocol

           Provides an acces to klk::adapter::obj::MODULES object

           @ingroup grAdapterModuleObjects
        */
        class ModulesProtocol : public Protocol<ipc::IModulesProxyPrx>
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory instance
            */
            explicit ModulesProtocol(IFactory* factory);

            /// Destructor
            virtual ~ModulesProtocol(){}

            /**
               Loads a module

               @param[in] id - the module id to be loaded
            */
            void load(const std::string& id);

            /**
               Unloads a module

               @param[in] id - the module id to be unloaded
            */
            void unload(const std::string& id);

            /**
               Checks is the module loaded or not

               @param[in] id - the module id to be checked

               @return
               - true - the module has been loaded
               - false - the module has not been loaded yet
            */
            bool isLoaded(const std::string& id);
        private:
            /// Fake copy constructor
            ModulesProtocol(const ModulesProtocol&);

            /// Fake assigment operator
            ModulesProtocol& operator=(const ModulesProtocol&);
        };
    }
}

#endif //KLK_MODULESPROTOCOL_H
