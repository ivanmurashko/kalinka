/**
   @file resources.cpp
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
   - 2009/03/27 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <libraw1394/raw1394.h>

#include "ifactory.h"
#include "ieee1394resources.h"
#include "ieee1394busdev.h"
#include "exception.h"
#include "defines.h"
#include "utils.h"

using namespace klk;
using namespace klk::fw;

/**
   Resources factory function
*/
IResourcesPtr klk_resources_get(IFactory *factory)
{
    return IResourcesPtr(new Resources(factory));
}

//
// Resources class
//

// Constructor
Resources::Resources(IFactory* factory) :
    dev::BaseResources(factory, dev::IEEE1394)
{
}

Resources::~Resources()
{
}

// Inits IEEE1394 resources
void Resources::initDevs()
{
    klk_log(KLKLOG_DEBUG, "IEEE1394 resource initialization");
    if (base::Utils::fileExist("/dev/raw1394"))
    {
        checkDevs();
    }
    else
    {
        klk_log(KLKLOG_INFO,
                "The mediaserver does not have any IEEE1394 devices");
    }
}

// Inits bus devs
void Resources::checkDevs()
{
    raw1394handle_t handle;

    if (!(handle = raw1394_new_handle()))
    {
        throw Exception(__FILE__, __LINE__,
                             "raw1394 - couldn't get handle. "
                             "The module is not loaded");
    }

    try
    {
        IDevPtr dev(new dev::IEEE1394Bus(m_factory));
        addDev(dev);
    }
    catch(...)
    {
        raw1394_destroy_handle(handle);
        throw;
    }

    raw1394_destroy_handle(handle);
}
