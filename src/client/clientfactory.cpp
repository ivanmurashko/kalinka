/**
   @file clientfactory.cpp
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
   - 2007/06/30 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clientfactory.h"
#include "client.h"
#include "log.h"
#include "exception.h"

using namespace klk;
using namespace klk::cli;

//
// CFactory class
//

Factory* Factory::m_instance = 0;

// Constructor
Factory::Factory() :
    base::Factory("KLK-CLIENT"),
    m_client(NULL), m_modprocessor(NULL), m_garbage(NULL)
{
}

// Destructor
Factory::~Factory()
{
    KLKDELETE(m_client);
    KLKDELETE(m_modprocessor);
    KLKDELETE(m_garbage);
}

// gets unique instance of the factory
// Pattern Singleton
// Replaces constructor
Factory* Factory::instance()
{
    if (m_instance == NULL)
    {
	m_instance = new Factory();
    }
    return m_instance;
}

// Frees resources
// Replace destructor
void Factory::destroy()
{
    KLKDELETE(m_instance);
}

// get an instance of the daemon client
Client* Factory::getClient()
{
    try
    {
        if (m_client == NULL)
        {
            m_client = new Client(this);
        }
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
    BOOST_ASSERT(m_client);

    return m_client;
}


// Retrives module processor
ModuleProcessor* Factory::getModuleProcessor()
{
    try
    {
        if (m_modprocessor == NULL)
        {
            m_modprocessor = new ModuleProcessor(this);
        }
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
    BOOST_ASSERT(m_modprocessor);

    return m_modprocessor;
}

// Gets garbage collector
Garbage* Factory::getGarbage()
{
    try
    {
        if (m_garbage == NULL)
        {
            m_garbage = new Garbage();
        }
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
    BOOST_ASSERT(m_garbage);

    return m_garbage;
}
