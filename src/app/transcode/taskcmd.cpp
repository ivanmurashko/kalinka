/**
   @file taskcmd.cpp
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
   - 2009/11/15 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>
#include <boost/lexical_cast.hpp>

#include "taskcmd.h"
#include "exception.h"
#include "defines.h"
#include "db.h"
#include "clitable.h"
#include "scheduleinfo.h"

using namespace klk;
using namespace klk::trans;

//
// TaskAddCommand class
//

/**
   Task add command name
*/
const std::string TASKADD_COMMAND_NAME = "task add";

const std::string TASKADD_COMMAND_SUMMARY =
    "Adds a task info";
const std::string TASKADD_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + TASKADD_COMMAND_NAME +
    " [task name] [input source] [output source]"
    " [video size] [video quality]"
    " [schedule info] [schedule duration]\n";


//  Constructor
TaskAddCommand::TaskAddCommand() :
    BaseCommand(TASKADD_COMMAND_NAME,
                TASKADD_COMMAND_SUMMARY, TASKADD_COMMAND_USAGE)
{
}

// Destructor
TaskAddCommand::~TaskAddCommand()
{
}

// Process the command
const std::string TaskAddCommand::process(const cli::ParameterVector& params)
{
    if (params.size() != 7 && params.size() != 11)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters (%d). %s", params.size(),
                        TASKADD_COMMAND_USAGE.c_str());
    }

    const std::string name = params[0];
    const std::string input = params[1];
    const std::string output = params[2];
    const std::string vsize = params[3];
    const std::string vquality = params[4];
    std::string schedule = params[5];
    if (params.size() == 11)
    {
        for (u_int i = 6; i < 10; i++)
        {
            schedule += " " + params[i];
        }
    }

    // task name
    StringList names = getTaskNameList();
    if (std::find(names.begin(), names.end(), name) != names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "The task name has been already assigned: " + name);
    }
    // input/output source name
    names = getSourceNameList();

    // we should have at least two sources to assign a task
    if (names.size() < 2)
    {
        throw Exception(__FILE__, __LINE__,
                        "Add more sources before assign a task");
    }

    if (std::find(names.begin(), names.end(), input) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect input source name: " + input);
    }
    if (std::find(names.begin(), names.end(), output) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect output source name: " + output);
    }
    // video size
    names = getVideoSizeList();
    if (std::find(names.begin(), names.end(), vsize) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect video size: " + vsize);
    }
    // video quality
    names = getVideoQualityList();
    if (std::find(names.begin(), names.end(), vquality) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect video quality: " + vquality);
    }

    // schedule
    // will produce an exception if wrong param
    CrontabParser parser(schedule);

    // duration
    // last parameter is the duration
    u_int duration = 0;
    try
    {
        duration = boost::lexical_cast<u_int>(*params.rbegin());
    }
    catch(...)
    {
        throw Exception(__FILE__, __LINE__,
                        "Invalid duration: %s", params.rbegin()->c_str());
    }

    // add the task
    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    // IN task_name VARCHAR(255),
    // IN input_name VARCHAR(255),
    // IN output_name VARCHAR(255),
    // IN size_name VARCHAR(255),
    // IN quality_name VARCHAR(255),
    // IN schedule_start VARCHAR(128);
    // IN schedule_duration INT;
    // OUT return_value INT
    dbparams.add("@task_name", name);
    dbparams.add("@input_name", input);
    dbparams.add("@output_name", output);
    dbparams.add("@size_name", vsize);
    dbparams.add("@quality_name", vquality);
    dbparams.add("@schedule_start", schedule);
    dbparams.add("@schedule_duration", duration);
    dbparams.add("@return_value");

    db::Result res = db.callSimple("klk_app_transcode_task_add_cli", dbparams);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__, "Failed to add the task");
    }

    return "Task '" + name + "' has been added\n";
}

// gets completion
const cli::ParameterVector
TaskAddCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;
    if (setparams.empty())
    {
        const StringList names = getTaskNameList();
        // our name sould not be in the list
        // FIXME!!! consider case when we have more than 1000 tasks
        // with the template name
        for (int i = 0; i < 1000; i++)
        {
            std::stringstream data;
            data << "task" << i;
            if (std::find(names.begin(), names.end(), data.str()) == names.end())
            {
                res.push_back(data.str());
                break;
            }
        }
    }
    else if (setparams.size() == 1)
    {
        const StringList names = getSourceNameList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    else if (setparams.size() == 2)
    {
        StringList names = getSourceNameList();
        // remove the input name from the list
        StringList::iterator input =
            std::find(names.begin(), names.end(), setparams[1]);
        BOOST_ASSERT(input != names.end());
        names.erase(input);
        // copy all others
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    else if (setparams.size() == 3)
    {
        const StringList names = getVideoSizeList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    else if (setparams.size() == 4)
    {
        const StringList names = getVideoQualityList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    else if (setparams.size() == 5)
    {
        res.push_back(schedule::ALWAYS);
        res.push_back(schedule::REBOOT);
        res.push_back(schedule::HOURLY);
        res.push_back(schedule::DAILY);
        res.push_back(schedule::MONTHLY);
        res.push_back(schedule::YEARLY);
        // minutes
        res.push_back("*");
        for (int i = 0; i < 60; i++)
        {
            std::stringstream data;
            data << i;
            res.push_back(data.str());
        }
    }
    else if (setparams.size() == 6)
    {
        if (*(setparams[4].begin()) == '@')
        {
            // duration here
            res.push_back("0");
            res.push_back("60");
            res.push_back("3600");
        }
        else
        {
            // hours
            res.push_back("*");
            for (int i = 0; i < 23; i++)
            {
                std::stringstream data;
                data << i;
                res.push_back(data.str());
            }
        }
    }
    else if (setparams.size() == 7)
    {
        // day of month
        res.push_back("*");
        for (int i = 1; i <= 31; i++)
        {
            std::stringstream data;
            data << i;
            res.push_back(data.str());
        }
    }
    else if (setparams.size() == 8)
    {
        // month
        res.push_back("*");
        for (int i = 1; i <= 12; i++)
        {
            std::stringstream data;
            data << i;
            res.push_back(data.str());
        }
    }
    else if (setparams.size() == 9)
    {
        // day of week
        res.push_back("*");
        for (int i = 0; i <= 7; i++)
        {
            std::stringstream data;
            data << i;
            res.push_back(data.str());
        }
    }
    else if (setparams.size() == 10)
    {
        // duration here
        res.push_back("0");
        res.push_back("60");
        res.push_back("3600");
    }
    return res;
}

// Retrives a list with possible video sizes names
const StringList
TaskAddCommand::getVideoSizeList()
{
    StringList list;

    // klk_app_transcode_video_size_list
    //SELECT klk_app_transcode_video_size.size_name AS name,

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    db::ResultVector rv = db.callSelect("klk_app_transcode_video_size_list",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["name"].toString());
    }

    return list;
}

// Retrives a list with possible video qualities names
const StringList
TaskAddCommand::getVideoQualityList()
{
    StringList list;

    // klk_app_transcode_video_quality_list
    //SELECT klk_app_transcode_video_quality.quality_name AS name,

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    db::ResultVector rv = db.callSelect("klk_app_transcode_video_quality_list",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["name"].toString());
    }

    return list;
}

//
// TaskDelCommand class
//

/**
   Task del command name
*/
const std::string TASKDEL_COMMAND_NAME = "task delete";

const std::string TASKDEL_COMMAND_SUMMARY =
    "Deletes a task info";
const std::string TASKDEL_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + TASKDEL_COMMAND_NAME + " [task name]\n";


//  Constructor
TaskDelCommand::TaskDelCommand() :
    BaseCommand(TASKDEL_COMMAND_NAME,
                 TASKDEL_COMMAND_SUMMARY, TASKDEL_COMMAND_USAGE)
{
}


// Destructor
TaskDelCommand::~TaskDelCommand()
{
}

// Process the command
const std::string TaskDelCommand::process(const cli::ParameterVector& params)
{
    if (params.size() != 1)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        TASKDEL_COMMAND_USAGE);
    }

    const std::string task_name = params[0];

    // check task type name
    StringList names = getTaskNameList();
    if (std::find(names.begin(), names.end(), task_name) == names.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect task name: " + task_name);
    }

    // del the task
    delTask(task_name);

    return "Task '" + task_name + "' has been deleted\n";
}

// gets completion
const cli::ParameterVector
TaskDelCommand::getCompletion(const cli::ParameterVector& setparams)
{
    cli::ParameterVector res;
    if (setparams.empty())
    {
        const StringList names = getTaskNameList();
        std::copy(names.begin(), names.end(),
                  std::back_inserter(res));
    }
    return res;
}


// Deletes a task with specified parameters
void TaskDelCommand::delTask(const std::string& name)
{
    db::DB db(getFactory());
    db.connect();

    db::Parameters params;

    // IN task_name VARCHAR(255),
    // OUT return_value INT
    params.add("@name", name);
    params.add("@return_value");

    db::Result res = db.callSimple("klk_app_transcode_task_del_cli", params);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__, "Failed to delete the task");
    }
}


//
// TaskShowCommand class
//

/**
   Task show command name
*/
const std::string TASKSHOW_COMMAND_NAME = "task show";

const std::string TASKSHOW_COMMAND_SUMMARY =
    "Shows a task info list";
const std::string TASKSHOW_COMMAND_USAGE = "Usage: " + MODNAME +
    + " " + TASKSHOW_COMMAND_NAME + "\n";


//  Constructor
TaskShowCommand::TaskShowCommand() :
    cli::Command(TASKSHOW_COMMAND_NAME,
                 TASKSHOW_COMMAND_SUMMARY, TASKSHOW_COMMAND_USAGE)
{
}


// Destructor
TaskShowCommand::~TaskShowCommand()
{
}

// Process the command
const std::string TaskShowCommand::process(const cli::ParameterVector& params)
{
    if (!params.empty())
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect parameters. " +
                        TASKSHOW_COMMAND_USAGE);
    }

    cli::Table table;

    StringList head;
    head.push_back("name");
    head.push_back("input");
    head.push_back("output");
    head.push_back("vsize");
    head.push_back("vquality");
    head.push_back("schedule");
    head.push_back("duration");
    table.addRow(head);

    db::DB db(getFactory());
    db.connect();
    db::Parameters dbparams;

    db::ResultVector rv = db.callSelect("klk_app_transcode_get",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        StringList row;
        row.push_back((*item)["task_name"].toString());
        row.push_back((*item)["input_name"].toString());
        row.push_back((*item)["output_name"].toString());
        row.push_back((*item)["vsize_name"].toString());
        row.push_back((*item)["vquality_name"].toString());
        row.push_back((*item)["schedule_start"].toString());
        row.push_back((*item)["schedule_duration"].toString());
        table.addRow(row);
    }

    return table.formatOutput();
}

// gets completion
const cli::ParameterVector
TaskShowCommand::getCompletion(const cli::ParameterVector& setparams)
{
    return cli::ParameterVector();
}
