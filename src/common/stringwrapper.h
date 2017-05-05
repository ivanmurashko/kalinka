/**
   @file stringwrapper.h
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
   - 2008/10/04 created by ipp (Ivan Murashko)
   - 2008/01/04 some exception improvments were made by ipp
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 templates for differfetn conversions were added
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STRINGWRAPPER_H
#define KLK_STRINGWRAPPER_H

#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>

#include "exception.h"

namespace klk
{

    /**
       @brief The std::string wrapper class

       The class holds values with different types inside a std::string wrapper

       @ingroup grCommon
    */
    class StringWrapper
    {
    public:
        /**
           Constructor from const char*

           @param[in] value - the value to initialize the wrapper
        */
        StringWrapper(const char* value);

        /**
           Constructor from std::string

           @param[in] value - the value to initialize the wrapper
        */
        StringWrapper(const std::string& value);

        /**
           Constructor for a misc type

           @param[in] value - the value for initialization
         */
        template <class T> StringWrapper(T value) :
        m_value(), m_null(false)
        {
            try
            {
                m_value = boost::lexical_cast<std::string>(value);
            }
            catch (const boost::bad_lexical_cast& err)
            {
                throw Exception(__FILE__, __LINE__, "boost::bad_lexical_cast was got: %s",
                                err.what());
            }
        }

        /**
           Copy constructor
        */
        StringWrapper(const StringWrapper& value);

        /**
           Destructor
        */
        virtual ~StringWrapper();

        /**
           Operator =
        */
        StringWrapper& operator=(const StringWrapper& value);

        /**
           Converts the storage to std::string

           @return the string representation

           @exception @ref klk::Exception
        */
        const std::string toString() const;

        /**
           Converts the storage to int
        */
        int toInt() const{return convertTo<int>(0);}

        /**
           Converts the storage to u_int

           @return the converted value

           @exception - there was an error while convertion
        */
        u_int toUInt() const{return convertTo<u_int>(0U);}

        /**
           Converts the storage to u_int64

           @return the converted value

           @exception - there was an error while convertion
        */
        u_int64_t toUInt64() const{return convertTo<u_int64_t>(0ULL);}

        /**
           Check is there null or not

           @return
           - true - the storage is NULL
           - false - the storage is not NULL
        */
        bool isNull() const throw(){return m_null;}
    private:
        std::string m_value; ///< value to be stored at the wrapper
        bool m_null; ///< is there a null value

        /**
           Converter to different types

           @param[in] def_value - the default value

           @return the converted value

           @exception - there was an error while convertion
        */
        template <class T> const T convertTo(const T def_value) const
        {
            if (m_null)
            {
                throw Exception(__FILE__, __LINE__, "Trying to convert a NULL value");
            }

            T res = def_value;
            try
            {
                res = boost::lexical_cast<T>(m_value);
            }
            catch (const boost::bad_lexical_cast& err)
            {
                throw klk::Exception(__FILE__, __LINE__,  err::BADLEXCAST + ": " + err.what() +
                                     ". Value: " + m_value);
            }
            return res;
        }
    };

    /**
       Container for the StringWrapper
    */
    typedef std::vector<StringWrapper> StringWrapperVector;

};

#endif //KLK_STRINGWRAPPER_H
