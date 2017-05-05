/**
   @file clicommands.cpp
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
   - 2008/11/14 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>

#include <boost/bind.hpp>

#include "service.h"
#include "clicommands.h"
#include "defines.h"
#include "utils.h"

using namespace klk;
using namespace klk::srv;

//
// InfoCommand class
//

const std::string INFONAME = "status";
const std::string INFOSUMMARY = "Shows server status";
const std::string INFOUSAGE = "Usage: " + INFONAME + "\n";

// Constructor
InfoCommand::InfoCommand() :
    cli::Command(INFONAME, INFOSUMMARY, INFOUSAGE)
{
}

// Process the command
const std::string InfoCommand::process(const cli::ParameterVector& params)

{
    std::stringstream data;
    ServicePtr srv = getModule<Service>();
    BOOST_ASSERT(srv);
    data << "Mediaserver is running\n";
    data << "Uptime: ";
    data << base::Utils::time2Str(srv->getUptime());
    data << "\n";
    data << "CPU usage: ";
    data << base::Utils::percent2Str(srv->getTotalCPUUsage());
    data << "\n";
    data << "Memory usage: ";
    data << base::Utils::memsize2Str(srv->getTotalMemUsage());
    data << "\n";
    return data.str();
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
InfoCommand::getCompletion(const cli::ParameterVector& setparams)
{
    // no parameters
    return cli::ParameterVector();
}

//
// InfoHelper class
//

// the not loaded string
const std::string NOTLOADED("not loaded");
// not available string
const std::string NOTAVAILABLE("n/a");

// Constructor
InfoHelper::InfoHelper(const std::string& name,
                       const std::string& summary,
                       const std::string& usage,
                       mod::Type type) :
    cli::Command(name, summary, usage), m_type(type)
{
}

// Creates a response
const std::string InfoHelper::getResponse(IModuleFactory* factory,
                                          const std::string& name)
{
    BOOST_ASSERT(factory);
    ModuleList list = factory->getModulesWithoutRegistration();
    size_t maxsize = getMaxSize(factory, list);
    std::stringstream data;
    data << base::Utils::align("Name", maxsize);
    data << "  ";
    data << base::Utils::align("Version", maxsize);
    data << "  ";
    data << base::Utils::align("Uptime", maxsize);
    data << "  ";
    data << "CPU usage\n";
    bool wasFound = false;
    for (ModuleList::iterator i = list.begin(); i != list.end(); i++)
    {
        if ((*i)->getType() != m_type)
            continue;
        if (name != cli::ALL && (*i)->getName() != name)
            continue;
        wasFound = true;
        data << base::Utils::align((*i)->getName(), maxsize);
        data << "  ";
        data << base::Utils::align((*i)->getVersion(), maxsize);
        data << "  ";
        data << base::Utils::align(getLoadInfo(factory, (*i)->getID()), maxsize);
        data << "  ";
        data << base::Utils::align(getCPUUsageInfo(factory, (*i)->getID()), maxsize);
        data << "\n";
    }
    if (!wasFound)
    {
        if (m_type == mod::APP)
        {
            return "Unknown application: " + name + "\n";
        }
        else
        {
            return "Unknown module: " + name + "\n";
        }
    }
    return data.str();
}

// Process the command
const std::string InfoHelper::process(const cli::ParameterVector&
                                      params)
{
    std::stringstream data;
    if (params.size() == 1)
    {
        data << getResponse(getModuleFactory(), params[0]);
    }
    else
    {
        data << "Incorrect parameters\n" << getUsage() << "\n";
    }
    return data.str();
}


// Finds max  base::Utils::align ment length
size_t InfoHelper::getMaxSize(IModuleFactory* factory,
                              const ModuleList& list) const
{
    size_t res = 0;
    for (ModuleList::const_iterator i = list.begin(); i != list.end(); i++)
    {
        if (res < (*i)->getName().size())
            res = (*i)->getName().size();
        if (res < (*i)->getVersion().size())
            res = (*i)->getVersion().size();
        if (res < getLoadInfo(factory, (*i)->getID()).size())
            res = getLoadInfo(factory, (*i)->getID()).size();
        if (res < getCPUUsageInfo(factory, (*i)->getID()).size())
            res = getCPUUsageInfo(factory, (*i)->getID()).size();
    }

    return res;
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
InfoHelper::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;

    if (setparams.empty())
    {
        res.push_back(cli::ALL);
        ModuleList list = getModuleFactory()->getModulesWithoutRegistration();
        for (ModuleList::iterator i = list.begin(); i != list.end(); i++)
        {
            if ((*i)->getType() != m_type)
                continue;
            res.push_back((*i)->getName());
        }
    }

    return res;
}

//
// AppInfoCommand class
//

const std::string APPINFONAME = "application status";
const std::string APPINFOSUMMARY = "Shows applications status";
const std::string APPINFOUSAGE = "Usage: "
                + APPINFONAME + " [application_name|" + cli::ALL + "]\n";

// Constructor
AppInfoCommand::AppInfoCommand() :
    InfoHelper(APPINFONAME, APPINFOSUMMARY, APPINFOUSAGE, mod::APP)
{
}

/// @copydoc klk::srv::InfoHelper::getLoadInfo()
const std::string AppInfoCommand::getLoadInfo(IModuleFactory* factory,
                                              const std::string& id) const
{
    const AppInfoPtr info = getModule<Service>()->getAppInfo(id);
    if (info)
    {
        return base::Utils::time2Str(info->getUptime());
    }

    return NOTAVAILABLE;
}

/// @copydoc klk::srv::InfoHelper::getCPUUsageInfo()
const std::string AppInfoCommand::getCPUUsageInfo(IModuleFactory* factory,
                                                  const std::string& id) const
{
    const AppInfoPtr info = getModule<Service>()->getAppInfo(id);
    if (info)
    {
        return info->getCPUUsage();
    }

    return NOTLOADED;
}

//
// ModInfoCommand class
//

const std::string MODINFOSUMMARY = "Shows module(s) status";
const std::string MODINFOUSAGE = "Usage: " +
                MODINFONAME + " [module_name|" + cli::ALL + "]\n";

// Constructor
ModInfoCommand::ModInfoCommand() :
    InfoHelper(MODINFONAME, MODINFOSUMMARY, MODINFOUSAGE, mod::MOD)
{
}

/// @copydoc klk::srv::InfoHelper::getLoadInfo()
const std::string ModInfoCommand::getLoadInfo(IModuleFactory* factory,
                                              const std::string& id) const
{
    if (factory->isLoaded(id))
    {
        return base::Utils::time2Str(factory->getModule(id)->getUptime());
    }
    return NOTLOADED;
}

/// @copydoc klk::srv::InfoHelper::getCPUUsageInfo()
const std::string ModInfoCommand::getCPUUsageInfo(IModuleFactory* factory,
                                                  const std::string& id) const
{
    if (factory->isLoaded(id))
    {
        return base::Utils::percent2Str(factory->getModule(id)->getCPUUsage());
    }
    return NOTAVAILABLE;
}


//
// LoadModuleCommand class
//

const std::string LOADMODSUMMARY = "Starts a module";
const std::string LOADMODUSAGE = "Usage: " + LOADMODNAME + " [module_name]\n";

// Constructor
LoadModuleCommand::LoadModuleCommand() :
    cli::Command(LOADMODNAME, LOADMODSUMMARY, LOADMODUSAGE)
{
}

// compare predicate
struct CompareLoadedOrApp
{
    bool operator()(const IModulePtr& module, IModuleFactory* factory)
        {
            if (module->getType() != mod::MOD)
                return true;
            if (factory->isLoaded(module->getID()))
                return true;
            return false;
        }
};

// Retrives usage string
const std::string LoadModuleCommand::getUsage()
{
    BOOST_ASSERT(getFactory());
    ModuleList list = getModuleFactory()->getModulesWithoutRegistration();
    // remove loaded and applications
    list.remove_if(boost::bind<bool>(CompareLoadedOrApp(), _1,
                                     getModuleFactory()));

    std::stringstream data;
    data << LOADMODUSAGE;
    data << "where <module> is one of the following: ";
    for (ModuleList::iterator i = list.begin(); i != list.end(); i++)
    {
        if (i != list.begin())
            data << ", ";
        data << (*i)->getName();
    }
    data << "\n";

    return data.str();
}

// Process the command
const std::string LoadModuleCommand::process(
    const cli::ParameterVector& params)

{
    if (params.empty() || params.size() > 1)
    {
        return getUsage();
    }

    BOOST_ASSERT(getModuleFactory());
    ModuleList list = getModuleFactory()->getModulesWithoutRegistration();
    // remove loaded and applications
    list.remove_if(boost::bind<bool>(CompareLoadedOrApp(), _1,
                                     getModuleFactory()));
    // find the module
    ModuleList::iterator i = std::find_if(list.begin(), list.end(),
                                          boost::bind(
                                              std::equal_to<std::string>(),
                                              params[0],
                                              boost::bind(
                                                  &IModule::getName,
                                                  _1)));
    if (i == list.end())
    {
        return getUsage();
    }

    getModuleFactory()->load((*i)->getID());
    std::stringstream data;
    data << "Module '" << params[0] << "' has been loaded\n";

    return data.str();
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
LoadModuleCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;

    if (setparams.empty())
    {
        ModuleList list = getModuleFactory()->getModulesWithoutRegistration();
        for (ModuleList::iterator i = list.begin(); i != list.end(); i++)
        {
            if ((*i)->getType() == mod::MOD)
                res.push_back((*i)->getName());
        }
    }

    return res;
}


//
// LoadApplicationCommand class
//

const std::string LOADAPPSUMMARY = "Starts a application";
const std::string LOADAPPUSAGE = "Usage: " + LOADAPPNAME + " [application]\n";

// Constructor
LoadApplicationCommand::LoadApplicationCommand() :
    cli::Command(LOADAPPNAME, LOADAPPSUMMARY, LOADAPPUSAGE)
{
}

// compare predicate
struct CompareLoadedOrMod
{
    bool operator()(const IModulePtr& application, IModuleFactory* factory)
        {
            if (application->getType() != mod::APP)
                return true;
            if (factory->isLoaded(application->getID()))
                return true;
            return false;
        }
};

// Retrives usage string
const std::string LoadApplicationCommand::getUsage()
{
    BOOST_ASSERT(getModuleFactory());
    ModuleList list = getModuleFactory()->getModulesWithoutRegistration();
    // remove loaded and applications
    list.remove_if(boost::bind<bool>(CompareLoadedOrMod(), _1,
                                     getModuleFactory()));

    std::stringstream data;
    data << LOADAPPUSAGE;
    data << "where [application] is one of the following: ";
    for (ModuleList::iterator i = list.begin(); i != list.end(); i++)
    {
        if (i != list.begin())
            data << ", ";
        data << (*i)->getName();
    }
    data << "\n";

    return data.str();
}


// Process the command
const std::string LoadApplicationCommand::process(
    const cli::ParameterVector& params)
{
    if (params.empty() || params.size() > 1)
    {
        return getUsage();
    }

    BOOST_ASSERT(getModuleFactory());
    ModuleList list = getModuleFactory()->getModulesWithoutRegistration();
    // remove loaded and applications
    list.remove_if(boost::bind<bool>(CompareLoadedOrMod(), _1,
                                     getModuleFactory()));
    // find the application
    ModuleList::iterator i = std::find_if(list.begin(), list.end(),
                                          boost::bind(
                                              std::equal_to<std::string>(),
                                              params[0],
                                              boost::bind(
                                                  &IModule::getName,
                                                  _1)));
    if (i == list.end())
    {
        return getUsage();
    }

    getModuleFactory()->load((*i)->getID());
    std::stringstream data;
    data << "Application '" << params[0] << "' has been started\n";

    return data.str();
}

// Retrives list of possible completions for a n's parameter
const cli::ParameterVector
LoadApplicationCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;

    if (setparams.empty())
    {
        ModuleList list = getModuleFactory()->getModulesWithoutRegistration();
        for (ModuleList::iterator i = list.begin(); i != list.end(); i++)
        {
            if ((*i)->getType() == mod::APP)
                res.push_back((*i)->getName());
        }
    }

    return res;
}
