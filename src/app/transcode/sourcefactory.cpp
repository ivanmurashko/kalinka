/**
   @file sourcefactory.cpp
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
   - 2009/09/05 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "sourcefactory.h"
#include "exception.h"
#include "fileinfo.h"
#include "netinfo.h"
#include "httpinfo.h"
#include "fwinfo.h"
#include "defines.h"
#include "mod/infocontainer.h"

using namespace klk;
using namespace klk::trans;

//
// SourceFactory class
//

// Constructor
SourceFactory::SourceFactory() : m_lock(), m_container()
{
}

// Destructor
SourceFactory::~SourceFactory()
{
}

// Gets the source by its data
// The method looks at the already created sources firs
// if nothing found it will create a new object
const SourceInfoPtr SourceFactory::getSource(const std::string& uuid,
                                             const std::string& name,
                                             const std::string& media_type,
                                             const std::string& type)

{
    SourceInfoPtr res;

    // look for the element
    // may be it's already exist
    {
        Locker lock(&m_lock);
        SourceInfoContainer::iterator find =
            std::find_if(m_container.begin(), m_container.end(),
                         boost::bind(mod::FindInfoByUUID<SourceInfo>(),
                                     _1, uuid));
        if (find != m_container.end())
            return *find; // found it
    }

    // FIXME!!! bad code
    if (type == type::FILE)
    {
        res = SourceInfoPtr(new FileInfo(uuid, name, media_type));
    }
    else if (type == type::NET)
    {
        res = SourceInfoPtr(new NetInfo(uuid, name, media_type));
    }
    else if (type == type::HTTPSRC)
    {
        res = SourceInfoPtr(new HTTPSrcInfo(uuid, name, media_type));
    }
    else if (type == type::FW)
    {
        res = SourceInfoPtr(new FWSrcInfo(uuid, name, media_type));
    }
    else
    {
        throw Exception(__FILE__, __LINE__, "Unsupported type: " + type);
    }
    return res;
}

