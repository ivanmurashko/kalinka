/**
   @file httpinfo.h
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
   - 2009/12/19 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_HTTPINFO_H
#define KLK_HTTPINFO_H

#include "mod/info.h"

namespace klk
{
    namespace httpsrc
    {
        /**
           @brief The HTTP source info holder

           The class keeps info about an HTTP source. The info includes
           - http address
           - login
           - password

           @ingroup grHTTPSrc
        */
        class HTTPInfo : public mod::Info
        {
        public:
            /**
               Constructor

               @param[in] uuid - the file uuid
               @param[in] name - the name
               @param[in] addr - the http address
               @param[in] login - the authentication login
               @param[in] passwd - the authentication password
            */
            HTTPInfo(const std::string& uuid,
                     const std::string& name,
                     const std::string& addr,
                     const std::string& login,
                     const std::string& passwd);

            /// Destructor
            virtual ~HTTPInfo(){}

            /// @copydoc klk::mod::Info::fillOutMessage
            virtual void fillOutMessage(const IMessagePtr& out) const;

            /// @copydoc klk::mod::Info::updateInfo
            virtual void updateInfo(const mod::InfoPtr& value);
        private:
            std::string m_addr; ///< http address
            std::string m_login; ///< http authentication login
            std::string m_passwd; ///< http authentication password
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            HTTPInfo(const HTTPInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            HTTPInfo& operator=(const HTTPInfo& value);
        };

        /**
           Smart pointer for http info

           @ingroup grHTTPSrc
        */
        typedef boost::shared_ptr<HTTPInfo> HTTPInfoPtr;
    }
}

#endif //KLK_HTTPINFO_H
