/**
   @file service.cpp
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
   - 2008/08/22 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/resource.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#ifdef DARWIN
#include <mach/mach_init.h>
#include <mach/task.h>
#endif

#include <boost/bind.hpp>
#include <iostream>

#include "service.h"
#include "common.h"
#include "log.h"
#include "defines.h"
#include "clicommands.h"
#include "messages.h"
#include "db.h"
#include "paths.h"

using namespace klk;
using namespace klk::srv;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new Service(factory));
}

//
// Service
//

// Constructor
// @param[in] factory the module factory
Service::Service(IFactory *factory) :
    ModuleWithDB(factory, MODID),
    m_apps(),
    m_global_usage(RUSAGE_SELF),
    m_cpu_usage(-1)
{
}

// Destructor
Service::~Service()
{
}

// Gets a human readable module name
// @return the name
const std::string Service::getName() const throw()
{
    return MODNAME;
}

// Does pre actions before start main loop
void Service::preMainLoop()
{
    // preparation staff
    ModuleWithDB::preMainLoop();
}


// Do final cleanup after stop
void Service::postMainLoop() throw()
{
    stopApplications();
    ModuleWithDB::postMainLoop();
}

// Register all processors
void Service::registerProcessors()
{
    ModuleWithDB::registerProcessors();

    registerSync( msg::id::SRVMODCHECK,
                  boost::bind(&Service::checkModule, this, _1, _2));

    registerASync(msg::id::SRVMODINFO,
                  boost::bind(&Service::updateCPUUsage4App, this, _1));

    registerCLI(cli::ICommandPtr(new InfoCommand()));
    registerCLI(cli::ICommandPtr(new ModInfoCommand()));
    registerCLI(cli::ICommandPtr(new AppInfoCommand()));
    registerCLI(cli::ICommandPtr(new LoadModuleCommand()));
    registerCLI(cli::ICommandPtr(new LoadApplicationCommand()));

    const time_t UPDATEINTERVAL = 60;

    registerTimer(boost::bind(&Service::updateCPUUsage, this),
                  UPDATEINTERVAL);

}

// Gets application list
// @return the application list for the host from the DB
// @exception @ref Result
AppInfoList Service::getApplications()
{
    db::DB db(getFactory());
    db.connect();
    db::Parameters params_select;
    params_select.add("@host", db.getHostUUID());
    db::ResultVector rv =
        db.callSelect("klk_application_list", params_select, NULL);
    AppInfoList list;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT
        // application, module,
        // name, status, prioriry, description
        const std::string app = (*i)["application"].toString();
        const std::string mod = (*i)["module"].toString();
        const std::string name = (*i)["name"].toString();
        // ignore main application see ticket #240
        if (mod != klk::MODULE_COMMON_ID)
        {
            list.push_back(AppInfoPtr(new AppInfo(app, mod, name)));
        }
    }
    return list;
}

// Starts applications for the daemon
void Service::processDB(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg);

    AppInfoList new_apps = getApplications();

    // first of all stop missing apps at the list
    AppInfoList list = m_apps.getMissing(new_apps);
    klk_log(KLKLOG_DEBUG, "Service: %d old application stopped",
            list.size());
    std::for_each(list.begin(), list.end(),
                  boost::bind(&Service::stopApplication, this, _1));
    // adds new apps
    klk_log(KLKLOG_DEBUG, "Service: %d new application started",
            new_apps.size());
    std::for_each(new_apps.begin(), new_apps.end(),
                  boost::bind(&Service::startApplication, this, _1));
}

// Stops applications for the daemon
void Service::stopApplications()
{
    AppInfoList apps = m_apps.getAll();
    std::for_each(apps.begin(), apps.end(),
                  boost::bind(&Service::stopApplication, this, _1));
}


// Loads an application
void Service::startApplication(const AppInfoPtr& info) throw()
{
    if (info == NULL)
    {
        KLKASSERT(info != NULL);
        return;
    }

    try
    {
        if (m_apps.find(info))
        {
            klk_log(KLKLOG_ERROR, "The application '%s' is already loaded",
                    info->getName().c_str());
            return;
        }

        // starts the application monitor
        // inside a separate thread
        ThreadPtr thread(new boost::thread(
                             boost::bind(&Service::doApplicationMonitor,
                                         this,
                                         info)));
        info->setThread(thread);
    }
    catch(const std::exception& err)
    {
        // not critical error
        klk_log(KLKLOG_ERROR,
                "Failed to load an application. "
                "Some functionality will not be available. "
                "Application uuid: %s. Error: %s",
                info->getAppUUID().c_str(),
                err.what());
        stopApplication(info);
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Failed to load an application. "
                "Unspecified exception. "
                "Some functionality will not be available. "
                "Application uuid: %s", info->getAppUUID().c_str());
        stopApplication(info);
    }
}

// Stops an application
void Service::stopApplication(const AppInfoPtr& info) throw()
{
    if (info == NULL)
    {
        KLKASSERT(info != NULL);
        return;
    }

    try
    {
        // stop it
        if (kill(info->getPid(), SIGTERM) < 0)
        {
            throw Exception(__FILE__, __LINE__, "Error %d in kill(): %s",
                            errno, strerror(errno));
        }

        ThreadPtr thread = info->getThread();
        BOOST_ASSERT(thread);

        int timeout = 5;
        if (!thread->timed_join(boost::posix_time::seconds(timeout)))
        {
            klk_log(KLKLOG_ERROR, "Application '%s' could not be stopped "
                    "in '%d' s. It will be interruped with SIGKILL. "
                    "Application pid '%d'",
                    info->getName().c_str(), timeout, info->getPid());

            if (kill(info->getPid(), SIGKILL) < 0)
            {
                throw Exception(__FILE__, __LINE__,
                                "Error %d in kill(): %s",
                                errno, strerror(errno));
            }
            // wait until end
            thread->join();
            klk_log(KLKLOG_ERROR, "Application '%s' stopped with SIGKILL. "
                    "Application pid '%d'",
                    info->getName().c_str(), info->getPid());
        }
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Failed to unload an application. "
                "Some functionality will not be available."
                "Application uuid: %s. Error: %s",
                info->getAppUUID().c_str(),
                err.what());
    }
}

// Checks is the module loaded or not
void Service::checkModule(const IMessagePtr& in,
                          const IMessagePtr& out)
{
    BOOST_ASSERT(in);
    BOOST_ASSERT(out);

    const std::string mod_id = in->getValue(msg::key::SRVMODID);
    BOOST_ASSERT(mod_id.empty() == false);

    if (getFactory()->getModuleFactory()->isLoaded(mod_id))
    {
        // module is loaded
        out->setData(msg::key::SRVMODSTATUS, msg::key::OK);
    }
    else if  (getAppInfo(mod_id))
    {
        // application is loaded
        out->setData(msg::key::SRVMODSTATUS, msg::key::OK);
    }
    else
    {
        // nothing was loaded
        out->setData(msg::key::SRVMODSTATUS, msg::key::FAILED);
    }
}

// Gets total CPU usage
const double Service::getTotalCPUUsage()
{
    Locker lock(&m_global_usage);
    if (m_cpu_usage < 0)
    {
        // update for the first call
        updateCPUUsage();
    }
    return m_cpu_usage;
}

// Gets total Memory usage
const long Service::getTotalMemUsage() const
{
    long res = 0;
#ifdef DARWIN
    // code taken from
    // http://www.platinumball.net/blog/2009/05/03/cocoa-app-memory-usage/
    task_basic_info         info;
    kern_return_t           rval = 0;
    mach_port_t             task = mach_task_self();
    mach_msg_type_number_t  tcnt = TASK_BASIC_INFO_COUNT;
    task_info_t             tptr = (task_info_t) &info;

    memset(&info, 0, sizeof(info));

    rval = task_info(task, TASK_BASIC_INFO, tptr, &tcnt);
    if (!(rval == KERN_SUCCESS))
        throw Exception(__FILE__, __LINE__, "Error in task_info()");

    res = info.resident_size;
#elif LINUX
    // FIXME!!! bad code
    // Unfortunately getrusage does not return memory
    // usage in Linux :(
    // as result we have to use this 'bad' code
    char buf[128];
    snprintf(buf, sizeof(buf), "/proc/%u/statm",
             static_cast<unsigned>(getpid()));
    FILE* pf = fopen(buf, "r");
    if (pf)
    {
        unsigned size; //       total program size
        //unsigned resident;//   resident set size
        //unsigned share;//      shared pages
        //unsigned text;//       text (code)
        //unsigned lib;//        library
        //unsigned data;//       data/stack
        //unsigned dt;//         dirty pages (unused in Linux 2.6)
        fscanf(pf, "%u" /* %u %u %u %u %u"*/, &size/*, &resident, &share, &text, &lib, &data*/);
        res = size * 1024; // size in kb
        fclose(pf);
    }
#else
#error "Unsupported platform"
#endif

    return res;
}

// Updates global CPU usage
void Service::updateCPUUsage()
{
    Locker lock(&m_global_usage);
    m_cpu_usage = m_global_usage.getCPUUsage();
}

// @brief Main application thread loop
void Service::doApplicationMonitor(const AppInfoPtr& info)
try
{
    // FIXME!!! complex code
    //

    // add it to the running applications list
    m_apps.add(info);

    // start the application as a separate process
    pid_t pid = 0;

    const std::string launcher = dir::INST + "/sbin/klklauncher";

    while(true)
    {
        switch (pid = fork())
        {
        case -1:
            throw Exception(__FILE__, __LINE__, "Error %d in fork(): %s\n",
                            errno, strerror(errno));
        case 0:
            execl(launcher.c_str(), launcher.c_str(),
                  "-m", info->getModUUID().c_str(),
                  "-n", info->getName().c_str(),
                  "-c", getFactory()->getConfig()->getPath().c_str(),
                  NULL);
            exit(errno);
        }

        info->setPid(pid);

        // wait for the child end
        int status = 0;
        if ((waitpid(pid, &status, 0) == pid) < 0)
        {
            throw Exception(__FILE__, __LINE__, "Error %d in waitpid(): %s",
                            errno, strerror(errno));
        }


        // check that it was stopped by a signal
        if (!WIFSIGNALED(status))
        {
            klk_log(KLKLOG_DEBUG, "Application '%s' exited with status '%d'",
                    info->getName().c_str(),
                    status);
            break;
        }

        // deternime the signal type
        int signal = WTERMSIG(status);
        if (signal == SIGSEGV ||
            signal == SIGABRT ||
            signal == SIGBUS)
        {
            klk_log(KLKLOG_ERROR,
                    "Application '%s' was interupted by signal '%d'"
                    " and will be restarted. "
                    "Application pid: %d",
                    info->getName().c_str(),
                    signal, pid);
            continue;
        }
        else
        {
            klk_log(KLKLOG_DEBUG,
                    "Application '%s' was interupted by signal '%d'",
                    info->getName().c_str(),
                    signal);
            break;
        }
    }

    m_apps.remove(info);
}
catch(const std::exception& err)
{
    klk_log(KLKLOG_ERROR, "Application startup failed. Error: %s", err.what());
    m_apps.remove(info);
}
catch(...)
{
    klk_log(KLKLOG_ERROR, "Application startup failed. Unknown error");
    m_apps.remove(info);
}

/// the application info finder
struct FindAppInfoByModID
{
    /// the finder functor
    bool operator()(const AppInfoPtr& info, const std::string& id){
        return (info->getModUUID() == id);
    }
};

// Retrives application pid by its uuid
const AppInfoPtr Service::getAppInfo(const std::string& mod_id) const
{
    AppInfoList list = m_apps.getAll();
    AppInfoList::iterator find = std::find_if(list.begin(), list.end(),
                                              boost::bind<bool>(FindAppInfoByModID(), _1, mod_id));
    if (find != list.end())
    {
        return *find;
    }

    return AppInfoPtr();
}

// Updates application cpu usage with info gotten in the message
void Service::updateCPUUsage4App(const IMessagePtr& in)
{
    BOOST_ASSERT(in);
    const std::string mod_id = in->getValue(msg::key::SRVMODID);
    BOOST_ASSERT(mod_id.empty() == false);
    AppInfoPtr info = getAppInfo(mod_id);
    const std::string cpu_usage = in->getValue(msg::key::SRVMODCPUUSAGE);
    BOOST_ASSERT(cpu_usage.empty() == false);
    info->setCPUUsage(cpu_usage);
}
