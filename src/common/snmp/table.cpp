/**
   @file table.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "table.h"
#include "exception.h"

using namespace klk;
using namespace klk::snmp;

//
// Table class
//

// Constructor
Table::Table() : m_lock(), m_data()
{
}

// Constructor that does deserialization
Table::Table(const BinaryData& data) :
    m_lock(), m_data()
{
    BOOST_ASSERT(data.size() >= sizeof(size_t));
    const void* raw_data = data.toVoid();
    size_t row_size = *(static_cast<const size_t*>(raw_data));

    if (row_size != 0)
    {
        const char* p = static_cast<const char*>(raw_data) + sizeof(size_t);
        const char* end = static_cast<const char*>(raw_data) + data.size();

        while (p < end)
        {
            TableRow row;
            for (size_t count = 0; count < row_size; count++)
            {
                if (p >= end)
                {
                    throw Exception(__FILE__, __LINE__,
                                         "Broken SNMP table data");
                }

                const char* value = p;
                row.push_back(StringWrapper(value));
                p += strlen(value) + 1;
            }

            addRow(row);
        }

        // check validity
        BOOST_ASSERT(p == end);
    }
}


// Destructor
Table::~Table()
{
}

// Does the table serialization
const BinaryData Table::serialize() const
{
    BinaryData data(sizeof(size_t));
    size_t* row_size = static_cast<size_t*>(data.toVoid());
    *row_size = 0;
    for (TableRowContainer::const_iterator i = m_data.begin();
         i != m_data.end(); i++)
    {
        TableRow row = *i;
        // FIXME!! bad code - the container itself can be changed
        row_size = static_cast<size_t*>(data.toVoid());
        if (*row_size == 0)
        {
            *row_size = row.size();
        }
        else if (*row_size != row.size())
        {
            throw Exception(__FILE__, __LINE__,
                                 "Broken SNMP table data");
        }

        for (TableRow::iterator j = row.begin(); j != row.end(); j++)
        {
            BinaryData val(j->toString());
            val.resize(val.size() + 1);
            char* p = static_cast<char*>(val.toVoid());
            p[val.size() - 1] = '\x0'; // final 0
            data.add(val);
        }
    }

    return data;
}

// Adds a row
void Table::addRow(const TableRow& row)
{
    Locker lock(&m_lock);
    // check size
    if (!m_data.empty())
    {
        // all row should have same size
        BOOST_ASSERT(m_data.begin()->size() == row.size());
    }

    m_data.push_back(row);
}

// Retrives data
const TableRowContainer Table::getData() const
{
    Locker lock(&m_lock);
    return m_data;
}
