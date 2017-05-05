/**
   @file busdev.cpp
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

#include "ieee1394busdev.h"
#include "dev.h"
#include "db.h"
#include "log.h"
#include "exception.h"
#include "defines.h"

using namespace klk::dev;

//
// IEEE1394Bus class
//

// Constructor
// @param[in] factory - the factory
IEEE1394Bus::IEEE1394Bus(IFactory* factory) :
    klk::dev::Base(factory, IEEE1394BUS)
{
    setParam(NAME, IEEE1394BUSNAME);
}

// Updates the dev info
void IEEE1394Bus::update()
{
    // IN host_uuid VARCHAR(40),
    // IN id INT,
    // IN bus_type VARCHAR(40),
    // IN description VARCHAR(255),
    // OUT bus_uuid VARCHAR(40),
    // OUT return_value INTEGER
    db::Parameters params;
    db::DB db(getFactory());
    db.connect();
    params.add("@host_uuid", db.getHostUUID());
    params.add("@id", 0); // FIXME!!!
    params.add("@bus_type", getStringParam(klk::dev::TYPE));
    // FIXME!!!
    // IEEE1394 64 bit type id
    std::string description = "IEEE1394 bus for " +
        getFactory()->getConfig()->getHostName();
    params.add("@description", description);
    params.add("@bus_uuid", db::Parameters::null);
    params.add("@return_value", db::Parameters::null);
    db::Result result = db.callSimple("klk_bus_get_uuid",
                                            params);
    BOOST_ASSERT(result["@return_value"].toInt() == 0);
    setParam(klk::dev::UUID, result["@bus_uuid"].toString());
    setParam(klk::dev::BUSUUID, result["@bus_uuid"].toString());
}

// Updates DB (adds the dev if it's missing)
void IEEE1394Bus::updateDB()
{
    // FIXME!!! add the code
}

// test equalance of two devs
bool IEEE1394Bus::equal_to(const IDevPtr& dev) const
{
    if (boost::dynamic_pointer_cast<IEEE1394Bus, IDev>(dev) != NULL)
    {
        // FIXME!!! we have to have only one IEEE1394 dev by now
        return true;
    }
    return false;
}


