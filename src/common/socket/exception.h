/**
   @file exception.h
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
   - 2009/05/15 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SOCKETEXCEPTION_H
#define KLK_SOCKETEXCEPTION_H

#include "../exception.h"

namespace klk
{
    /**
       @brief Special exception for closed connection event

       Special exception for closed connection event
    */
    class ClosedConnection : public Exception
    {
    public:
        /**
           Constructor

           @param[in] file - the source file
           @param[in] line - the source file line
           @param[in] peername - host that closes the connection
        */
        ClosedConnection(const std::string& file, int line,
                         const std::string& peername) throw();

        /**
           Copy constructor
           @param[in] value - the copy param
        */
        ClosedConnection(const ClosedConnection& value);

        /**
           Destructor
        */
        virtual ~ClosedConnection() throw(){}
    private:
        /**
           Assigment operator
           @param[in] value - the copy param
        */
        ClosedConnection& operator=(const ClosedConnection& value);
    };
}

#endif //KLK_SOCKETEXCEPTION_H
