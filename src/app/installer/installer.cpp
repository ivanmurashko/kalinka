/**
   @file installer.cpp
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
   - 2009/03/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>

#include <boost/bind.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "installer.h"
#include "exception.h"
#include "db.h"
#include "utils.h"

using namespace klk;
using namespace klk::inst;

//
// Installer class
//

// Constructor
Installer::Installer(IFactory* factory) : m_factory(factory)
{
    BOOST_ASSERT(factory);
}

/**
   Destructor
*/
Installer::~Installer()
{
}

// Processes interactive option
void Installer::processInteractive()
{
    // process all options
    IConfig* config = m_factory->getConfig();
    // before set any new config file option
    // reread config file
    config->load();

    // look for the option
    OptionList options = config->getOptions();
    for (OptionList::iterator i = options.begin();
         i != options.end(); i++)
    {
        const std::string name = (*i)->getName();
        const std::string oldval = (*i)->getValue();

        std::cout << "Option '" << name << "' has the following value: "
                  << oldval << std::endl;

        if (ask("Do you want to change it [N/y]: "))
        {
            std::string value = get("New value: ");
            setOption((*i)->getName(), value);
        }
    }

    if (ask("Do you want to recreate DB [N/y]: "))
    {
        recreateDB();
    }

    // resources detection
    if (ask("Do you want to detect resources [N/y]: "))
    {
        detectResources();
    }
}


// Sets an option
void Installer::setOption(const std::string& name,
                          const std::string& value)
{
    BOOST_ASSERT(name.empty() == false);
    BOOST_ASSERT(value.empty() == false);

    // before set any new config file option
    // reread config file
    m_factory->getConfig()->load();

    // look for the option
    OptionList options = m_factory->getConfig()->getOptions();
    OptionList::iterator i = std::find_if(
        options.begin(), options.end(),
        boost::bind(
            std::equal_to<std::string>(),
            name,
            boost::bind(&IOption::getName, _1)));
    if (i == options.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Cannot find option with name: " + name);
    }

    (*i)->setValue(value);

    // save result
    m_factory->getConfig()->save();

}

// Detects resources
void Installer::detectResources()
{
    ModuleList modules =
        m_factory->getModuleFactory()->getModules();

    try
    {
        m_factory->getResources()->initDevs();
        IModuleFactory *modfactory = m_factory->getModuleFactory();

        for (ModuleList::iterator mod = modules.begin();
             mod != modules.end(); mod++)
        {
            IResourcesPtr res = modfactory->getResources((*mod)->getID());
            if (res)
            {
                res->initDevs();
                res->updateDB(res->getType());
            }
        }
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Installer error: %s", err.what());
        throw;
    }
}

// Recreates DB
void Installer::recreateDB()
{
    base::Utils::runScript(base::Utils::PERL,
                           "usr/share/kalinka/sql/klkdbcreate.pl", "-c");
}

// Asks a question
bool Installer::ask(const std::string& prompt)
{
    BOOST_ASSERT(prompt.empty() == false);
    std::cout << prompt;
    char buff[128];
    std::cin.getline(buff, sizeof(buff), '\n');
    // remove \n
    std::string result(buff);
    boost::trim_if(result,  boost::is_any_of(" \r\n\t"));
    // convert to lower
    if (base::Utils::toLower(result) == "y" ||
        base::Utils::toLower(result) == "yes")
    {
        return true;
    }

    return false;
}

// Gets a value
const std::string Installer::get(const std::string& prompt)
{
    BOOST_ASSERT(prompt.empty() == false);
    std::cout << prompt;
    char buff[128];
    std::cin.getline(buff, sizeof(buff), '\n');
    // remove \n
    std::string result(buff);
    boost::trim_if(result,  boost::is_any_of(" \r\n\t"));
    return result;
}
