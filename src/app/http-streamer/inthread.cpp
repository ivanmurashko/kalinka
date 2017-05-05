/**
   @file inthread.cpp
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
   - 2009/03/09 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <string.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include "inthread.h"
#include "exception.h"
#include "httpfactory.h"
#include "defines.h"

#include "utils.h"

// module specific
#include "network/defines.h"

using namespace klk;
using namespace klk::http;

//
// InputInfo class
//

// Constructor
// FIXME!!! should check route parameter not NULL
InputInfo::InputInfo(const RouteInfoPtr& route) :
    mod::Info(route->getUUID(), route->getName()),
    m_route(route), m_path(), m_mname(), m_muuid()
{
    BOOST_ASSERT(m_route.getValue());
}

// Sets the path
void InputInfo::setPath(const std::string& path)
{
    BOOST_ASSERT(path.empty() == false);
    m_path = path;
}

// Sets the media type name
void InputInfo::setMediaTypeName(const std::string& mname)
{
    BOOST_ASSERT(mname.empty() == false);
    m_mname = mname;
}

// Sets the media type uuid
void InputInfo::setMediaTypeUuid(const std::string& muuid)
{
    BOOST_ASSERT(muuid.empty() == false);
    m_muuid = muuid;
}

//
// InThread class
//

// Constructor
InThread::InThread(Factory* factory, const InputInfoPtr& info) :
    RouteThread(factory), m_reader_lock(), m_info(info),
    m_reader(), m_con_count_lock(), m_con_count(0)
{
    BOOST_ASSERT(m_info);
    setRoute(m_info->getRouteInfo());
}

// Destructor
InThread::~InThread()
{
}

// Main thread's body
void InThread::start()
{
    // only one connection
    while (!isStopped())
    {
        try
        {
            resetReader();
            doLoop();
        }
        catch(const std::exception& err)
        {
            klk_log(KLKLOG_ERROR,
                    "Got an exception at HTTP input thread. "
                    "Thread is listening on %s:%d. Exception: %s",
                    getRoute()->getHost().c_str(),
                    getRoute()->getPort(),
                    err.what());
        }
        catch(...)
        {
            klk_log(KLKLOG_ERROR,
                    "Got an unknown exception at HTTP input thread. "
                    "Thread is listening on %s:%d",
                    getRoute()->getHost().c_str(),
                    getRoute()->getPort());
        }

        // final clearing
        if (!isStopped())
        {
            //FIXME!!! bad code

            // stop all connections associated with the thread
            InThreadPtr self = getFactory()->getInThreadContainer()->getThreadByPath(getInfo()->getPath());
            BOOST_ASSERT(self.get() == this);
            // get self
            getFactory()->getConnectThreadContainer()->stopThreads(self);
        }
    }
}

// Gets info
const InputInfoPtr InThread::getInfo() const throw()
{
    Locker lock(&m_lock);
    return m_info;
}

// Updates input info
void InThread::updateInfo(const InputInfoPtr& info)
{
    BOOST_ASSERT(info);
    Locker lock(&m_lock);
    BOOST_ASSERT(m_info); // FIXME!!! may be not so important
                               // should be always not null
    // protocol should not be changed
    BOOST_ASSERT(info->getRouteInfo()->getProtocol() ==
                    m_info->getRouteInfo()->getProtocol());

    if (!checkRoute(info->getRouteInfo()->getUUID()))
    {
        // update it
        setRoute(info->getRouteInfo());
    }
    m_info = info;
}


// stops thread execution
void InThread::stop() throw()
{
    RouteThread::stop();

    Locker lock(&m_reader_lock);
    if (m_reader)
    {
        m_reader->stop();
    }
}

// Retrives the reader
const IReaderPtr InThread::getReader() const
{
    Locker lock(&m_reader_lock);
    BOOST_ASSERT(m_reader);
    return m_reader;
}

// Resets reader
void InThread::resetReader()
{
    Locker lock(&m_reader_lock);
    m_reader.reset();
}

// Inits reader
void InThread::initReader()
{
    ISocketPtr sock = getListener()->accept();

    // setting keep alive interval
    sock->setKeepAlive(10);

    Locker lock(&m_reader_lock);
    const std::string mtype = m_info->getMediaTypeUuid();
    m_reader = getFactory()->getReader(mtype, sock);

    klk_log(KLKLOG_DEBUG,
            "Got income connection for HTTP streamer: "
            "%s -> %s:%d",
            m_reader->getPeerName().c_str(),
            getRoute()->getHost().c_str(),
            getRoute()->getPort());
}

// Retrives rate
const double InThread::getRate() const
{
    return getReader()->getRate();
}

// Do the main loop action
// wait for a data portion and send it to connection threads
void InThread::doLoopAction()
{
    while (getReader()->checkData() == ERROR)
    {
        if (isStopped())
            return;
        // just log and try again
        klk_log(KLKLOG_DEBUG,
                "No income connection from %s within %d sec. "
                "for input HTTP thread on %s:%d",
                getReader()->getPeerName().c_str(),
                WAITINTERVAL,
                getRoute()->getHost().c_str(),
                getRoute()->getPort());
    }
    if (isStopped())
        return;


    BinaryData buff;
    getReader()->getData(buff);
    BOOST_ASSERT(buff.empty() == false);

#if 0
    klk_log(KLKLOG_DEBUG,
            "Got %d bytes for HTTP streamer at inthread", buff.size());
#endif

    // send the data to recived connection from end-users
    getFactory()->getConnectThreadContainer()->sendConnectionData(
        m_info->getPath(), buff);
}

// Increases connection count
void InThread::increaseConnectionCount()
{
    Locker lock(&m_con_count_lock);
    m_con_count++;
}

// Decreases connection count
void InThread::decreaseConnectionCount()
{
    Locker lock(&m_con_count_lock);
    if (m_con_count > 0)
        m_con_count--;
}

// Retrives connection count for displaying
const u_long InThread::getConnectionCount() const throw()
{
    Locker lock(&m_con_count_lock);
    return m_con_count;
}

//
// InThreadContainer class
//

// Constructor
InThreadContainer::InThreadContainer(Factory* factory) :
    BaseContainer(factory), m_list()
{
}

// Destructor
InThreadContainer::~InThreadContainer()
{
}

#if 0

// Sets input threds accordingly with the info at the list
// Only threads that apply the route info will be keep and new threads
// will be start
void InThreadContainer::setInfo(const InputInfoList& list)
{
    if (m_stop.isStopped())
    {
        return;
    }

    InThreadList old = m_list;

    // process all gotten data
    std::for_each(list.begin(), list.end(),
                  boost::bind(&InThreadContainer::addInfo, this,
                              _1, boost::ref(old)));

    // remove missing
    std::for_each(old.begin(), old.end(),
                  boost::bind(&InThreadContainer::delInfo,
                              this, _1));
}


// Deletes an imput route
void InThreadContainer::delInfo(const InThreadPtr& thread)
{
    BOOST_ASSERT(thread);
    // stop the input thread
    getFactory()->getScheduler()->stopThread(thread);
    // remove it from the known list
    {
        Locker lock(&m_lock);
        m_list.remove(thread);
    }

    // now stop all connection threads associate with the current
    // input thread
    getFactory()->getConnectThreadContainer()->stopThreads(thread);
}

// Retrives a list with input info
const InputInfoList InThreadContainer::getInfo() const
{
    Locker lock(&m_lock);
    InputInfoList result;

    for (InThreadList::const_iterator i = m_list.begin();
         i != m_list.end(); i++)
    {
        result.push_back((*i)->getInfo());
    }

    return result;
}

#endif

/**
   Helper functor to search the route info
*/
struct IsRouteMatch
{
    bool operator()(const InputInfoPtr info,
                    const InThreadPtr thread) {
        return (thread->getInfo()->getRouteInfo()->getUUID() ==
                info->getRouteInfo()->getUUID());
    }

    bool operator()(const std::string path,
                    const InThreadPtr thread){
        std::string path1 = path;
        std::string path2 = thread->getInfo()->getPath();
        boost::trim_if(path1,  boost::is_any_of("/"));
        boost::trim_if(path2,  boost::is_any_of("/"));
        if (path1.empty() || path2.empty())
            return false;
        return (path1 == path2);
    }
};


// Will add info to be processed
void InThreadContainer::addInfo(const InputInfoPtr& info)
{
    Locker lock(&m_lock);

    InThreadList::iterator found =
        std::find_if(m_list.begin(), m_list.end(),
                     boost::bind<bool>(IsRouteMatch(), info, _1));
    if (found != m_list.end())
    {
        (*found)->updateInfo(info);
    }
    else
    {
        // path should be unique
        InThreadList::iterator find_path = std::find_if(
            m_list.begin(), m_list.end(),
            boost::bind<bool>(IsRouteMatch(),
                              info->getPath(), _1));
        BOOST_ASSERT(find_path == m_list.end());

        if (m_stop.isStopped())
            return;

        // start the thread
        InThreadPtr thread = getFactory()->getInThread(info);
        getFactory()->getScheduler()->startThread(thread);
        m_list.push_back(thread);
    }
}

// Will add info to be processed
void InThreadContainer::delInfo(const InputInfoPtr& info)
{
    BOOST_ASSERT(info);

    const InThreadPtr thread = getThreadByPath(info->getPath());
    BOOST_ASSERT(thread);
    // stop the input thread
    getFactory()->getScheduler()->stopThread(thread);
    // remove it from the known list
    {
        Locker lock(&m_lock);
        m_list.remove(thread);
    }

    // now stop all connection threads associate with the current
    // input thread
    getFactory()->getConnectThreadContainer()->stopThreads(thread);
}

// Retrives thread by path
const InThreadPtr InThreadContainer::getThreadByPath(const std::string& path)
{
    if (path.empty())
    {
        return InThreadPtr();
    }

    Locker lock(&m_lock);
    InThreadList::iterator i = std::find_if(
        m_list.begin(), m_list.end(),
        boost::bind<bool>(IsRouteMatch(), path, _1));
    if (i == m_list.end())
    {
        return InThreadPtr();
    }

    return *i;

}
