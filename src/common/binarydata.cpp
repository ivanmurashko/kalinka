/**
   @file binarydata.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "binarydata.h"
#include "exception.h"

using namespace klk;

//
// BinaryData class
//

// Default constructor
BinaryData::BinaryData() : m_data()
{
}

// Constructor from string data
BinaryData::BinaryData(const std::string& data) :
    m_data(data.begin(), data.end())
{
}

// Constructor from klk::BinaryDataContainer data
BinaryData::BinaryData(const BinaryDataContainer& data) :
    m_data(data)
{
}


// Default constructor
BinaryData::BinaryData(size_t size) :
    m_data(size)
{
}

// Destructor
BinaryData::~BinaryData()
{
}

// Converts data to string
const std::string BinaryData::toString() const
{
    BinaryDataContainer tmp = m_data;
    tmp.resize(tmp.size() + 1);
    tmp[tmp.size() - 1] = '\x0';
    return &tmp[0];
}

// Compare operator
bool BinaryData::operator!=(const BinaryData& data) const
{
    return m_data != data.m_data;
}

// Compare operator
bool BinaryData::operator==(const BinaryData& data) const
{
    return m_data == data.m_data;
}


// Sets buffer size
void BinaryData::resize(size_t size)
{
    m_data.resize(size);
}

// Gets buffer
void* BinaryData::toVoid()
{
    if (m_data.empty())
    {
        throw Exception(__FILE__, __LINE__,  "No data");
    }
    return &m_data[0];
}

// Gets data buffer
const void* BinaryData::toVoid() const
{
    if (m_data.empty())
    {
        throw Exception(__FILE__, __LINE__,  "No data");
    }
    return &m_data[0];
}

// Adds data
void BinaryData::add(const BinaryData& data)
{
    add(data.m_data);
}

// Adds data
void BinaryData::add(const BinaryDataContainer& data)
{
    // nothing to do if there is no data
    if (!data.empty())
    {
        m_data.insert(m_data.end(), data.begin(), data.end());
    }
}

// Copy constructor
BinaryData::BinaryData(const BinaryData& value) :
    m_data(value.m_data)
{
}

// Assigment operator
BinaryData& BinaryData::operator=(const BinaryData& value)
{
    m_data = value.m_data;
    return *this;
}

// Converts data to klk::BinaryDataContainer
const BinaryDataContainer BinaryData::toData() const
{
    return m_data;
}

