/**
   @file file.cpp
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
   - 2009/03/23 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "file.h"
#include "exception.h"
#include "defines.h"
#include "messages.h"
#include "db.h"
#include "cmd.h"

using namespace klk;
using namespace klk::file;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new File(factory));
}

// Constructor
File::File(IFactory *factory) :
    ModuleWithInfo<FileInfo>(factory, MODID,
                             msg::id::FILESTART,
                             msg::id::FILESTOP)
{
}

// Destructor
File::~File()
{
}

// Gets a human readable module name
const std::string File::getName() const throw()
{
    return MODNAME;
}

// Retrives an set with data from @ref grDB "database"
const File::InfoSet File::getInfoFromDB()
{
    db::DB db(getFactory());
    db.connect();
    // CREATE PROCEDURE `klk_file_list` (
    // IN host VARCHAR(40)
    db::Parameters params;
    params.add("@host", db.getHostUUID());
    db::ResultVector rv = db.callSelect("klk_file_list", params, NULL);
    InfoSet set;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT file, name, file_path, file_type  FROM klk_file
        const std::string uuid = (*i)["file"].toString();
        const std::string name = (*i)["name"].toString();
        const std::string path = (*i)["file_path"].toString();
        const std::string type = (*i)["type_uuid"].toString();

        // FIXME!!! bad code here
        if (type == type::REGULAR)
        {
            const FileInfoPtr info(new FileInfo(uuid, name,
                                                path, getFactory()));
            set.insert(info);
        }
        else if (type == type::FOLDER)
        {
            const FileInfoPtr info(new FolderInfo(uuid, name, path, getFactory()));
            set.insert(info);
        }
        else
        {
            throw Exception(__FILE__, __LINE__, "Unsopported file type: " + type);
        }
    }

    return set;
}

// Starts a file usage
void File::doStart(const IMessagePtr& in,
                   const IMessagePtr& out)
{
    BOOST_ASSERT(out);
    const mod::InfoPtr info = getModuleInfo(in);
    if (info->isInUse())
    {
        // already tuned
        throw Exception(__FILE__, __LINE__,
                        "Failed to start usage the module info with "
                        "name '%s'", info->getName().c_str());
    }
    info->check(in);
    info->setInUse(true);
    info->fillOutMessage(out);
}

// Retrives an info about a file
void File::doInfo(const IMessagePtr& in, const IMessagePtr& out)
{
    BOOST_ASSERT(out);
    const mod::InfoPtr info = getModuleInfo(in);
    info->check(in);
    info->fillOutMessage(out);
}

// Register all processors
void File::registerProcessors()
{
    ModuleWithInfo<FileInfo>::registerProcessors();

    registerSync(
        msg::id::FILEINFO,
        boost::bind(&File::doInfo, this, _1, _2));

    registerCLI(cli::ICommandPtr(new AddCommand()));
    registerCLI(cli::ICommandPtr(new DelCommand()));
    registerCLI(cli::ICommandPtr(new ShowCommand()));

    registerDBUpdateMessage(UPDATEDB_MESSAGE);
}
