/**
   @file launchbase.cpp
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
   - 2010/10/01 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "launchbase.h"

#include "adapter/defines.h"
#include "msgcore/defines.h"
#include "checkdb/defines.h"

using namespace klk::launcher;

//
// Base class
//

// Constructor
Base::Base(IFactory* factory) :
    m_local(factory->getMainModuleId() == klk::MODULE_COMMON_ID ? true : false),
    m_main_module_id(factory->getMainModuleId()),
    m_local_load_list()
{
    BOOST_ASSERT(factory);
    // modules that should be loaded locally
    m_local_load_list.push_back(m_main_module_id);
    m_local_load_list.push_back(klk::adapter::MODID);
    m_local_load_list.push_back(klk::msgcore::MODID);
    m_local_load_list.push_back(klk::db::MODID);
}


// Check is the module process locally or remotelly
bool Base::isLocal(const std::string& id) const
{
    // FIXME!!! used mainly in utests
    if (m_local)
        return true;

    StringList::const_iterator find =
        std::find(m_local_load_list.begin(), m_local_load_list.end(), id);
    return (find != m_local_load_list.end());
}
