/**
   @file cliapp.cpp
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
   - 2009/02/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cliapp.h"
#include "log.h"
#include "db.h"
#include "exception.h"
#include "utils.h"

using namespace klk;
using namespace klk::cli;

//
// ApplicationBase class
//

// Constructor
ApplicationBase::ApplicationBase(const std::string& msgid) :
    m_factory(NULL), m_msgid(msgid), m_modid()
{
    BOOST_ASSERT(m_msgid.empty() == false);
}

//Sets factory object
void ApplicationBase::setFactory(IFactory* factory)
{
    BOOST_ASSERT(factory);
    m_factory = factory;
}

// Sets module ID
void ApplicationBase::setModuleID(const std::string& id)
{
    m_modid = id;
}

// Retrives module name
const std::string ApplicationBase::getModuleName() const
{
    BOOST_ASSERT(m_factory);
    BOOST_ASSERT(m_modid.empty() == false);
    const IModulePtr
        module = m_factory->getModuleFactory()->getModule(m_modid);
    return module->getName();
}

// Retrives module name
const std::string ApplicationBase::getModuleID() const
{
    BOOST_ASSERT(m_modid.empty() == false);
    return m_modid;
}


// Retrives factory
IFactory* ApplicationBase::getFactory()
{
    BOOST_ASSERT(m_factory);
    return m_factory;
}


//
// AutostartSet class
//

// the command name
const std::string AUTOSTARTSET_NAME = "autostart set";

// the command name
const std::string AUTOSTARTSET_SUMMARY =
    "Sets the application autostart feature";

// Constructor
AutostartSet::AutostartSet(const std::string& msgid) :
    ApplicationBase(msgid)
{
}

// Gets the command name
const std::string AutostartSet::getName() const
{
    return AUTOSTARTSET_NAME;
}

// Gets the command description
const std::string AutostartSet::getSummary() const
{
    return AUTOSTARTSET_SUMMARY;
}

// Gets the command usage description
const std::string AutostartSet::getUsage() const
{
    std::string result = "Usage: ";
    result += getModuleName();
    result += " " + AUTOSTARTSET_NAME;
    result += " [" + AUTOSTARTSET_ON +"|" +
        AUTOSTARTSET_OFF + "]";
    return result;
}

// Process the command
const std::string AutostartSet::process(const ParameterVector& params)
{
    BOOST_ASSERT(params.size() == 1);

    int enabled = 0;
    if (params[0] == AUTOSTARTSET_ON)
    {
        enabled = 1;
    }

    db::DB db(getFactory());
    db.connect();

    // default values
    std::string application = base::Utils::generateUUID();
    const std::string module = getModuleID();
    std::string name = getModuleName() +
        "@" + getFactory()->getConfig()->getHostName();
    int status = 0;
    int priority = 0;
    const std::string host = db.getHostUUID();
    std::string description = "The application '" + name +
        "' for host '" + host + "'";
    bool was = false;

    db::Parameters dbparams;
    dbparams.add("@host");
    db::ResultVector rv =
        db.callSelect("klk_application_list", dbparams, NULL);
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        //SELECT application, module, name, enabled, status, priority,
        // host, description
        if ((*i)["module"].toString() == module &&
            (*i)["host"].toString() == host)
        {
            name = (*i)["name"].toString();
            application = (*i)["application"].toString();
            status = (*i)["status"].toInt();
            priority = (*i)["priority"].toInt();
            description = (*i)["description"].toInt();
            was = true;
            break;
        }
    }

    //IN application VARCHAR(40),
    //IN module VARCHAR(40),
    //IN name VARCHAR(50),
    //IN enabled TINYINT,
    //IN status TINYINT,
    //IN priority INTEGER,
    //IN host VARCHAR(40),
    //IN description VARCHAR(255),
    //OUT return_value INTEGER
    dbparams.clear();
    dbparams.add("@application", application);
    dbparams.add("@module", module);
    dbparams.add("@name", name);
    dbparams.add("@enabled", enabled);
    dbparams.add("@status", status);
    dbparams.add("@priority", priority);
    dbparams.add("@host", host);
    dbparams.add("@description", priority);
    dbparams.add("@return_value");
    if (was)
    {
        db::Result res =
            db.callSimple("klk_application_update", dbparams);
        BOOST_ASSERT(res["@return_value"].toInt() == 0);
    }
    else
    {
        db::Result res =
            db.callSimple("klk_application_add", dbparams);
        BOOST_ASSERT(res["@return_value"].toInt() == 0);
    }

    if (enabled)
    {
        return "The application autostart property was set into 'on'";
    }
    return "The application autostart property was set into 'off'";
}

// Retrives list of possible completions for a n's parameter
const ParameterVector
AutostartSet::getCompletion(const ParameterVector& setparams)
{
    ParameterVector params;
    if (setparams.empty())
    {
        params.push_back(AUTOSTARTSET_ON);
        params.push_back(AUTOSTARTSET_OFF);
    }
    return params;
}


//
// AutostartShow class
//

// the command name
const std::string AUTOSTARTSHOW_NAME = "autostart show";

// the command name
const std::string AUTOSTARTSHOW_SUMMARY =
    "Shows the application autostart feature";


// Constructor
AutostartShow::AutostartShow(const std::string& msgid) :
    ApplicationBase(msgid)
{
}

// Gets the command name
const std::string AutostartShow::getName() const
{
    return AUTOSTARTSHOW_NAME;
}

// Gets the command description
const std::string AutostartShow::getSummary() const
{
    return AUTOSTARTSHOW_SUMMARY;
}

// Gets the command usage description
const std::string AutostartShow::getUsage() const
{
    std::string result = "Usage: ";
    result += getModuleName();
    result += " " + AUTOSTARTSHOW_NAME;
    return result;
}

// Process the command
const std::string AutostartShow::process(const ParameterVector& params)
{
    const std::string modid = getModuleID();
    db::Parameters params_select;
    db::DB db(getFactory());
    db.connect();
    params_select.add("@host", db.getHostUUID());
    db::ResultVector rv =
        db.callSelect("klk_application_list", params_select, NULL);
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT
        // application, module,
        // name, status, prioriry, description
        if ((*i)["module"].toString() == modid)
        {
            return "The application state is 'on'";
        }
    }

    return "The application state is 'off'";
}

// Retrives list of possible completions for a n's parameter
const ParameterVector
AutostartShow::getCompletion(const ParameterVector& setparams)
{
    // no parameters
    return ParameterVector();
}
