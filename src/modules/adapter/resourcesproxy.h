/**
   @file resourcesproxy.h
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
   - 2010/09/12 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_RESOURCESPROXY_H
#define KLK_RESOURCESPROXY_H

#include "iproxy.h"
#include "ifactory.h"

namespace klk
{
    namespace adapter
    {
        namespace ipc
        {
            /**
               @brief The proxy class for resources IPC

               The class is used for resource info sharing

               @ingroup grAdapterModule
            */
            class ResourcesProxy : public IResourcesProxy
            {
            public:
                /**
                   Constructor

                   @param[in] factory - the factory instance
                */
                ResourcesProxy(IFactory* factory);

                /// Destructor
                virtual ~ResourcesProxy(){}
            private:
                IFactory* m_factory; ///< the factory instance

                /**
                   Check is there the specified dev type

                   @param[in] type - the dev's type to be checked
                */
                virtual bool hasDev(const ::std::string& type,
                                    const Ice::Current&);

                /**
                   Gets the resource id by its name

                   @param[in] name - the resource's name

                   @return the dev uuid
                */
                std::string getResourceByName(const std::string& name, const Ice::Current&);

                /**
                   Gets a list of devices ids by it's type

                   @param[in] type - the dev's type

                   @return the list
                */
                std::vector<std::string>
                    getResourceByType(const std::string& type, const Ice::Current&);
            private:
                /// Fake copy constructor
                ResourcesProxy(const ResourcesProxy&);

                /// Fake assigment operator
                ResourcesProxy& operator=(const ResourcesProxy&);
            };
        }
    }
}

#endif //KLK_RESOURCESPROXY_H
