/**
   @file scalar.h
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
   - 2009/05/27 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SCALAR_H
#define KLK_SCALAR_H

#include "idata.h"
#include "stringwrapper.h"
#include "thread.h"

namespace klk
{
    namespace snmp
    {
        /**
           @brief SNMP scalar object representation

           SNMP scalar object representation

           @ingroup grSNMP
        */
        class Scalar : public IData
        {
        public:
            /**
               Constructor
            */
            Scalar();

            /**
               Constructor that does deserialization

               @param[in] data - the serialized data
            */
            Scalar(const klk::BinaryData& data);

            /**
               Destructor
            */
            virtual ~Scalar();

            /**
               Retrives scalar value
            */
            const klk::StringWrapper getValue() const;

            /**
               Sets value

               @param[in] value - the value to be set
            */
            void setValue(const klk::StringWrapper& value);
        private:
            mutable klk::Mutex m_lock; ///< locker
            klk::StringWrapper m_value; ///< value

            /**
               Does the scalar serialization

               @return the serialized data
            */
            virtual const klk::BinaryData serialize() const;

            /**
               Retrives the type

               @return the data type
            */
            virtual const ValueType getType() const throw(){return SCALAR;}
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Scalar(const Scalar& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Scalar& operator=(const Scalar& value);
        };

        /**
           Scalar smart pointer
        */
        typedef boost::shared_ptr<Scalar> ScalarPtr;
    }
}

#endif //KLK_SCALAR_H
