/**
   @file traps.h
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/12/20 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TRAPS_H
#define KLK_TRAPS_H

namespace klk
{
    namespace dvb
    {
        /** @defgroup grDVBTrap DVB module related traps
            @brief Defenitions for DVB traps

            There is a list of traps that can be send by
            @ref grDVB "DVB module"

            @ingroup grSNMP

            @{
        */
        /**
           @brief OID for check timeout

           Check timeout
        */
        const std::string TRAP_TIMEOUT = "KLK-DVB-MIB::klkTimeOut";

        /**
           @brief OID for check hasLock

           Check hasLock
        */
        const std::string TRAP_NOSIGNAL = "KLK-DVB-MIB::klkNoSignal";


        /**
           @brief OID for check signal

           Bad signal
        */
        const std::string TRAP_BADSIGNAL = "KLK-DVB-MIB::klkBadSignal";

        /**
           @brief OID for check SNR

           Bad SNR
        */
        const std::string TRAP_BADSNR = "KLK-DVB-MIB::klkBadSNR";

        /**
           @brief OID for check BER

           Bad BER
        */
        const std::string TRAP_BADBER = "KLK-DVB-MIB::klkBadBER";

        /**
           @brief OID for check UNC

           Bad UNC
        */
        const std::string TRAP_BADUNC = "KLK-DVB-MIB::klkBadUNC";

        /**
           @brief OID for check Device lost trap

           Device does not exist
        */
        const std::string TRAP_DEVLOST = "KLK-DVB-MIB::klkDevLost";

        /** @} */
    }
}


#endif //KLK_TRAPS_H
