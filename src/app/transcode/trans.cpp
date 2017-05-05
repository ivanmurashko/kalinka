/**
   @file trans.cpp
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
   - 2009/03/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "trans.h"
#include "exception.h"
#include "defines.h"
#include "cliapp.h"
#include "db.h"
#include "sourcecmd.h"
#include "taskcmd.h"

#include "snmp/factory.h"
#include "snmp/scalar.h"
#include "snmp/table.h"

// modules specific info
#include "file/defines.h"
#include "httpsrc/defines.h"
#include "network/defines.h"
#ifdef LINUX
#include "ieee1394/defines.h"
#include "ieee1394/messages.h"
#endif //LINUX


using namespace klk;
using namespace klk::trans;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new Transcode(factory));
}

//
// Transcode class
//

/**
   Application set command id
*/
const std::string SETCLI_MSGID = "e9580d7b-41ca-44e3-b3d1-d15d95bbaf9c";

/**
   Application show command id
*/
const std::string SHOWCLI_MSGID = "6a339133-0b6a-4e52-b83f-8dc0ff53f479";


// Constructor
Transcode::Transcode(IFactory *factory) :
    launcher::Module(factory, MODID, SETCLI_MSGID, SHOWCLI_MSGID),
    m_source_factory(), m_scheduler(factory)
{
    addDependency(net::MODID);
    addDependency(file::MODID);
    addDependency(httpsrc::MODID);
#ifdef LINUX
    addDependency(fw::MODID);
#endif //LINUX
}

// Destructor
Transcode::~Transcode()
{
}

// Gets a human readable module name
const std::string Transcode::getName() const throw()
{
    return MODNAME;
}

// Retrives an set with data from @ref grDB "database"
const Transcode::InfoSet Transcode::getTaskInfoFromDB()
{
    // read tasks by input sources

    // klk_app_transcode_get

    db::DB db(getFactory());
    db.connect();

    db::Parameters params;

    db::ResultVector rv = db.callSelect("klk_app_transcode_get",
                                        params, NULL);

    InfoSet set;
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        // Tasks specific

        // input source specific
        // input_source.source_uuid AS input_uuid,
        const std::string input_uuid = (*item)["input_uuid"].toString();
        // input_source.source_name AS input_name,
        const std::string input_name = (*item)["input_name"].toString();
        // input_source.source_type AS input_source_type,
        const std::string input_type =
            (*item)["input_source_type"].toString();
        // input_source.media_type AS input_media_type,
        const std::string input_media_type =
            (*item)["input_media_type"].toString();
        SourceInfoPtr input =
            m_source_factory.getSource(input_uuid, input_name, input_media_type,
                                       input_type);

        // output source specific
        // output_source.source_uuid AS output_uuid,
        const std::string output_uuid = (*item)["output_uuid"].toString();
        // output_source.source_name AS output_name,
        const std::string output_name = (*item)["output_name"].toString();
        // output_source.source_type AS output_source_type,
        const std::string output_type =
            (*item)["output_source_type"].toString();
        // output_source.media_type AS output_media_type,
        const std::string output_media_type =
            (*item)["output_media_type"].toString();

        SourceInfoPtr output =
            m_source_factory.getSource(output_uuid, output_name, output_media_type,
                                       output_type);

        // SELECT klk_app_transcode_task.task AS task_uuid,
        const std::string task_uuid = (*item)["task_uuid"].toString();
        // klk_app_transcode_task.task_name AS task_name,
        const std::string task_name = (*item)["task_name"].toString();


        // klk_app_transcode_video_size.size_uuid AS vsize_uuid,
        const std::string vsize =   (*item)["vsize_uuid"].toString();
        //klk_app_transcode_video_quality.quality_uuid AS vquality_uuid,
        const std::string vquality =   (*item)["vquality_uuid"].toString();
        quality::VideoPtr video_params(new quality::Video(vsize, vquality));

        // klk_app_transcode_task.schedule_start AS schedule_start,
        const std::string schedule_start =
            (*item)["schedule_start"].toString();
        // klk_app_transcode_task.schedule_duration AS schedule_duration
        const time_t schedule_duration =
            static_cast<time_t>((*item)["schedule_duration"].toInt());
        TaskInfoPtr task(new TaskInfo(task_uuid, task_name,
                                       input, output,
                                       video_params,
                                       schedule_start, schedule_duration));
        set.insert(task);
    }

    return set;
}

// Register all processors
void Transcode::registerProcessors()
{
    launcher::Module::registerProcessors();
    registerCLI(cli::ICommandPtr(new SourceAddCommand()));
    registerCLI(cli::ICommandPtr(new SourceDelCommand()));
    registerCLI(cli::ICommandPtr(new SourceShowCommand()));
    registerCLI(cli::ICommandPtr(new TaskAddCommand()));
    registerCLI(cli::ICommandPtr(new TaskDelCommand()));
    registerCLI(cli::ICommandPtr(new TaskShowCommand()));

    // schedule playback functor
    registerTimer(boost::bind(&Transcode::doSchedulePlayback, this),
                  SCHEDULE_INTERVAL);

#ifdef LINUX
    // processing events about IEEE1394 devices changes
    registerASync(
        msg::id::IEEE1394DEV,
        boost::bind(&Transcode::processIEEE1394, this, _1));
#endif //LINUX

    registerSNMP(boost::bind(&Transcode::processSNMP, this, _1), MODID);

    registerDBUpdateMessage(UPDATEDB_MESSAGE);
}

// Process changes at the DB
// @param[in] msg - the input message
void Transcode::processDB(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg);

    klk_log(KLKLOG_DEBUG, "Processing DB change event for module '%s'",
            getName().c_str());

    InfoSet db = getTaskInfoFromDB();
    InfoSet del = m_info.getDel(db);
    InfoSet add = m_info.getAdd(db);

    // stop that should be deleted
    std::for_each(del.begin(), del.end(),
                  boost::bind(&Transcode::deinitInfo,
                              this, _1));

    // delete all elements from the del list
    m_info.del(del);
    // add new elements
    // and update existent
    m_info.add(db, true);
}

// Does the task initialization
void Transcode::initTask(const TaskInfoPtr& taskinfo) throw()
{
    try
    {
        // do the task initialization
        TaskPtr task(new Task(getFactory(), this, taskinfo));
        // start the task
        m_scheduler.addTask(task);
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Exception during transcode task initialization. "
                "Task name: %s."
                "Error description: %s",
                taskinfo->getName().c_str(), err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Exception during transcode task initialization. "
                "Task name: %s.",
                taskinfo->getName().c_str());
    }
}


// Does the task de-initialization
// Free resources allocated with the task
void Transcode::deinitInfo(const mod::InfoPtr& info) throw()
{
    try
    {
        m_scheduler.delTask(mod::Info::dynamicPointerCast<TaskInfo>(info));
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Exception during transcode task deinitialization. "
                "Task name: %s."
                "Error description: %s",
                info->getName().c_str(), err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Exception during transcode task deinitialization. "
                "Task name: %s.",
                info->getName().c_str());
    }
}

// Do some actions before main loop
void Transcode::preMainLoop()
{
    // Inits scheduler
    // starts processor thread
    m_scheduler.init();
    launcher::Module::preMainLoop();
}


// Do some actions after main loop
void Transcode::postMainLoop() throw()
{
    // stop the scheduler
    m_scheduler.stop();

    // base actions that should be done on exit
    launcher::Module::postMainLoop();
}

// Checks schedule playback info. It stops task with
// duration interval exceed and also start a task
// that should be started accordingly with the schedule
void Transcode::doSchedulePlayback()
{
    // first of all we should stop tasks
    TaskInfoList stop_list = getStopList();

    // stop affected pipelines
    m_scheduler.pausePipelines(stop_list);
    std::for_each(stop_list.begin(), stop_list.end(),
                  boost::bind(&Transcode::deinitInfo,
                              this, _1));
    // start processing affected pipelines
    m_scheduler.playPipelines(stop_list);

    TaskInfoList start_list = getStartList();

    // stop affected pipelines
    m_scheduler.pausePipelines(start_list);

    // tasks container updates
    // it will also start all new threads
    // and stops threads that currently not active
    std::for_each(start_list.begin(), start_list.end(),
                  boost::bind(&Transcode::initTask,
                              this, _1));

    // start processing affected pipelines
    m_scheduler.playPipelines(start_list);
}

// Retrives a list of tasks that should be stopped
// accordingly with scheduled playback settings
const TaskInfoList Transcode::getStopList() const
{
    TaskInfoList full = m_info.getInfoList();
    TaskInfoList result;
    for (TaskInfoList::iterator i = full.begin(); i != full.end(); i++)
    {
        TaskInfoPtr task = *i;
        if (task->isExperied())
        {
            result.push_back(task);
        }
    }

    return result;
}

// Retrives a list of tasks that should be started
// accordingly with scheduled playback settings
const TaskInfoList Transcode::getStartList() const
{
    // original list
    TaskInfoList full = m_info.getInfoList();
    TaskInfoList result;
    for (TaskInfoList::iterator task = full.begin(); task != full.end(); task++)
    {
        if ((*task)->needStart())
            result.push_back(*task);
    }

    return result;
}

// Processes SNMP request
const snmp::IDataPtr Transcode::processSNMP(const snmp::IDataPtr& req)
{
    BOOST_ASSERT(req);
    snmp::ScalarPtr reqreal =
        boost::dynamic_pointer_cast<snmp::Scalar, snmp::IData>(req);
    BOOST_ASSERT(reqreal);

    const std::string reqstr = reqreal->getValue().toString();
    // support only snmp::GETSTATUSTABLE
    if (reqstr != snmp::GETSTATUSTABLE)
    {
        throw Exception(__FILE__, __LINE__,
                        "Unknown SNMP request: " + reqstr);

    }

    // create the response
    // table with data
    snmp::TablePtr table(new snmp::Table());
    TaskInfoList tasks = m_info.getInfoList();
    u_int count = 0;
    for (TaskInfoList::iterator i = tasks.begin();
         i != tasks.end();
         i++, count++)
    {
        // klkIndex                    Counter32,
        // klkTaskName           DisplayString,
        // klkInputSource        DisplayString,
        // klkOutputSource     DisplayString,
        // klkDuration              Unsigned64,
        // klkRunningCount    Counter32,
        // klkTotalDuration              Unsigned64
        snmp::TableRow row;
        row.push_back(count);
        const std::string task_name = (*i)->getName();
        row.push_back(task_name);
        try
        {
            const std::string source_name = (*i)->getSource()->getName();
            row.push_back(source_name);
            const std::string dest_name = (*i)->getDestination()->getName();
            row.push_back(dest_name);

            u_int64_t duration =
                static_cast<u_int64_t>((*i)->getActualDuration());
            /*
              klk_log(KLKLOG_DEBUG, "Task '%s' duration: %llu",
              task_name.c_str(),
              duration);
            */
            row.push_back(duration);
            row.push_back((*i)->getRunningCount());

            u_int64_t total_duration =
                static_cast<u_int64_t>((*i)->getRunningTime());
            row.push_back(total_duration);
        }
        catch(const std::exception&)
        {
            row.clear();
            row.push_back(count);
            row.push_back(task_name);
            row.push_back("n/a");
            row.push_back("n/a");
            row.push_back(0ULL);
            row.push_back(0);
            row.push_back(0ULL);
        }
        table->addRow(row);
    }

    return table;
}

#ifdef LINUX
// Processes info about IEEE1394 changes
void Transcode::processIEEE1394(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg);
    // FIXME!!! add the processing for events
    // msg::key::MODINFOUUID -> dev's uuid
    // msg::key::IEEE1394STATE -> msg::key::IEEE1394STATENEW or
    //                                                      msg::key::IEEE1394STATEDEL
    // msg::key::IEEE1394PORT -> dev's port
}
#endif //LINUX
