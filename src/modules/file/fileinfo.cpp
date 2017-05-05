/**
   @file fileinfo.cpp
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
   - 2009/03/25 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fileinfo.h"
#include "exception.h"
#include "utils.h"
#include "messages.h"
#include "traps.h"
#include "db.h"

using namespace klk;
using namespace klk::file;

//
// FileInfo class
//

// Constructor
FileInfo::FileInfo(const std::string& uuid, const std::string& name,
                   const std::string& path,
                   IFactory* factory) :
    mod::Info(uuid, name), m_path(path), m_factory(factory)
{
    BOOST_ASSERT(m_path.empty() == false);
    BOOST_ASSERT(m_factory);
}

// Checks file existance
bool FileInfo::exist() const
{
    Locker lock(&m_lock);
    return base::Utils::fileExist(m_path);
}

// Updates the module info
void FileInfo::updateInfo(const mod::InfoPtr& value)
{
    const FileInfoPtr finfo = boost::dynamic_pointer_cast<FileInfo>(value);
    BOOST_ASSERT(finfo);
    Locker lock(&m_lock);
    mod::Info::updateInfo(value);
    m_path = finfo->m_path;
}

// Fills output message with values from the object
void FileInfo::fillOutMessage(const IMessagePtr& out) const
{
    Locker lock(&m_lock);
    mod::Info::fillOutMessage(out);
    out->setData(msg::key::FILEPATH, m_path);
}

// Checks that info corresponds data stored at the info message
void FileInfo::check(const IMessagePtr& in) const
{
    BOOST_ASSERT(in);
    const std::string mode = in->getValue(msg::key::FILEMODE);
    if (mode == msg::key::FILEREAD)
    {
        if (exist() == false)
        {
            m_factory->getSNMP()->sendTrap(TRAP_FILEMISSING,
                                         getPath());
            throw Exception(__FILE__, __LINE__,
                            "File '%s' at path '%s' does not exist",
                            getName().c_str(),
                            getPath().c_str());
        }
    }
    else if (mode == msg::key::FILEWRITE)
    {
        // request should be start
        if (in->getID() != msg::id::FILESTART)
        {
            throw Exception(__FILE__, __LINE__,
                            "Write operation should be locked. "
                            "Name: %s. Path: %s",
                            getName().c_str(),
                            getPath().c_str());

        }
    }
    else
    {
        throw Exception(__FILE__, __LINE__,
                        "Unknown file mode '%s'", mode.c_str());
    }
}

//
// FolderInfo class
//

// Constructor
FolderInfo::FolderInfo(const std::string& uuid,
                       const std::string& name,
                       const std::string& path,
                       IFactory* factory) :
    FileInfo(uuid, name, path, factory), m_file_uuid()
{
}

// Checks that info corresponds data stored at the info message
void FolderInfo::check(const IMessagePtr& in) const
{
    BOOST_ASSERT(in);
    const std::string mode = in->getValue(msg::key::FILEMODE);
    if (mode == msg::key::FILEREAD)
    {
        // we could not ask it for folder (it's undefined)
        throw Exception(__FILE__, __LINE__,
                        "Read operation is undefined for folder. "
                        "Name: %s. Path: %s",
                        getName().c_str(),
                        getPath().c_str());
    }

    // base check
    FileInfo::check(in);
}


// Fills output message with values from the object
void FolderInfo::fillOutMessage(const IMessagePtr& out) const
{
    // all check should be done before
    // at FolderInfo::check
    // now we should generate an unique name
    // and create a folder for the file
    const std::string folder = getPath() +
        base::Utils::getCurrentTime("/%Y/%m/%d/%H");
    // create it
    base::Utils::mkdir(folder);
    // path for the file
    const std::string name = base::Utils::generateUUID() + ".bin";
    const std::string path = folder + "/" + name;
    // add the path into our DB
    db::DB db(getFactory());
    db.connect();
    // CREATE PROCEDURE `klk_file_add_child` (
    // INOUT file VARCHAR(40),
    // IN name VARCHAR(40),
    // IN file_path VARCHAR(255),
    // IN host VARCHAR(40),
    // IN parent_uuid VARCHAR(40),
    // OUT return_value INT
    db::Parameters params;
    params.add("@file");
    params.add("@name", name);
    params.add("@file_path", path);
    params.add("@host", db.getHostUUID());
    params.add("@parent_uuid", getUUID());
    params.add("@return_value");
    db::Result res = db.callSimple("klk_file_add_child", params);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Adding a child failed. "
                        "Parent Name: %s. Parent Path: %s",
                        getName().c_str(),
                        getPath().c_str());
    }

    const std::string file_uuid = getFileUUID();
    // set uuid of current file for future end time update
    setFileUUID(res["@file"].toString());
    // process end time
    if (!file_uuid.empty())
    {
        // CREATE PROCEDURE `klk_file_update_end_time` (
	// IN file VARCHAR(40),
	// OUT return_value INT
        params.clear();
        params.add("@file", file_uuid);
        params.add("@return_value");
        res = db.callSimple("klk_file_update_end_time", params);
        if (res["@return_value"].toInt() != 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "Updating time for a child failed. "
                            "Parent Name: %s. Parent Path: %s",
                            getName().c_str(),
                            getPath().c_str());
        }
    }

    // All is fine. Fill output
    mod::Info::fillOutMessage(out);
    // we set a virtual path at the output
    out->setData(msg::key::FILEPATH, path);
}

// Sets file uuid
void FolderInfo::setFileUUID(const std::string& uuid) const
{
    BOOST_ASSERT(!uuid.empty());
    Locker lock(&m_lock);
    m_file_uuid = uuid;
}
