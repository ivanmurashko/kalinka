/**
   @file resourcesprotocol.h
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

#ifndef KLK_RESOURCESPROTOCOL_H
#define KLK_RESOURCESPROTOCOL_H

#include "iproxy.h"
#include "protocol.h"

namespace klk
{
    namespace adapter
    {
        /**
           @brief Resources RPC protocol

           Provides an acces to klk::adapter::obj::RESOURCES object

           @ingroup grAdapterModuleObjects
        */
        class ResourcesProtocol : public Protocol<ipc::IResourcesProxyPrx>
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory instance
            */
            explicit ResourcesProtocol(IFactory* factory);

            /// Destructor
            virtual ~ResourcesProtocol(){}

            ///@copydoc klk::IResources::hasDev
            bool hasDev(const std::string& type);

            /**
               Gets the resource id by its name

               @param[in] name - the resource's name

               @return the dev uuid
            */
            const std::string getResourceByName(const std::string& name);

            /**
               Gets a list of devices ids by it's type

               @param[in] type - the dev's type

               @return the list
            */
            StringList getResourceByType(const std::string& type);
        private:
            /// Fake copy constructor
            ResourcesProtocol(const ResourcesProtocol&);

            /// Fake assigment operator
            ResourcesProtocol& operator=(const ResourcesProtocol&);
        };
    }
}

#endif //KLK_RESOURCESPROTOCOL_H
