/**
   @file libfactory.cpp
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
   - 2008/08/01 created by ivmu
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <boost/bind.hpp>

#include "modfactory.h"
#include "common.h"
#include "log.h"
#include "paths.h"
#include "utils.h"
#include "xml.h"
#include "exception.h"

using namespace klk;

//
// LibFactory class
//

// Constructor
LibFactory::LibFactory(IFactory *factory) :
    Mutex(), m_factory(factory)
{
    KLKASSERT(m_factory != NULL);
}

// Destructor
LibFactory::~LibFactory()
{
}

// Gets module by its id from a modules description
// @param[in] id the module's id
// @return the pointer to a module or NULL if there was an error
// @note - there is an internal method that does not do any locks
const IModulePtr LibFactory::getModule(const std::string& id)
{
    BOOST_ASSERT(id.empty() == false);
    void *handle = NULL;
    handle = getMakeFun(id, "klk_module_get");
    BOOST_ASSERT(handle);

    typedef IModulePtr (*modfactory_func_type) (IFactory*);
    modfactory_func_type fn =
        reinterpret_cast<modfactory_func_type>(handle);
    BOOST_ASSERT(fn);

    IModulePtr res = (*fn)(m_factory);
    BOOST_ASSERT(res);

    return res;
}

// Gets resources by modules id
const IResourcesPtr LibFactory::getResources(const std::string& id)
{
    BOOST_ASSERT(id.empty() == false);
    void *handle = NULL;
    handle = getMakeFun(id, "klk_resources_get");
    IResourcesPtr res;
    if (handle)
    {
        typedef IResourcesPtr (*resfactory_func_type) (IFactory*);
        resfactory_func_type fn =
            reinterpret_cast<resfactory_func_type>(handle);
        BOOST_ASSERT(fn);

        res = (*fn)(m_factory);
        BOOST_ASSERT(res);
    }
    else
    {
        klk_log(KLKLOG_DEBUG, "There is no resources make function "
                "for module: %s", id.c_str());
    }

    return res;
}



// Gets module's path by its id from a modules description
// @param[in] id the module's id
// @param[in] fname - the file name for test
// @return the pointer to a module or NULL if there was an error
// @note - there is an internal method that does not do any locks
std::string LibFactory::getPathFromDescription(
    const std::string& id,
    const std::string& fname) const
{
    std::string res = "";
    // process only files with .xml extension
    if (base::Utils::getFileExt(fname) != "xml")
    {
        // nothing to do
        return res;
    }
    try
    {
        XML xml;
        xml.parseFromFile(fname);
        std::string xpath_id = "/klkdata/module/id";
        if (xml.getValue(xpath_id) == id)
        {
            std::string xpath_lib = "/klkdata/module/lib_path";
            // found it
            res = xml.getValue(xpath_lib);
        }
    }
    catch(const std::exception& err)
    {
        // ignore errors
        klk_log(KLKLOG_ERROR, "Got an error while processing '%s': %s",
                fname.c_str(), err.what());
    }

    return res;
}

// Gets module lib path by the module id
std::string LibFactory::getPath(const std::string& id) const
{
    std::string res;
    std::string folder = dir::SHARE + "/modules";
    StringList list = base::Utils::getFiles(folder);
    for (StringList::iterator i = list.begin(); i != list.end(); i++)
    {
        res = getPathFromDescription(id, *i);
        if (!res.empty())
            break;
    }
    if (res.empty())
    {
        throw Exception(__FILE__, __LINE__,
                             "Cannot get library path for module with id: " +
                             id);
    }
    return res;
}

// Gets make function
// @param[in] id - the module id
void* LibFactory::getMakeFun(const std::string& id,
                             const std::string& signature)
{
    BOOST_ASSERT(id.empty() == false);
    BOOST_ASSERT(signature.empty() == false);

    std::string libpath = getPath(id);
    BOOST_ASSERT(libpath.empty() == false);

    Locker lock(this);
    LibContainerMap::iterator iter = m_handles.find(libpath);
    if (iter != m_handles.end())
    {
        BOOST_ASSERT(iter->second);
        return iter->second->sym(signature);
    }

    // we should open the lib and add it
    LibContainerPtr lib(new LibContainer(libpath));
    lib->open();
    void *res = lib->sym(signature);
    m_handles.insert(LibContainerMap::value_type(libpath, lib));
    return res;
}

