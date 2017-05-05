/**
   @file garbage.cpp
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
   - 2009/05/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/assert.hpp>

#include "garbage.h"
#include "clidriver.h"
#include "exception.h"

using namespace klk;
using namespace klk::cli;

//
// Garbage class
//

// Constructor
Garbage::Garbage() : m_garbage()
{
}

// Destructor
Garbage::~Garbage()
{
    std::for_each(m_garbage.begin(), m_garbage.end(),
                  boost::bind(&free, _1));
    m_garbage.clear();
}

// Allocs data for usage
char* Garbage::allocData(const std::string& data)
{
    char* result = strdup(data.c_str());
    BOOST_ASSERT(result);
    m_garbage.push_back(static_cast<void*>(result));
    return result;
}

// Unregister an entry
void Garbage::freeEntry(struct ast_cli_entry* entry)
{
    // find it in the garbage
    GarbageCollectorList::iterator i = std::find(m_garbage.begin(),
                                                 m_garbage.end(),
                                                 static_cast<void*>(entry));
    if (i != m_garbage.end())
    {
        BOOST_ASSERT(entry);
        if (ast_cli_unregister(entry) != 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "ast_cli_unregister() failed");
        }
        KLKFREE(*i);
        m_garbage.erase(i);
    }
}

// Allocates an entry and fills it with zeros
struct ast_cli_entry* Garbage::allocEntry()
{
    void* entry_pure = calloc(sizeof(struct ast_cli_entry), 1);
    BOOST_ASSERT(entry_pure);
    m_garbage.push_back(entry_pure);
    struct ast_cli_entry* entry =
        static_cast<struct ast_cli_entry*>(entry_pure);

    return entry;
}

