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
   - 2009/02/12 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_EXCEPTION_H
#define KLK_EXCEPTION_H

#include <string>

#include "errors.h"
#include "log.h"

namespace klk
{
    namespace err
    {
        /**
           Out of range error description

           @ingroup grResults
        */
        const std::string OUTRANGE = "Out of range detected";

        /**
           Out of range error description

           @ingroup grResults
        */
        const std::string UNSPECIFIED = "Unspecified error";

        /**
           Bad lexical cast exception was got

           @ingroup grResults
        */
        const std::string BADLEXCAST = "boost::bad_lexical_cast was got";

        /**
           Bad memory allocation exception was got

           @ingroup grResults
        */
        const std::string MEMORYALLOC = "std::bad_alloc was got";
    }

    /**
       @brief Base exception class

       Base exception

       @ingroup grResults
    */
    class Exception : public std::exception
    {
    public:
        /**
           Constructor

           @param[in] file - the source file
           @param[in] line - the source file line
           @param[in] description - the error description
        */
        Exception(const std::string& file, int line,
                  const std::string& description) throw();

        /**
           Constructor

           @param[in] file - the source file
           @param[in] line - the source file line
           @param[in] format - the format string
        */
        Exception(const std::string& file, int line,
                  const char* format, ...) throw();

        /**
           Copy constructor
           @param[in] value - the copy param
        */
        Exception(const Exception& value);

        /**
           Destructor
        */
        virtual ~Exception() throw() {}

        /**
           Retrives the error description
        */
        virtual const char* what() const throw();
    private:
        std::string m_description; ///< description
    private:
        /**
           Assigment operator
           @param[in] value - the copy param
        */
        Exception& operator=(const Exception& value);
    };
}

/**
   @def KLKTHROW
   @brief Throws a default exception

   Throws a default exception
*/
#define KLKTHROW(x)                                     \
    {throw klk::Exception(__FILE__, __LINE__, #x);}

/**
   @def NOTIMPLEMENTED
   @brief Throws not implemented exception

   Throws not implemented exception
*/
#define NOTIMPLEMENTED                                                  \
    {throw klk::Exception(__FILE__, __LINE__, "Not implemented");}

#endif //KLK_EXCEPTION_H
