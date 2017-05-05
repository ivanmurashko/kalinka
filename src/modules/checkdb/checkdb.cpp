/**
   @file checkdb.cpp
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
   - 2009/03/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "checkdb.h"
#include "exception.h"
#include "defines.h"
#include "db.h"

using namespace klk;
using namespace klk::db;


/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new CheckDB(factory));
}

//
// CheckDB
//

// Constructor
// @param[in] factory the module factory
CheckDB::CheckDB(IFactory *factory) :
    Module(factory, MODID), m_last(-1)
{
}

// Destructor
CheckDB::~CheckDB()
{
}

// Register all processors
void CheckDB::registerProcessors()
{
    registerTimer(boost::bind(&CheckDB::checkDB, this), CHECKDBINTERVAL);
}

// Gets a human readable module name
const std::string CheckDB::getName() const throw()
{
    return db::MODNAME;
}

// Checks DB
void CheckDB::checkDB()
{
    db::DB db(getFactory());
    db.connect();

    // retrive the current last
    //klk_checkdb_getlastid` (
    // OUT return_id INT
    db::Parameters params;
    params.add("@id");
    db::Result res = db.callSimple("klk_checkdb_get_lastid", params);
    int new_last = res["@id"].toInt();
    if (new_last > m_last)
    {
        //CREATE PROCEDURE `klk_checkdb_get_changed` (
        // IN last_id INT

        params.clear();
        params.add("@id", m_last);
        db::ResultVector rv = db.callSelect("klk_checkdb_get_changed",
                                               params, NULL);
        if (rv.empty() == false)
        {
            IMessagePtr msg =
            getFactory()->getMessageFactory()->getMessage(msg::id::CHANGEDB);
            msg->clearReceiverList();
            //	SELECT DISTINCT module_uuid FROM klk_log WHERE id > last_id;
            for (db::ResultVector::iterator i = rv.begin();
                 i != rv.end(); i++)
            {
                msg->addReceiver((*i)["module_uuid"].toString());
            }
            getFactory()->getModuleFactory()->sendMessage(msg);
        }
    }
    m_last = new_last;
}
