/**
   @file netresources.cpp
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
   - 2009/03/03 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#include "netresources.h"
#include "netdev.h"
#include "log.h"
#include "exception.h"
#include "defines.h"

using namespace klk;
using namespace klk::net;

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
Resources::Resources(IFactory* factory) : BaseResources(factory, dev::NET)
{
}

// Destructor
Resources::~Resources()
{
}

// Inits network resources
void Resources::initDevs()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        throw Exception(__FILE__, __LINE__, "Error %d in socket(): %s",
                             errno, strerror(errno));
    }

    // initial buffer size guess
    size_t len = 100 * sizeof(struct ifreq);
    std::vector<char> tmpvec(len);
    struct ifconf ifc;
    try
    {
        //FIXME!!! the loop
        int lastlen = 0;
        for(;;)
        {
            char* buf = &tmpvec[0];
            ifc.ifc_buf = buf;
            ifc.ifc_len = len;
            if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
            {
                if (errno != EINVAL || lastlen != 0)
                {
                    throw Exception(__FILE__, __LINE__,
                                         "Error %d in ioctl(): %s",
                                         errno, strerror(errno));
                }
            }
            else
            {
                if (ifc.ifc_len == lastlen)
                    break;  /* success, len has not changed */
                lastlen = ifc.ifc_len;
            }
            len += 10 * sizeof(struct ifreq);  /* increment */
            tmpvec.resize(len);
        }
    }
    catch(...)
    {
        close(sock);
        throw;
    }
    close(sock);

    u_char* cur = reinterpret_cast<u_char*>(ifc.ifc_req);
    const u_char* end = reinterpret_cast<u_char*>(&ifc.ifc_buf[ifc.ifc_len]);

    while (cur < end)
    {
        const struct ifreq *ifr =
            reinterpret_cast<const struct ifreq*>(cur);
        addDevInfo(ifr);
#ifdef DARWIN
        cur += sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len;
#else
        cur += sizeof(struct ifreq);
#endif
    }
}

// Adds network device info
void Resources::addDevInfo(const struct ifreq *ifr)
{
    BOOST_ASSERT(ifr);
    // getting name
    std::string name = ifr->ifr_name;
    std::string addr = getAddress(ifr);
    if (addr.empty())
    {
        klk_log(KLKLOG_DEBUG, "Network resource '%s' was ignored. "
                "Address retriving failed",
                name.c_str());
        return; // nothing to do for the iface
    }

    // getting flags
    short flags = getFlags(ifr);
    if ((flags & IFF_UP) == 0)
    {
        klk_log(KLKLOG_DEBUG, "Network resource '%s' was ignored. "
                "Resource is inactive",
                name.c_str());
        return; // nothing to do for the iface
    }

    // getting additional info
    std::string netmask = getMask(ifr);
    if (netmask.empty())
    {
        klk_log(KLKLOG_DEBUG, "Network resource '%s' was ignored. "
                "Network mask retriving failed",
                name.c_str());
        return; // nothing to do for the iface
    }

    IDevPtr dev(new dev::Net(m_factory));
    // make net name more unique
    name += "@" + m_factory->getConfig()->getHostName();
    dev->setParam(dev::NAME, name);
    dev->setParam(dev::NETADDR, addr);
    dev->setParam(dev::NETMASK, netmask);
    if ((flags & IFF_MULTICAST) != 0)
    {
        dev->setParam(dev::NETMCAST, "");
    }
    if ((flags & IFF_LOOPBACK) != 0)
    {
        dev->setParam(dev::NETLOOP, "");
    }
    addDev(dev);

    klk_log(KLKLOG_DEBUG, "Detected '%s' network device. "
            "Network address: %s. Network mask: %s",
            name.c_str(), addr.c_str(), netmask.c_str());
}


// Gets network address for the found iface
const std::string Resources::getAddress(const struct ifreq *ifr) const
{
    BOOST_ASSERT(ifr);
    std::string addr;
    const struct sockaddr *sa = &ifr->ifr_addr;
    BOOST_ASSERT(sa);
    if (sa->sa_family == AF_INET)
    {
        const struct sockaddr_in*
            sin = reinterpret_cast<const struct sockaddr_in*>(sa);
        char addrstr[64];
        const void *addrp = static_cast<const void*>(&sin->sin_addr);
        inet_ntop(sa->sa_family, addrp, addrstr, sizeof(addrstr));
        addr = addrstr;
    }

    return addr;
}

// Gets network mask for the found iface
const std::string Resources::getMask(const struct ifreq *ifr) const
{
    BOOST_ASSERT(ifr);
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in socket(): %s",
                             errno, strerror(errno));
    }

    // getting additional info
    std::string netmask;
    try
    {
        //get netmask
        struct ifreq ifrnm;
        strncpy(ifrnm.ifr_name, ifr->ifr_name, sizeof(ifrnm.ifr_name));
        if(ioctl(sock, SIOCGIFNETMASK, reinterpret_cast<char*>(&ifrnm)) == -1)
        {
            throw Exception(__FILE__, __LINE__,
                                 "Error %d in ioctl(): %s",
                                 errno, strerror(errno));
        };
        struct sockaddr *sa = &ifrnm.ifr_addr;
        struct sockaddr_in* sin = reinterpret_cast<struct sockaddr_in*>(sa);
        char addrstr[64];
        void *addrp = static_cast<void*>(&sin->sin_addr);
        inet_ntop(sa->sa_family, addrp, addrstr, sizeof(addrstr));
        netmask = addrstr;
    }
    catch(...)
    {
        close(sock);
        throw;
    }
    close(sock);

    return netmask;
}

// Gets flags for the found iface
const short Resources::getFlags(const struct ifreq *ifr) const
{
    BOOST_ASSERT(ifr);
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in socket(): %s",
                             errno, strerror(errno));
    }

    // getting additional info
    short flags = 0;
    try
    {
        //get netmask
        struct ifreq copy = *ifr;
        if(ioctl(sock, SIOCGIFFLAGS, reinterpret_cast<char*>(&copy)) == -1)
        {
            throw Exception(__FILE__, __LINE__,
                                 "Error %d in ioctl(): %s",
                                 errno, strerror(errno));
        };
        flags = copy.ifr_flags;
    }
    catch(...)
    {
        close(sock);
        throw;
    }
    close(sock);

    return flags;
}
