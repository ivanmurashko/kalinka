/**
   @file commontraps.h
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
   - 2007/05/15 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_COMMONTRAP_H
#define KLK_COMMONTRAP_H

#include <string>

namespace klk
{
    namespace snmp
    {

        /** @defgroup grCommonTrap Common traps
            @brief Defenitions for common traps

            Defenitions for common traps

            @ingroup grSNMP

            @{
        */

        /**
           @brief OID for db failed

           db failed
        */
        const std::string DB_FAILED = "KLK-MIB::klkDBFailed";

        /**
           @brief Module load failed

           Module load failed trap
        */
        const std::string MODULE_LOAD_FAILED = "KLK-MIB::klkModuleLoadFailed";

        /**
           @brief Module does not respond

           Trap for module does not respond event
        */
        const std::string MODULE_DNT_RESPONDE = "KLK-MIB::klkModuleDontResponde";

        /**
           @brief Module does not respond

           Trap for module does not respond event
        */
        const std::string GSTPLUGINMISSING = "KLK-MIB::klkGSTPluginMissing";
        /** @} */
    }
}


#endif //KLK_COMMONTRAP_H
