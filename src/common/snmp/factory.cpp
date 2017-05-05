/**
   @file factory.cpp
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


#include "factory.h"
#include "protocol.h"
#include "table.h"
#include "scalar.h"

#include "exception.h"

using namespace klk;
using namespace klk::snmp;

//
// Factory class
//

// Constructor
Factory::Factory(const std::string& modid) :
    m_modid(modid), m_table(), m_counter()
{
    BOOST_ASSERT(m_modid.empty() == false);
}

// Destructor
Factory::~Factory()
{
}

// Replaces the destructor
void Factory::destroy()
{
    clearData();
}

// Retrives a new table data
void Factory::retriveData()
{
    Protocol proto(m_modid);
    ScalarPtr req(new Scalar());
    req->setValue(klk::StringWrapper(GETSTATUSTABLE));
    proto.sendData(req);
    IDataPtr res = proto.recvData();
    TablePtr table =
        boost::dynamic_pointer_cast<Table, IData>(res);
    BOOST_ASSERT(table);
    m_table = table->getData();
    m_counter = m_table.begin();
}

// Clears
void Factory::clearData()
{
    m_table.clear();
}

// Retrives next data row
TableRow* Factory::getNext()
{
    TableRow* row = NULL;
    if (!m_table.empty())
    {
        if (m_counter != m_table.end())
        {
            row = &(*m_counter);
            m_counter++;
        }
    }

    return row;
}
