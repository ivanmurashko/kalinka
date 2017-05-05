/**
   @file netinfo.h
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
   - 2009/10/30 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_NETINFO_H
#define KLK_NETINFO_H

#include "transinfo.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief The network source (TCP/IP, UDP) info container

           The container for network source (TCP/IP, UDP) info

           @ingroup grTrans
        */
        class NetInfo : public SourceInfo
        {
        public:
            /// @copydoc klk::trans::SourceInfo
            NetInfo(const std::string& uuid,  const std::string& name,
                     const std::string& media_type);

            /// Destructor
            virtual ~NetInfo(){}
        private:
            SafeValue<Transcode*> m_module; ///< the module instance

            /**
               Inits the network source element assosiated with the source

               @param[in] module - the klk::trans::Transcode module instance

               @exception klk::Exception
            */
            virtual void initInternal(Transcode* module);

            /**
               Deinits the network source element assosiated with the source

               @exception klk::Exception
            */
            virtual void deinit() throw();

            /**
               Locks resource if it's necessry

               @param[in] module - the module

               @return the response message with data from
               klk::net::Network module
            */
            const IMessagePtr lockResource(Transcode* module);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            NetInfo(const NetInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            NetInfo& operator=(const NetInfo& value);
        };
    }
}

#endif //KLK_NETINFO_H
