/**
   @file conthread.cpp
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
   - 2009/03/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <sstream>

#include <boost/bind.hpp>

#include "conthread.h"
#include "exception.h"
#include "utils.h"
#include "httpfactory.h"
#include "version.h"
#include "utils.h"
#include "defines.h"

using namespace klk;
using namespace klk::http;

//
// ConnectThread class
//

// Constructor
ConnectThread::ConnectThread(Factory* factory,
                             const ISocketPtr& sock) :
    Thread(factory),
    m_uuid(base::Utils::generateUUID()),
    m_sock(sock), m_path(),
    m_data(), m_wait(), m_request_type(REQ_UNKNOWN),
    m_response_type(RES_UNKNOWN),
    m_http_version(HTTP_UNKNOWN),
    m_hang_time(0)
{
    m_sock->setSendTimeout(5/*WAITINTERVAL4SLOW*/);
}

// Destructor
ConnectThread::~ConnectThread()
{
}

// Thread initialization and check
void ConnectThread::init()
{
    BOOST_ASSERT(m_sock);
    Thread::init();
}


// main thread body
void ConnectThread::start()
{

    klk_log(KLKLOG_DEBUG, "Got a connection request");
    try
    {
        // do processing
        processRequest();
        if (m_request_type == GET && m_response_type == OK)
        {
            processData();
        }
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Exception gotten at HTTP connection thread: %s",
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Unknown exception gotten at HTTP connection thread");
    }

    // crease connetion count
    if (InThreadPtr inthread = getInThread())
    {
        inthread->decreaseConnectionCount();
    }

    m_sock->disconnect();
    notifyStop();
    klk_log(KLKLOG_DEBUG, "Finished connection thread");
}

// Retrives the path
const std::string ConnectThread::getPath() const throw()
{
    Locker lock(&m_lock);
    return m_path;
}

// Checks path
const bool ConnectThread::isPathMatch(const std::string& path) const throw()
{
    Locker lock(&m_lock);
    return (path == m_path);
}

// Sends a binary data portion
void ConnectThread::addData(const std::string& path,
                            const BinaryData& data)
{
    if (isPathMatch(path))
    {
        // Check hang up conditions
        if (m_data.size() > CONNECTIONBUFFER_MAX_SIZE)
        {
#if 0
            klk_log(KLKLOG_DEBUG, "Data size: %d->%d. Time: %d->%d",
                    m_data.size(), CONNECTIONBUFFER_MAX_SIZE,
                    time(NULL) - m_hang_time.getValue(),
                    WAITINTERVAL4SLOW);
#endif
            if (m_hang_time.getValue() == 0)
            {
                m_hang_time.setValue(time(NULL));
            }
            else if (time(NULL) - m_hang_time.getValue() > WAITINTERVAL4SLOW)
            {
                klk_log(KLKLOG_ERROR, "Hang-up detected. Requested path: %s. "
                        "Client: %s",
                        path.c_str(),
                        m_sock->getPeerName().c_str());
                // stop the thread
                stop();
                return;
            }
        }
        else
        {
            m_hang_time.setValue(0);
            m_data.push_back(data);
        }

        m_wait.stopWait();
    }
}

// Stops thread
void ConnectThread::stop() throw()
{
    Thread::stop();
    m_wait.stopWait();
}

// Sends an event about thread stop
void ConnectThread::notifyStop()
{
    getFactory()->getConnectThreadContainer()->notifyStopConnectThread(this);
}

// Processes request
void ConnectThread::processRequest()
{
    const std::string SEPARATOR1("\r\n\r\n");
    const std::string SEPARATOR2("\n\n");
    std::string request;
    for(;;)
    {
        if (m_sock->checkData(WAITINTERVAL) != klk::OK)
        {
            // timeout exceed
            throw Exception(__FILE__, __LINE__,
                            "No input data within %d seconds",
                            WAITINTERVAL);
        }
        BinaryData buff(SOCKBUFFSIZE);
        m_sock->recv(buff);
        if (buff.empty())
        {
            throw Exception(__FILE__, __LINE__,
                            "Connection closed by client");
        }

        request += buff.toString();
        // check separator
        if (request.find(SEPARATOR1) != std::string::npos ||
            request.find(SEPARATOR2) != std::string::npos)
        {
            // we got the header
            break;
        }
        if (buff.size() != SOCKBUFFSIZE)
        {
            // try again
            continue;
        }
    }

    // split by \r\n
    std::vector<std::string> vector = base::Utils::split(request, "\r\n");
    klk_log(KLKLOG_DEBUG, "Got the following HTTP request from %s: %s",
            m_sock->getPeerName().c_str(),
            vector[0].c_str());

    parseRequest(vector[0]);

    InThreadPtr inthread = getInThread();

    processResponse(inthread);

    // send a multimedia data header
    if (m_request_type == GET && m_response_type == OK && inthread)
    {
        const BinaryData header = inthread->getReader()->getHeader();
        if (header.empty())
        {
            klk_log(KLKLOG_DEBUG, "No header data for connection thread");
        }
        else
        {
            m_data.push_back(header);
            klk_log(KLKLOG_DEBUG,
                    "Header data for connection thread was added. "
                    "Header size: %d", header.size());
        }
    }
}

// Processes data
void ConnectThread::processData()
{
    // main loop
    while(!isStopped())
    {
        if (m_wait.startWait(WAITINTERVAL) != klk::OK)
        {
            // timeout exceed
            throw Exception(__FILE__, __LINE__,
                            "No input data within %d seconds",
                            WAITINTERVAL);
        }
        if (isStopped())
        {
            break;
        }
        // we have data to be sent
        while (m_data.empty() == false)
        {
            sendData(m_data.retrive_front());
        }
    }
}

// Sends a data portion
// There are some code from getstream2
// http://silicon-verl.de/home/flo/projects/streaming/
/*
 * Function to return an http chunk with previous unknown size.
 * HTTP/1.1 allows for chunked transfers which HTTP/1.0 doesnt
 * know about. There is no legal way to do this so we hope our best.
 *
 */
// See also http://en.wikipedia.org/wiki/Chunked_transfer_encoding
void ConnectThread::sendData(const BinaryData& data)
{
    if (data.empty())
    {
        // Chunked transfer - send a 0 chunk
        if (m_http_version == HTTP11)
        {
            m_sock->send(BinaryData("0\r\n\r\n"));
        }
        // FIXME!!! and the closing connection here
    }

    // In case of Transfer-Encoding: chunked send a chunk
    // otherwise just data
    if (m_http_version == HTTP11)
    {
        char head[64];
        snprintf(head, sizeof(head), "%x\r\n",
                 static_cast<u_int>(data.size()));
        m_sock->send(BinaryData(head));
        m_sock->send(data);
        m_sock->send(BinaryData("\r\n"));
    }
    else
    {
        // just send data
        m_sock->send(data);
    }

#if 0
    // save result
    base::Utils::saveData2File("contmp.flv", *data);
#endif
}

InThreadPtr ConnectThread::getInThread()
{
    std::string path;
    {
        Locker lock(&m_lock);
        path = m_path;
    }

    return getFactory()->getInThreadContainer()->getThreadByPath(path);
}

// Parses request and set internal parameters:
// request type, path and HTTP version
void ConnectThread::parseRequest(const std::string& reqstr)
{
    std::vector<std::string> vec = base::Utils::split(reqstr, "\t ");
    if (vec.size() != 3)
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect HTTP request: " + reqstr);
    }

    Locker lock(&m_lock);

    // request type
    if (vec[0] == "HEAD")
    {
        m_request_type = HEAD;
    }
    else if (vec[0] == "GET")
    {
        m_request_type = GET;
    }
    else
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect HTTP request: " + reqstr);
    }


    // path
    if (*vec[1].begin() != '/')
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect HTTP request: " + reqstr);
    }
    m_path = vec[1];

    // HTTP version
    m_http_version = HTTP_UNKNOWN;
    if (vec[2] == "HTTP/1.0")
    {
        m_http_version = HTTP10;
    }
    else if (vec[2] == "HTTP/1.1")
    {
        m_http_version = HTTP11;
    }
    else
    {
        throw Exception(__FILE__, __LINE__,
                        "Incorrect HTTP request: " + reqstr);
    }
}

// Processes a response
void ConnectThread::processResponse(const InThreadPtr& inthread)
{
    m_response_type = RES_UNKNOWN;

    if (inthread)
    {
        // stop waiting in input thread
        inthread->increaseConnectionCount();
        // set type and process the response
        m_response_type = OK;
        processOK(inthread);
    }
    else
    {
        m_response_type = NOT_FOUND;
        processNotFound();
    }
}

// Processes an OK response
void ConnectThread::processOK(const InThreadPtr& inthread)
{
    BOOST_ASSERT(inthread);
    BOOST_ASSERT(m_response_type == OK);

    klk_log(KLKLOG_DEBUG, "Starts processing new request for path: %s",
            getPath().c_str());

    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Date: " <<
        base::Utils::getCurrentTime("%a, %d %b %Y %H:%M:%S %Z\r\n");
    response << "Server: " << VERSION_STR << "\r\n";
    response << "Pragma: no-cache,private\r\n";
    response << "Cache-Control: no-cache\r\n";
    response << "Content-Type: "
             << inthread->getReader()->getContentType() << "\r\n";
    if (m_http_version == HTTP11)
    {
        response << "Transfer-Encoding: chunked\r\n";
    }
    else
    {
        response << "Accept-Ranges: bytes\r\n";
        response << "Length: unspecified [" <<
            inthread->getReader()->getContentType() << "]\r\n";
    }

    if (m_http_version == HTTP11 && m_request_type == GET)
    {
        response << "Connection: keep-alive\r\n";
    }
    else
    {
        response << "Connection: close\r\n";
    }

    response << "\r\n";

    m_sock->send(BinaryData(response.str()));
}

// Processes a Not Found response
void ConnectThread::processNotFound()
{

    klk_log(KLKLOG_ERROR, "Path '%s' was not found", getPath().c_str());

    BOOST_ASSERT(m_response_type == NOT_FOUND);

    std::stringstream response;
    response << "HTTP/1.1 404 Not Found\r\n";
    response << "Date: " <<
        base::Utils::getCurrentTime("%a, %d %b %Y %H:%M:%S %Z\r\n");
    response << "Server: " << VERSION_STR << "\r\n";
    response << "Pragma: no-cache,private\r\n";
    response << "Cache-Control: no-cache\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    if (m_request_type == GET)
    {
        const std::string data =
            "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
            "<html><head>"
            "<title>404 Not Found</title>"
            "</head><body>"
            "<h1>404 Not Found</h1>"
            "</body>"
            "</html>";
        response << data;
    }

    m_sock->send(BinaryData(response.str()));
}

// Retrives rate
const double ConnectThread::getRate() const
{
    return m_sock->getOutputRate();
}

//
// ConnectThreadContainer class
//

// Constructor
ConnectThreadContainer::ConnectThreadContainer(Factory* factory) :
    BaseContainer(factory), m_list()
{
}

// Destructor
ConnectThreadContainer::~ConnectThreadContainer()
{
}

// Sends a connection data to all out threads
void ConnectThreadContainer::sendConnectionData(const std::string& path,
                                                const BinaryData& data)
{
    if (m_stop.isStopped())
        return;

    BOOST_ASSERT(path.empty() == false);
    Locker lock(&m_lock);
    std::for_each(m_list.begin(), m_list.end(),
                  boost::bind(&ConnectThread::addData, _1,
                              boost::ref(path),
                              boost::ref(data)));
}

// Starts connection thread
void ConnectThreadContainer::startConnectThread(const ConnectThreadPtr& thread)
{
    if (m_stop.isStopped())
        return;

    // add to list before real startup because
    // input thread check the presence in the list
    // FIXME!!! bad code
    {
        Locker lock(&m_lock);
        m_list.push_back(thread);
    }

    getFactory()->getScheduler()->startThread(thread);
}

// Sends a notification about stop connection thread
void ConnectThreadContainer::notifyStopConnectThread(
    const ConnectThread* thread)
{
    if (m_stop.isStopped())
        return;

    BOOST_ASSERT(thread);
    Locker lock(&m_lock);
    ConnectThreadList::iterator i =
        std::find_if(m_list.begin(), m_list.end(),
                     boost::bind(std::equal_to<std::string>(),
                                 thread->getUUID(),
                                 boost::bind(&ConnectThread::getUUID, _1)));
    if (i == m_list.end())
        return; // nothing to do

    // stop it
    getFactory()->getStopThread()->addConnectThread(*i);
    // some clearing
    m_list.erase(i);
}

// Stops all connection threads associated with the current input oneh
void ConnectThreadContainer::stopThreads(const InThreadPtr& thread)
{
    BOOST_ASSERT(thread);
    const std::string path = thread->getInfo()->getPath();
    BOOST_ASSERT(path.empty() == false);

    ConnectThreadList stoplist;

    Locker lock(&m_lock);
    for (ConnectThreadList::iterator i = m_list.begin();
         i != m_list.end(); i++)
    {
        if ((*i)->getPath() == path)
        {
            stoplist.push_back(*i);
            getFactory()->getStopThread()->addConnectThread(*i);
        }
    }

    // remove them from the list
    for (ConnectThreadList::iterator i = stoplist.begin();
         i != stoplist.end(); i++)
    {
        m_list.remove(*i);
    }
}

// Retrive connection rate by input thread
const double
ConnectThreadContainer::getConnectionRate(const std::string& path) const
{
    BOOST_ASSERT(path.empty() == false);
    double res = 0;
    Locker lock(&m_lock);
    for (ConnectThreadList::const_iterator i = m_list.begin();
         i != m_list.end(); i++)
    {
        if ((*i)->getPath() == path)
        {
            res += (*i)->getRate();
        }
    }

    return res;
}

