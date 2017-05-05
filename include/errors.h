/**
   @file errors.h
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
   - 2008/10/04 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ERRORS_H
#define KLK_ERRORS_H

#include <assert.h>

/** @defgroup grResults Results defenitions
    @{

    Results defenitions

    @ingroup grCommon
*/

namespace klk
{
    /**
       Result type

       @ingroup grResults
    */
    typedef enum
    {
        OK = 0,
        ERROR = 1
    } Result;
};

/**
   @def KLKASSERT
   @brief KLK replacement for assert

   The replacement for assert
*/
#define KLKASSERT(x)                                            \
    {if (!(x)) {klk_log(KLKLOG_ERROR,                           \
                        "%s:%d: %s: Assertion '%s' failed",     \
                        __FILE__, __LINE__, __FUNCTION__, #x);  \
        }assert(x);}

/** @def CHECKCODE
    @brief Checks the result code

    Checks the result code
*/
#define CHECKCODE(x) {if(x != klk::OK){KLKASSERT(x == klk::OK);return x;}}

/** @def CHECKNOTNULL
    @brief Checks that the value is not NULL

    Checks that the value is not NULL
*/
#define CHECKNOTNULL(x) {if(!(x)){KLKASSERT(x); \
            return klk::ERROR;}}

/** @def CHECKNOTVALUE
    @brief Checks the argument

    If the first argument is equal to the second one
    the @ref klk::ERROR will be returned

    @ingroup grResults
*/
#define CHECKNOTVALUE(x, y) {if(x == y){KLKASSERT(x != y);      \
            return klk::ERROR;}}

/** \def CHECKVALUE
    @brief Checks the argument

    If the first argument is not equal to the second one
    the @ref klk::ERROR will be returned
*/
#define CHECKVALUE(x, y) {if(x != y){KLKASSERT(x == y); \
            return klk::ERROR;}}

/** @} */

#endif //KLK_ERRORS_H
