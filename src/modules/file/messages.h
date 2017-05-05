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

#ifndef KLK_FILEMESSAGES_H
#define KLK_FILEMESSAGES_H

#include <string>
/** @defgroup grFileMessageID File module messages IDs
    @ingroup grMessageID

    Messages IDs
*/


namespace klk
{
    namespace msg
    {
        namespace id
        {
            /** @addtogroup grFileMessageID
                @{
            */

            /**
               Starts a file usage
            */
            const std::string FILESTART("de11a09f-621c-4375-b422-1fbf837bcfc1");

            /**
               Stops a file usage
            */
            const std::string FILESTOP("e459510e-f561-4a07-8147-1c1d2ca8c418");

            /**
               Message ID for file info

               The message just retrives info without any locking
               and as result does not require stop
            */
            const std::string FILEINFO = "54d7768b-b3f7-436a-b475-60ca848fb359";
            /** @} */
        }

        namespace key
        {
            /** @addtogroup grFileMessageID
                @{
            */

            /**
               File path
            */
            const std::string FILEPATH("file_path");

            /**
               File opening mode
            */
            const std::string FILEMODE("file_mode");

            /**
               File opening mode read
            */
            const std::string FILEREAD("file_mode_read");

            /**
               File opening mode read
            */
            const std::string FILEWRITE("file_mode_write");

            /** @} */
        }
    }
}

#endif //KLK_FILEMESSAGES_H
