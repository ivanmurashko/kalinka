/**
   @file modulesproxy.h
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

#ifndef KLK_MODULESPROXY_H
#define KLK_MODULESPROXY_H

#include "ifactory.h"
#include "iproxy.h"

namespace klk
{
    namespace adapter
    {
        namespace ipc
        {
            /**
               @brief RPC interface to klk::IModuleFactory

               The IModuleProxy iface implementation

               @ingroup grAdapterModule
            */
            class ModulesProxy : public IModulesProxy
            {
            public:
                /**
                   Constructor

                   @param[in] factory - the factory instance
                */
                ModulesProxy(IFactory* factory);

                /// Destructor
                virtual ~ModulesProxy(){}
            private:
                IModuleFactory* m_factory; ///< the module factory instance

                /**
                   Loads a module

                   @param[in] id - the module id to be loaded
                   @param[in] crn - Ice current object that provides access
                   to information about the currently executing request to
                   the implementation of an operation in the server
                 */
                virtual void load(const std::string& id,
                                  const Ice::Current& crn);

                /**
                   Unloads a module

                   @param[in] id - the module id to be unloaded
                   @param[in] crn - Ice current object that provides access
                   to information about the currently executing request to
                   the implementation of an operation in the server
                 */
                virtual void unload(const std::string& id,
                                    const Ice::Current& crn);

                /**
                   Checks is the module loaded or not

                   @param[in] id - the module id to be checked
                   @param[in] crn - Ice current object that provides access
                   to information about the currently executing request to
                   the implementation of an operation in the server

                   @return
                   - true - the module has been loaded
                   - false - the module has not been loaded yet
                */
                virtual bool isLoaded(const std::string& id,
                                      const Ice::Current& crn);
            private:
                /// Fake copy constructor
                ModulesProxy(const ModulesProxy&);

                /// Fake assigment operator
                ModulesProxy& operator=(const ModulesProxy&);
            };
        }
    }
}

#endif //KLK_MODULESPROXY_H
