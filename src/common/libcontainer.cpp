/**
   @file libcontainer.cpp
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/10/22 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dlfcn.h>
#include <string.h>
#include <errno.h>

#include "libcontainer.h"
#include "log.h"
#include "utils.h"
#include "exception.h"

using namespace klk;

//
// LibContainer class
//

// static variable initialization
Mutex LibContainer::m_lock;

// Constructor
// @param[in] path - the path to the lib
LibContainer::LibContainer(const std::string& path) :
    m_path(path), m_handle(NULL)
{
}

// Destructor
LibContainer::~LibContainer()
{
    if (m_handle)
    {
        if (dlclose(m_handle) != 0)
        {
            klk_log(KLKLOG_ERROR, "Error %d in dlclose(): %s. Lib path: %s",
                    errno, strerror(errno), m_path.c_str());
        }
    }
    m_handle = NULL;
}

// Opens the lib
void LibContainer::open()
{
    if (base::Utils::fileExist(m_path.c_str()) == false)
    {
        throw Exception(__FILE__, __LINE__,
                             "Lib path is invalid: " + m_path);
    }
    // dlerror() is not thread safe. The call should be locked
    // http://www.opengroup.org/onlinepubs/009695399/functions/dlerror.html
    Locker lock(&m_lock);
    m_handle = dlopen(m_path.c_str(), RTLD_NOW);
    if (!m_handle)
    {
        char* descr = dlerror();
        BOOST_ASSERT(descr);
        throw Exception(__FILE__, __LINE__, descr);
    }
}

// Do dlsym call
// @param[in] fname - the function name
// @return the pointer to a function or NULL if tehre was an error
void* LibContainer::sym(const std::string& fname)
{
    if (m_handle == NULL)
    {
        KLKASSERT(m_handle);
        return NULL;
    }

    // dlerror() is not thread safe. The call should be locked
    // http://www.opengroup.org/onlinepubs/009695399/functions/dlerror.html
    Locker lock(&m_lock);
    void* res = dlsym(m_handle, fname.c_str());
    if (res == NULL)
    {
        klk_log(KLKLOG_ERROR, "Result in dlsym(): %s", dlerror());
    }

    return res;
}
