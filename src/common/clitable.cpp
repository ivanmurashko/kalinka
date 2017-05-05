/**
   @file clitable.cpp
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
   - 2009/11/22 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <numeric>

#include <boost/bind.hpp>
#include <boost/assert.hpp>

#include "clitable.h"
#include "utils.h"

using namespace klk;
using namespace klk::cli;

//
// Table class
//

// Constructor
Table::Table() : m_storage(), m_alignment()
{
}

// Destructor
Table::~Table()
{
}

// Adds a row to the table
void Table::addRow(const StringList& row)
{
    if (m_storage.empty())
    {
        // create alignment info
        m_alignment.resize(row.size());
        std::fill(m_alignment.begin(), m_alignment.end(), 0);
    }
    else
    {
        BOOST_ASSERT(m_storage.begin()->size() == row.size());
    }

    m_storage.push_back(row);
    // update alignment info
    AlignmentInfo::iterator align = m_alignment.begin();
    for (StringList::const_iterator item = row.begin();
         item != row.end() && align != m_alignment.end();
         item++, align++)
    {
        *align = std::max(item->size(), *align);
    }
}

// Retrives the formated represination of the table for displaying
const std::string Table::formatOutput() const
{
    return std::accumulate(
        m_storage.begin(),
        m_storage.end(), std::string(),
        boost::bind<const std::string>(&Table::formatRow, this, _1, _2));
}

// Retrives a formated info from a row
const std::string Table::formatRow(const std::string& initial,
                                   const StringList& row) const
{
    BOOST_ASSERT(row.size() == m_alignment.size());

    // we do alignment for all item except last
    std::string result = *(row.rbegin()) + "\n";
    AlignmentInfo::const_reverse_iterator align = m_alignment.rbegin();
    for (StringList::const_reverse_iterator item = row.rbegin();
         item != row.rend() && align != m_alignment.rend();
         item++, align++)
    {
        // ignore first (has been already included)
        if (item == row.rbegin())
            continue;
        result = base::Utils::align(*item, *align) + " " + result;
    }

    return initial + result;
}
