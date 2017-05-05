/**
   @file exception.cpp
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
   - 2009/02/12 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdarg.h>
#include <execinfo.h>
#include <stdlib.h>

#include <boost/assert.hpp>

#include "exception.h"
#include "log.h"

using namespace klk;

//
// Exception class
//

// Constructor
Exception::Exception(const std::string& file,
                     int line, const std::string& description) throw() :
    m_description()
{
    char buff[1024];
    snprintf(buff, sizeof(buff), "Exception at %s:%d: %s",
             file.c_str(), line, description.c_str());
    m_description = buff;

    klk_log(KLKLOG_ERROR, "Exception throwed at %s:%d: %s",
            file.c_str(), line, description.c_str());
}

// Constructor
Exception::Exception(const std::string& file, int line,
                     const char* format, ...) throw() :
    m_description()
{
    va_list args;

    char description[1024];
    va_start(args, format);
    vsnprintf(description, sizeof(description) - 1, format, args);
    va_end(args);

    char buff[1024];
    snprintf(buff, sizeof(buff), "Exception at %s:%d: %s",
             file.c_str(), line, description);
    m_description = buff;

    klk_log(KLKLOG_DEBUG, "Exception throwed at %s:%d: %s",
            file.c_str(), line, description);
}

// Copy constructor
Exception::Exception(const Exception& value) :
    m_description(value.m_description)
{
}

// Retrives the error description
const char* Exception::what() const throw()
{
    return m_description.c_str();
}

//
// BOOST_ASSERT should throw klk::Exception see ticket #212
//
namespace boost
{
    void assertion_failed(char const * expr, char const * function,
                          char const * file, long line)
    {
        klk_log( KLKLOG_ERROR,
                 "BOOST_ASSERT at klkd application detected" );

        void *trace[16];
        char **messages = static_cast< char **> ( NULL );
        int trace_size = 0;

        trace_size = backtrace(trace, 16);
        messages = backtrace_symbols(trace, trace_size);

        for ( int i = 0; i < trace_size; ++i )
        {
            klk_log( KLKLOG_ERROR, "%d: %s", i, messages[i] );
        }

        free( messages );

        throw Exception(file, line, "Failed assertion '%s' at %s",
                        expr, function);
    }

    void assertion_failed_msg(char const * expr, char const * msg, 
                              char const * function,
                              char const * file, long line)
    {
        klk_log( KLKLOG_ERROR,
                 "BOOST_ASSERT at klkd application detected" );

        void *trace[16];
        char **messages = static_cast< char **> ( NULL );
        int trace_size = 0;

        trace_size = backtrace(trace, 16);
        messages = backtrace_symbols(trace, trace_size);

        for ( int i = 0; i < trace_size; ++i )
        {
            klk_log( KLKLOG_ERROR, "%d: %s: %s", i, messages[i], msg );
        }

        free( messages );

        throw Exception(file, line, "Failed assertion '%s' at %s",
                        expr, function);
    }

}
