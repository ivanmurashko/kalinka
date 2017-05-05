/**
   @file stringwrapper.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stringwrapper.h"
#include "common.h"
#include "log.h"
#include "exception.h"

using namespace klk;

//
// StringWrapper class
//

// Constructor from const char*
// @param[in] value - the value to initialize the wrapper
StringWrapper::StringWrapper(const char* value) :
    m_value(""), m_null(false)
{
    if (value != NULL)
    {
        m_value = value;
    }
    else
    {
        m_null = true;
    }
}

// Constructor from std::string
// @param[in] value - the value to initialize the wrapper
StringWrapper::StringWrapper(const std::string& value) :
    m_value(value), m_null(false)
{
}


// Copy constructor
StringWrapper::StringWrapper(const StringWrapper& value) :
    m_value(value.m_value), m_null(value.m_null)
{
}

// Operator =
StringWrapper& StringWrapper::operator=(const StringWrapper& value)
{
    if (this == &value)
        return *this;
    m_value = value.m_value;
    m_null = value.m_null;
    return *this;
}

// Destructor
StringWrapper::~StringWrapper()
{
    // nothing to do
}

// Converts the storage to std::string
const std::string StringWrapper::toString() const
{
    if (m_null)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "The 'null' value at the string container");
    }

    return m_value;
}
