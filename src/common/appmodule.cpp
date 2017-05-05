/**
   @file appmodule.cpp
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
   - 2009/04/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "appmodule.h"
#include "exception.h"
#include "cliapp.h"
#include "db.h"

using namespace klk::app;

//
// Module class
//

// Constructor
Module::Module(klk::IFactory* factory,
               const std::string& modid,
               const std::string& setmsgid,
               const std::string& showmsgid) :
    klk::ModuleWithDB(factory, modid), m_appuuid_mutex(), m_appuuid(),
    m_setmsgid(setmsgid),
    m_showmsgid(showmsgid)
{
    BOOST_ASSERT(m_setmsgid.empty() == false);
    BOOST_ASSERT(m_showmsgid.empty() == false);
    BOOST_ASSERT(m_setmsgid != m_showmsgid);
}

// Retrives application uuid
const std::string Module::getAppUUID()
{
    using namespace klk;
    Locker lock(&m_appuuid_mutex);
    if (m_appuuid.empty())
    {
        // retrive application id
        // `klk_application_uuid_get` (
        // IN module VARCHAR(40),
        // IN host VARCHAR(40),
        // OUT application VARCHAR(40)
        db::DB db(getFactory());
        db.connect();

        db::Parameters params;
        params.add("@module", getID());
        params.add("@host", db.getHostUUID());
        params.add("@application");

        db::Result res = db.callSimple("klk_application_uuid_get", params);
        if (res["@application"].isNull())
        {
            throw Exception(__FILE__, __LINE__,
                            "DB error while retriving application uuid");
        }

        m_appuuid = res["@application"].toString();
    }
    return m_appuuid;
}

// Register all processors
void Module::registerProcessors()
{
    using namespace klk;
    ModuleWithDB::registerProcessors();

    registerCLI(cli::ICommandPtr(new cli::AutostartSet(m_setmsgid)));
    registerCLI(cli::ICommandPtr(new cli::AutostartShow(m_showmsgid)));
}
