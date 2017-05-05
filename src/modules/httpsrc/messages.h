/**
   @file messages.h
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
   - 2009/11/21 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_HTTPSRCMESSAGES_H
#define KLK_HTTPSRCMESSAGES_H

#include <string>
/** @defgroup grHttpsrcMessageID HTTP source module messages IDs
    @ingroup grMessageID

    Messages IDs
*/


namespace klk
{
    namespace msg
    {
        namespace id
        {
            /** @addtogroup grHttpsrcMessageID
                @{
            */

            /**
               Starts a httpsrc usage
            */
            const std::string HTTPSRCSTART = "d423db7d-b5c4-4bf2-8664-5981e674330a";

            /** @} */
        }

        namespace key
        {
            /** @addtogroup grHttpsrcMessageID
                @{
            */

            /**
               HTTP source address

               Value is something like http://www.webcam.com/camera1
            */
            const std::string HTTPSRC_ADDR = "httpsrc_addr";

            /**
               HTTP source login
            */
            const std::string HTTPSRC_LOGIN = "httpsrc_login";

            /**
               HTTP source password
            */
            const std::string HTTPSRC_PWD = "httpsrc_pwd";

            /** @} */
        }
    }
}

#endif //KLK_HTTPSRCMESSAGES_H
