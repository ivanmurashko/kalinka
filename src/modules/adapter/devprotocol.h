/**
   @file devprotocol.h
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

#ifndef KLK_DEVPROTOCOL_H
#define KLK_DEVPROTOCOL_H

#include "iproxy.h"
#include "protocol.h"

namespace klk
{
    namespace adapter
    {
        /**
           @brief Dev RPC protocol

           Provides an acces to klk::adapter::obj::DEV object

           @ingroup grAdapterModuleObjects
        */
        class DevProtocol : public Protocol<ipc::IDevProxyPrx>
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory instance
            */
            explicit DevProtocol(IFactory* factory);

            /// Destructor
            virtual ~DevProtocol(){}

            /**
               Retrives the dev state

               @param[in] uuid - the dev uuid

               @return the state of the device
            */
            klk::dev::State getState(const std::string& uuid);

            /**
               Sets device state

               @param[in] uuid - the dev uuid

               @param[in] state - the state value to be set
            */
            void setState(const std::string& uuid, klk::dev::State state);

            /**
               Tests is there a specified parameter available or not

               @param[in] uuid - the dev uuid
               @param[in] key - the key

               @return
               - true the parametr with the specified key exists
               - false the parametr with the specified key does not exist
            */
            bool hasParam(const std::string& uuid, const std::string& key);

            /**
               Gets dev info by it's key

               @param[in] uuid - the dev uuid
               @param[in] key - the key

               @return the parameter value

               @exception @ref klk::Exception
            */
            const std::string getStringParam(const std::string& uuid,   const std::string& key);

            /**
               Gets dev info by it's key

               @param[in] uuid - the dev uuid
               @param[in] key - the key

               @return the parameter value

               @exception @ref klk::Exception
            */
            int getIntParam(const std::string& uuid,  const std::string& key);

            /**
               Sets a value for a dev's parameter

               @param[in] uuid - the dev uuid
               @param[in] key - the key for retriiving
               @param[in] value - the value to be set
               @param[in] mutable_flag - do update (false) the last or not (true)

               @see IDev::getIntParam
               @see IDev::getStringParam
            */
            void setParam(const std::string& uuid,
                          const std::string& key,
                          const std::string& value,
                          bool mutable_flag);

            /**
               Sets a value for a dev's parameter

               @param[in] uuid - the dev uuid
               @param[in] key - the key for retriiving
               @param[in] value - the value to be set
               @param[in] mutable_flag - do update (false) the last or not (true)

               @see IDev::getIntParam
               @see IDev::getStringParam
            */
            void setParam(const std::string& uuid,
                          const std::string& key, int value,
                          bool mutable_flag);

            /**
               Gets last update time

               @param[in] uuid - the dev uuid

               @return the time
            */
            time_t getLastUpdateTime(const std::string& uuid);
        private:
            /// Fake copy constructor
            DevProtocol(const DevProtocol&);

            /// Fake assigment operator
            DevProtocol& operator=(const DevProtocol&);
        };
    }
}

#endif //KLK_DEVPROTOCOL_H
