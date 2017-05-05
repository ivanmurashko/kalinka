/**
   @file dvbresources.cpp
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
   - 2009/01/24 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <linux/dvb/frontend.h>
#include <sys/ioctl.h>


#include <boost/lexical_cast.hpp>

#include "ifactory.h"
#include "dvbresources.h"
#include "log.h"
#include "dvbdev.h"
#include "utils.h"
#include "exception.h"
#include "defines.h"

using namespace klk;
using namespace klk::dvb;

/**
   Resources factory function
*/
IResourcesPtr klk_resources_get(IFactory *factory)
{
    return IResourcesPtr(new Resources(factory));
}

//
// Resources class
//

// Constructor
Resources::Resources(IFactory* factory) :
    dev::BaseResources(factory, dev::DVB_ALL)
{
}

Resources::~Resources()
{
}

// Inits DVB resources
void Resources::initDevs()
{
    klk_log(KLKLOG_DEBUG, "DVB resource initialization");
    const u_int MAXDEV_NUMBER = 16;
    if (base::Utils::fileExist("/dev/dvb"))
    {
        // DVB devices
        for (u_int adapter = 0; adapter < MAXDEV_NUMBER; adapter++)
        {
            const std::string path = "/dev/dvb/adapter" +
                boost::lexical_cast<std::string>(adapter);
            if (base::Utils::fileExist(path) != 1)
                break; // adapter doesn't exist
            for (u_int frontend = 0; frontend < MAXDEV_NUMBER; frontend++)
            {
                const std::string name = path + "/frontend" +
                    boost::lexical_cast<std::string>(frontend);
                if (base::Utils::fileExist(name) != 1)
                    break; // frontend doesn't exist
                try
                {
                    addDevInfo(adapter, frontend);
                }
                catch(const std::exception& err)
                {
                    klk_log(KLKLOG_ERROR,
                            "Incorrect (or in use) DVB device '%s'. Error: %s",
                            name.c_str(), err.what());
                }
                catch(...)
                {
                    klk_log(KLKLOG_ERROR,
                            "Unspecified error while DVB device "
                            "detection: %s", name.c_str());
                }
            }
        }
    }
    else
    {
        klk_log(KLKLOG_INFO,
                "The mediaserver does not have any DVB devices");
    }
}

// Adds DVB device info
void Resources::addDevInfo(u_int adapter, u_int frontend)
{
    const std::string name = "/dev/dvb/adapter" +
        boost::lexical_cast<std::string>(adapter) +
        "/frontend" + boost::lexical_cast<std::string>(frontend);
    checkDev(name);

    int fd = open(name.c_str(), O_RDWR|O_NONBLOCK);
    if (fd < 0)
    {
        klk_log(KLKLOG_ERROR, "Error %d in open(): %s",
                errno, strerror(errno));
        throw Exception(__FILE__, __LINE__, "Error in open()");
    }

    try
    {
        struct dvb_frontend_info fe_info;
        int status = 0;
        if ((status = ioctl (fd, FE_GET_INFO, &fe_info) >= 0))
        {
            std::string type = "";
            switch (fe_info.type)
            {
                // DVB-T
            case FE_OFDM:
                type = dev::DVB_T;
                break;
                // DVB-S
            case FE_QPSK:
                type = dev::DVB_S;
                break;
                // DVB-C
            case FE_QAM:
                type = dev::DVB_C;
                break;
            default:
                throw Exception(__FILE__, __LINE__,
                    "Unknown DVB type for " + name);
                break;
            }
            if (!type.empty())
            {
                IDevPtr dev(new dev::DVB(m_factory, type));
                dev->setParam(
                    dev::ADAPTER,
                    boost::lexical_cast<std::string>(adapter));
                dev->setParam(
                    dev::FRONTEND,
                    boost::lexical_cast<std::string>(frontend));
                // create a default name
                // the name should be unique for whole project
                std::string name = "dvb";
                name += dev->getStringParam(dev::ADAPTER) +
                    dev->getStringParam(dev::FRONTEND);
                name += "@" + m_factory->getConfig()->getHostName();
                dev->setParam(dev::NAME, name);
                addDev(dev);
            }

        }
        else
        {
            klk_log(KLKLOG_ERROR, "Error %d in ioctl() for FE_GET_INFO: %s",
                    errno, strerror(errno));
            throw Exception(__FILE__, __LINE__,
                                 "Error in ioctl()");
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_DEBUG, "Got an exception in Resources::addDevInfo()");
        klk_close(fd);
        throw;
    }

    klk_close(fd);
}

void Resources::checkDev(const std::string& dev)
{
    BOOST_ASSERT(base::Utils::fileExist(dev) == 1);

    // getting info
    struct stat st;
    if (lstat(dev.c_str(), &st) < 0)
    {
        klk_log(KLKLOG_ERROR, "Error %d in lstat(): %s",
                errno, strerror(errno));
        throw Exception(__FILE__, __LINE__,
                             "Error in lstat()");
    }

    if (!S_ISCHR(st.st_mode))
    {
        throw Exception(__FILE__, __LINE__,
                             "The DVB dev file '" + dev + "' is "
                             "not a character device");
    }
}
