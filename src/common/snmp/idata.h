/**
   @file idata.h
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
   - 2009/05/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_IDATA_H
#define KLK_IDATA_H

#include <boost/shared_ptr.hpp>

#include "binarydata.h"

namespace klk
{
    namespace snmp
    {
        /**
           SNMP value type
        */
        typedef enum
        {
            UNDEFINED = 0,
            SCALAR = 1,
            TABLE = 2
        } ValueType;

        /**
           @brief SNMP data interface

           SNMP data interface
        */
        class IData
        {
        public:
            /**
               Destructor
            */
            virtual ~IData(){}

            /**
               Does data serialization

               @return the serialized data
            */
            virtual const klk::BinaryData serialize() const = 0;

            /**
               Retrives the type

               @return the data type
            */
            virtual const ValueType getType() const throw() = 0;
        };

        /**
           IData smart pointer
        */
        typedef boost::shared_ptr<IData> IDataPtr;
    }
}

#endif //KLK_IDATA_H
