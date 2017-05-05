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

#ifndef KLK_SRVMESSAGES_H
#define KLK_SRVMESSAGES_H

#include <string>


namespace klk
{
    namespace msg
    {
        namespace id
        {
            /** @defgroup grSrvMessageID Service module messages IDs

                Messages ids used in service module

                @ingroup grSysModule
            */

            /// Message ID for module loading check in the service module
            const std::string SRVMODCHECK =
                "831f88e7-e420-446e-8c10-82e5219215a6";

            /// Message ID to send info from external applications
            const std::string SRVMODINFO =
                "978e8a9d-de2e-4d2c-beba-4d9cba1a0eef";

            /** @} */
        }

        namespace key
        {
            /** @addtogroup grSrvMessageID
                @{
            */
            /// Module id field
            const std::string SRVMODID = "srv_mod_id";

            /// Module id status field
            const std::string SRVMODSTATUS = "srv_mod_status";

            /// CPU usage field
            const std::string SRVMODCPUUSAGE = "srv_mod_cpuusage";

            /** @} */
        }
    }
}



#endif //KLK_SRVMESSAGES_H
