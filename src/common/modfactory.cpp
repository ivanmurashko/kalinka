/**
   @file modfactory.cpp
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
   - 2008/07/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include "common.h"
#include "log.h"

#include"modfactory.h"
#include "paths.h"
#include "utils.h"
#include "xml.h"
#include "commontraps.h"
#include "exception.h"

// modules specific info
#include "msgcore/defines.h"

using namespace klk;

//
// ModuleFactory class
//

// Constructor
ModuleFactory::ModuleFactory(IFactory *factory) :
    m_lock(),
    m_modules(), m_resources(), m_factory(factory),
    m_libfactory(NULL), m_scheduler(NULL), m_dependency()
{
    BOOST_ASSERT(m_factory);
}

// Destructor
ModuleFactory::~ModuleFactory()
{
    m_modules.clear();
    m_resources.clear();
    KLKDELETE(m_scheduler);
    // Lib factory should be last
    // because it keeps module handle
    KLKDELETE(m_libfactory);
}

// Gets libfactory when it's necessary
// @note use it instead of m_libfactory direct usage
LibFactory* ModuleFactory::getLibFactory()
{
    Locker lock(&m_lock);
    if (m_libfactory == NULL)
    {
        m_libfactory = new LibFactory(m_factory);
    }

    return m_libfactory;
}

// Gets scheduler when it's necessary
// @note use it instead of m_scheduler direct usage
base::Scheduler* ModuleFactory::getScheduler()
{
    Locker lock(&m_lock);
    try
    {
        if (m_scheduler == NULL)
        {
            m_scheduler = new base::Scheduler();
        }
    }
    catch(const std::bad_alloc&)
    {
        throw klk::Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }

    return m_scheduler;
}


// Loads a module by its id
// @param[in] id the module id
void ModuleFactory::load(const std::string& id)
{
    try
    {
        // create module instance
        IModulePtr module = getModule4Start(id);
        BOOST_ASSERT(module);

        // before load this module we should load all it's dependency
        StringList deps = m_dependency.getDependency(id);
        for (StringList::iterator i = deps.begin(); i != deps.end(); i++)
        {
            load(*i);
        }

        // start the thread
        startModuleThread(module);

        // wait until it will be really started
        time_t timeout = 60; // FIXME!!!  add to somewhere
        module->waitStart(timeout);
    }
    catch(const std::exception& err)
    {
        m_factory->getSNMP()->sendTrap(snmp::MODULE_LOAD_FAILED,
                                       id);

        klk_log(KLKLOG_ERROR, "Failed to load module '%s': %s",
                id.c_str(),
                err.what());
        throw;
    }
}

// Gets a module instance by its id
const IModulePtr ModuleFactory::getModule4Start(const std::string& id)
{
    Locker lock(&m_lock);
    // first of all look at the known modules list
    IModulePtr module = getModuleUnsafe(id);
    if (!module)
    {
        // not found here
        // will try to find at the modules description
        module = getLibFactory()->getModule(id);
        if (module)
        {
            addModule(module);
        }
    }

    if (!module)
    {
        throw Exception(__FILE__, __LINE__,
                        "Unknown module with id: " + id);
    }

    return module;
}

// Starts module thread
void ModuleFactory::startModuleThread(const IModulePtr& module)
{
    BOOST_ASSERT(module);

    Locker lock(&m_lock);

    IThreadPtr thread =
        boost::dynamic_pointer_cast<IThread, IModule>(module);
    BOOST_ASSERT(thread);
    if (getScheduler()->isStarted(thread) == false)
    {
        // before load the module we have to init it's devices
        IResourcesPtr resources = getResources(module->getID());
        if (resources)
        {
            resources->initDevs();
            m_factory->getResources()->add(resources);
        }
        getScheduler()->startThread(thread);
    }
    BOOST_ASSERT(getScheduler()->isStarted(thread) == true);
}


// Unloads a module by its id
// @param[in] id the module id
void ModuleFactory::unload(const std::string& id)
{
    BOOST_ASSERT(id.empty() == false);
    // first of all look at the known modules list
    IModulePtr module = getModule(id);
    if (module == NULL)
    {
        // can be called during unloading dependencies
        // that can be not loaded
        klk_log(KLKLOG_ERROR, "Failed to unload module '%s'. "
                "The module does not exist.", id.c_str());
        return;
    }

    IThreadPtr thread =
        boost::dynamic_pointer_cast<IThread, IModule>(module);
    BOOST_ASSERT(thread);
    if (getScheduler()->isStarted(thread))
    {
        getScheduler()->stopThread(thread);
    }
    BOOST_ASSERT(getScheduler()->isStarted(thread) == false);
}

// Unloads all loaded modules
void ModuleFactory::unloadAll()
{
    StringList modules = m_dependency.getSortedList();
    std::for_each(modules.begin(), modules.end(),
                  boost::bind(&ModuleFactory::unload, this, _1));
    // clear dependencies
    m_dependency.clear();

    // stop all left
    getScheduler()->stop();
}


// Gets module by its id
// @param[in] id the module's id
const IModulePtr ModuleFactory::getModule(const std::string& id)
{
    Locker lock(&m_lock);
    IModulePtr res = getModuleUnsafe(id);
    return res;
}

// Gets resources by its id
// @param[in] id the resources's id
const IResourcesPtr ModuleFactory::getResources(const std::string& id)
{
    Locker lock(&m_lock);
    IResourcesPtr res = getResourcesUnsafe(id);
    if (res == NULL)
    {
        res = getLibFactory()->getResources(id);
        // we add NULL resources to prevent a lot of calls to a lib
        addResources(id, res);
    }
    return res;
}


// Gets module by its id
// @param[in] id the module's id
const IModulePtr ModuleFactory::getModule(const std::string& id) const
{
    Locker lock(&m_lock);

    ModuleList::const_iterator i = std::find_if(
        m_modules.begin(), m_modules.end(),
        boost::bind(std::equal_to<std::string>(), id,
                    boost::bind(&IModule::getID, _1)));
    if (i == m_modules.end())
    {
        BOOST_ASSERT(false);
    }

    return *i;
}

// Gets module by its id
IModulePtr ModuleFactory::getModuleUnsafe(const std::string& id)
{
    BOOST_ASSERT(id.empty() == false);

    IModulePtr res;
    ModuleList::iterator i = std::find_if(
        m_modules.begin(), m_modules.end(),
        boost::bind(std::equal_to<std::string>(), id,
                    boost::bind(&IModule::getID, _1))
        );
    if (i != m_modules.end())
    {
        res = *i;
        BOOST_ASSERT(res);
    }

    return res;
}

// Gets resources by module's id
IResourcesPtr ModuleFactory::getResourcesUnsafe(const std::string& id)
{
    BOOST_ASSERT(id.empty() == false);

    IResourcesPtr res;
    ResourcesMap::iterator i = m_resources.find(id);
    if (i != m_resources.end())
    {
        res = i->second;
    }

    return res;
}


// Adds a module to the known modules list
void ModuleFactory::addModule(const IModulePtr& module)
{
    BOOST_ASSERT(module);
    std::string modid = module->getID();
    BOOST_ASSERT(getModuleUnsafe(modid) == NULL);
    // register messages
    module->registerProcessors();
    m_modules.push_back(module);
}

// Adds resources to the known resources list
void ModuleFactory::addResources(const std::string& modid,
                                 const IResourcesPtr& res)
{
    BOOST_ASSERT(getResourcesUnsafe(modid) == NULL);
    m_resources.insert(ResourcesMap::value_type(modid, res));
}


// Sends a message for processing
// @param[in] msg the pointer to the message that has to be sent
// @note the message type and receivers have to be set by the caller
void ModuleFactory::sendMessage(const IMessagePtr& msg)
{
    // check parameters
    BOOST_ASSERT(msg);
    // reseiver should be
    size_t reciever_count = msg->getReceiverList().size();

    switch(msg->getType())
    {
    case msg::ASYNC:
    {
        BOOST_ASSERT(reciever_count != 0);
        break;
    }
    case msg::SYNC_REQ:
    case msg::SYNC_RES:
    {
        BOOST_ASSERT(reciever_count == 1);
        break;
    }
    default:
    {
        BOOST_ASSERT(false);
    }
    }

    // get the message core module
    IModulePtr core = getModule(msgcore::MODID);
    BOOST_ASSERT(core);
    // add the message to the core for dispatching
    core->addMessage(msg);
}

// Retrieves a list with available module ids
// static
StringList ModuleFactory::getModuleIDs()
{
    StringList list;

    boost::filesystem::path folder(dir::SHARE);
    folder /= "modules";
    // default construction yields past-the-end
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(folder);
         itr != end_itr; itr++)
    {
        if (!boost::filesystem::is_regular_file(itr->status()))
            continue; // ignore directories
        // process only files with .xml extension
        boost::filesystem::path file(itr->path());
        if (file.extension() != ".xml")
            continue;

        XML xml;
        try
        {
            xml.parseFromFile(file.string());
        }
        catch(const std::exception& err)
        {
            klk_log(KLKLOG_ERROR, "Failed to get module info from '%s': %s",
                    file.string().c_str(), err.what());
            continue;
        }

        const std::string id = xml.getValue("/klkdata/module/id");
        list.push_back(id);
    }
    return list;
}


// @copydoc klk::IModuleFactory::getModulesWithoutRegistration
const ModuleList ModuleFactory::getModulesWithoutRegistration()
{
    ModuleList list;
    StringList ids = getModuleIDs();
    for(StringList::iterator id = ids.begin(); id != ids.end(); id++)
    {
        Locker lock(&m_lock);
        IModulePtr module = getModuleUnsafe(*id);
        if (!module)
        {
            // not found here
            // will try to find at the modules description
            module = getLibFactory()->getModule(*id);
        }
        if (module)
        {
            list.push_back(module);
        }
    }
    return list;
}

// Retrives all modules in the form of list
const ModuleList ModuleFactory::getModules()
{
    //FIXME!!! move it to lib factory class
    StringList ids = getModuleIDs();
    Locker lock(&m_lock);
    for(StringList::iterator id = ids.begin(); id != ids.end(); id++)
    {
        IModulePtr module = getModuleUnsafe(*id);
        if (!module)
        {
            // not found here
            // will try to find at the modules description
            module = getLibFactory()->getModule(*id);
            BOOST_ASSERT(module);
            if (module)
            {
                addModule(module);
            }
        }

    }
    return m_modules;
}

// Checks if the module loaded or not
// @param[in] id - the module id to be tested
bool ModuleFactory::isLoaded(const std::string& id)
{
    try
    {
        BOOST_ASSERT(id.empty() == false);
        IModulePtr module = getModule(id);
        if (module == NULL)
            return false;

        BOOST_ASSERT(module);
        IThreadPtr thread =
            boost::dynamic_pointer_cast<IThread, IModule>(module);
        BOOST_ASSERT(thread);
        if (getScheduler()->isStarted(thread) == false)
            return false;
        // check that initialization part was passed
        return module->isStarted();
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Error while checking module load state: %s",
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Unknown exception while checking module load state");
    }

    return false;
}


// Adds a dependency between modules
Result ModuleFactory::addDependency(const std::string& child,
                                    const std::string& parent)
{
    return m_dependency.addDependency(child, parent);
}


// Removes a dependency between modules
void ModuleFactory::rmDependency(const std::string& child,
                                 const std::string& parent)
{
    return m_dependency.rmDependency(child, parent);
}
