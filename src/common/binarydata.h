/**
   @file binarydata.h
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
   - 2009/03/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_BINARYDATA_H
#define KLK_BINARYDATA_H

#include <string>
#include <vector>

#include "binarydatacontainer.h"

namespace klk
{
    /**
       @brief Binary data

       The binary data container

       @ingroup grCommon
    */
    class BinaryData
    {
    public:
        /**
           Default constructor
        */
        BinaryData();

        /**
           Default constructor

           @param[in] size - the buffer size
        */
        explicit BinaryData(size_t size);

        /**
           Constructor from string data

           @param[in] data - the data
        */
        explicit BinaryData(const std::string& data);

        /**
           Constructor from klk::BinaryDataContainer data

           @param[in] data - the data
        */
        explicit BinaryData(const BinaryDataContainer& data);

        /**
           Copy constructor
           @param[in] value - the copy param
        */
        BinaryData(const BinaryData& value);

        /**
           Destructor
        */
        ~BinaryData();

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        BinaryData& operator=(const BinaryData& value);

        /**
           Converts data to string

           @exception klk::Exception

           @return data in the string form
        */
        const std::string toString() const;

        /**
           Converts data to klk::BinaryDataContainer

           @exception klk::Exception

           @return data in the string form
        */
        const BinaryDataContainer toData() const;

        /**
           Compare operator

           @param[in] data

           @return
           - true
           - false

           @note used in utests
        */
        bool operator!=(const BinaryData& data) const;

        /**
           Compare operator

           @param[in] data

           @return
           - true
           - false

           @note used in utests
        */
        bool operator==(const BinaryData& data) const;

        /**
           Sets buffer size

           @param[in] size - the size to be set
        */
        void resize(size_t size);

        /**
           Gets data buffer

           @return the buffer
        */
        void* toVoid();

        /**
           Gets data buffer

           const method

           @return the buffer
        */
        const void* toVoid() const;

        /**
           Retrives size of the binary data container

           @return the size
        */
        const size_t size() const throw(){return m_data.size();}

        /**
           Adds data

           @param[in] data - the data to be added
        */
        void add(const BinaryData& data);

        /**
           Adds data

           @param[in] data - the data to be added
        */
        void add(const BinaryDataContainer& data);

        /**
           Checks is there any data or not

           @return
           - true - there is not data
           - false the container is not empty
        */
        const bool empty() const throw(){return m_data.empty();}
    private:
        BinaryDataContainer m_data;
    private:
        /**
           Don't allow to allocate the objects in heap
         */
        void *operator new(size_t size);
    };
};

#endif //KLK_BINARYDATA_H
