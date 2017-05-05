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
   - 2009/03/24 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_NETMESSAGES_H
#define KLK_NETMESSAGES_H

#include <string>

/** @defgroup grNetMessageID Network module messages IDs

    @ingroup grMessageID

    Messages IDs
*/
namespace klk
{
    namespace msg
    {
        namespace id
        {
            /** @addtogroup grNetMessageID
                @{
            */
            /**
               Message ID for network list for available routes
            */
            const std::string NETLIST =
                "08219724-89ff-4458-95c9-cb7dfca0dda2";

            /**
               Message ID for network tune
            */
            const std::string NETSTART =
                "15c599aa-ce63-4bcf-bc07-c01ec38baf9b";

            /**
               Message ID for network stop
            */
            const std::string NETSTOP =
                "946d4a08-64a4-4e23-bf7d-e18f6da5e32f";

            /**
               Message ID for network info

               The message just retrives info without any locking
               and as result does not require stop
            */
            const std::string NETINFO =
                "381fbb6f-1675-4ade-9f2e-60454915ac22";
            /** @} */
        }

        namespace key
        {
            /** @addtogroup grNetMessageID
                @{
            */

            /**
               Network protocol
            */
            const std::string NETPROTO = "net_protocol";

            /**
               Network host
            */
            const std::string NETHOST = "net_host";

            /**
               Network address
            */
            const std::string NETADDR = "net_addr";

            /**
               Network port
            */
            const std::string NETPORT = "net_port";

            /**
               Network type key
            */
            const std::string NETTYPE = "net_type";

            /**
               Network type unicast
            */
            const std::string NETUNICAST = "net_unicast";

            /**
               Network type unicast
            */
            const std::string NETMULTICAST = "net_multicast";
            /** @} */
        }
    }
}

#endif //KLK_NETMESSAGES_H
