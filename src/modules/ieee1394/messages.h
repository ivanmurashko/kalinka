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
   - 2009/03/27 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_IEEE1394MESSAGES_H
#define KLK_IEEE1394MESSAGES_H

#include <string>

/** @defgroup grIeee1394MessageID Ieee1394 module messages IDs

    @ingroup grMessageID

    Messages IDs for IEEE1394 module
*/

namespace klk
{
    namespace msg
    {
        namespace id
        {
            /** @addtogroup grIeee1394MessageID
                @{
            */

            /**
               Starts a ieee1394 usage
            */
            const std::string IEEE1394START("7a211098-5234-4b64-8ab5-aa4204c246a1");

            /**
               Stops a ieee1394 usage
            */
            const std::string IEEE1394STOP("f80b9841-95bb-40bb-8b38-8746bdf49408");

            /**
               A state of a DV camera was changed
            */
            const std::string IEEE1394DEV("bb52629f-9aee-4ead-8574-86580db09d0b");
            /** @} */
        };


        namespace key
        {
            /** @addtogroup grIeee1394MessageID
                @{
            */

            /**
               Ieee1394 dev state key
            */
            const std::string IEEE1394STATE("ieee1394_state");

            /**
               Ieee1394 dev state new
            */
            const std::string IEEE1394STATENEW("ieee1394_state_new");

            /**
               Ieee1394 dev state removed
            */
            const std::string IEEE1394STATEDEL("ieee1394_state_del");

            /**
               Ieee1394 port
            */
            const std::string IEEE1394PORT("ieee1394_port");

            /** @} */
        }
    }
}

#endif //KLK_IEEE1394MESSAGES_H
