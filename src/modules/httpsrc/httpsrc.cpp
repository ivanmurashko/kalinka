/**
   @file httpsrc.cpp
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
   - 2009/11/21 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "db.h"
#include "httpsrc.h"
#include "httpinfo.h"
#include "exception.h"
#include "defines.h"
#include "messages.h"
#include "cmd.h"

using namespace klk;
using namespace klk::httpsrc;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new HTTPSrc(factory));
}

// Constructor
HTTPSrc::HTTPSrc(IFactory *factory) :
    ModuleWithInfo<HTTPInfo>(factory, MODID,
                             msg::id::HTTPSRCSTART)
{
}

// Destructor
HTTPSrc::~HTTPSrc()
{
}

// Do the start info usage for a http source
void HTTPSrc::doStart(const IMessagePtr& in, const IMessagePtr& out)
{
    const mod::InfoPtr info = getModuleInfo(in);
    info->fillOutMessage(out);
}

// Gets a human readable module name
const std::string HTTPSrc::getName() const throw()
{
        return MODNAME;
}

// Retrives an set with data from @ref grDB "database"
const HTTPSrc::InfoSet HTTPSrc::getInfoFromDB()
{
    db::DB db(getFactory());
    db.connect();
    // CREATE PROCEDURE `klk_httpsrc_list` (
    db::Parameters params;
    db::ResultVector rv = db.callSelect("klk_httpsrc_list", params, NULL);
    InfoSet set;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT httpsrc AS uuid, title AS name, address AS addr,
        // login AS login, passwd AS password FROM klk_httpsrc;
        const std::string uuid = (*i)["uuid"].toString();
        const std::string name = (*i)["name"].toString();
        const std::string addr = (*i)["addr"].toString();
        const std::string login = (*i)["login"].toString();
        const std::string passwd = (*i)["password"].toString();

        const HTTPInfoPtr info(new HTTPInfo(uuid, name, addr, login, passwd));
        set.insert(info);
    }

    return set;
}

// Register all processors
void HTTPSrc::registerProcessors()
{
    ModuleWithInfo<HTTPInfo>::registerProcessors();
    registerCLI(cli::ICommandPtr(new AddCommand()));
    registerCLI(cli::ICommandPtr(new DelCommand()));
    registerCLI(cli::ICommandPtr(new ShowCommand()));

    registerDBUpdateMessage(UPDATEDB_MESSAGE);
}
