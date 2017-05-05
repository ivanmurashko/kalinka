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

#ifndef KLK_DVBMESSAGES_H
#define KLK_DVBMESSAGES_H

#include <string>

/** @defgroup grDVBMessageID DVB module messages IDs

    @ingroup grMessageID

    Messages IDs
*/
namespace klk
{
    namespace msg
    {
        namespace id
        {
            /** @addtogroup grDVBMessageID
                @{
            */

            /**
               Message ID for DVB signal lost
            */
            const std::string DVBSIGNALLOST =
                "6351f9d5-12f5-4526-afbb-ab4435629528";

            /**
               Message ID for DVB startup tune
            */
            const std::string DVBSTART =
                "12d05b41-c1eb-4d15-9992-5e741e589a0b";

            /**
               Message ID for DVB stop tune
            */
            const std::string DVBSTOP =
                "cfcef94e-04e6-45aa-81b0-f02c96c0dadb";
            /** @} */
        }

        namespace key
        {
            /** @addtogroup grDVBMessageID
                @{
            */

            /**
               DVB activity key
            */
            const std::string DVBACTIVITY("dvb_activity");

            /**
               DVB source key
               @see klk::dev::SOURCE
            */
            const std::string DVBSOURCE("dvb_source");

            /**
               TV channel number
            */
            const std::string TVCHANNELNO = "tvchannel_no";

            /**
               TV channel name
            */
            const std::string TVCHANNELNAME = "tvchannel_name";

            /**
               TV channel id (key name)
            */
            const std::string TVCHANNELID = "tvchannel_id";

            /** @} */
        }
    }
}

#endif //KLK_DVBMESSAGES_H
