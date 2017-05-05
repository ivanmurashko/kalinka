/**
   @file theora.cpp
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
   - 2010/02/22 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "theora.h"
#include "exception.h"
#include "defines.h"

using namespace klk;
using namespace klk::http;

//
// TheoraReader class
//

// Constructor
TheoraReader::TheoraReader(const ISocketPtr& sock) :
    Reader(sock, WAITINTERVAL), m_header(), m_state(), m_streams()
{
    if (ogg_sync_init(&m_state) != 0)
    {
        throw Exception(__FILE__, __LINE__, "ogg_sync_init() failed");
    }
}

// Destructor
TheoraReader::~TheoraReader()
{
    ogg_sync_clear(&m_state);
}

// retrives content type
const std::string TheoraReader::getContentType() const throw()
{
    // rfc5334
    return "application/ogg";
}

// Retrives header
const BinaryData TheoraReader::getHeader() const
{
    Locker lock(&m_header_lock);
    return m_header;
}

// Extracts binary data from page structure
// static
const BinaryData TheoraReader::page2Data(const ogg_page& page)
{
    long total_size = page.header_len + page.body_len;
    if (total_size < 0)
    {
        throw Exception(__FILE__, __LINE__, "Incorrect OGG frame size: %ld", total_size);
    }

    if (total_size == 0)
    {
        klk_log(KLKLOG_DEBUG, "EOS detected for theora reader");
        return BinaryData();
    }

    BinaryData data(static_cast<size_t>(total_size));
    u_char* header = static_cast<u_char*>(data.toVoid());
    memcpy(header, page.header, page.header_len);

    if (page.body_len > 0)
    {
        u_char* body = header + page.header_len;
        memcpy(body, page.body, page.body_len);
    }

    return data;
}

// Retrives data size from the page
// static
size_t TheoraReader::getSize(const ogg_page& page)
{
    long total_size = page.header_len + page.body_len;
    if (total_size < 0)
    {
        throw Exception(__FILE__, __LINE__, "Incorrect OGG frame size: %ld", total_size);
    }

    return static_cast<size_t>(total_size);
}

// Reads a data portion
void TheoraReader::getData(BinaryData& data)
{
    ogg_page page = getPage();

    if (getSize(page) == 0)
    {
        klk_log(KLKLOG_DEBUG, "EOS detected for theora reader");
        data.resize(0);
        return;
    }

    data = page2Data(page);

    int serial = ogg_page_serialno(&page);

    // headers detection
    if (ogg_page_bos(&page))
    {
        m_streams[serial] = StreamInfo();
        Locker lock(&m_header_lock);
        // there is a begining of a stream
        m_header.add(data);
        KLKASSERT(!ogg_page_continued(&page));
        return; // nothing to do
    }

    // is there a multiply pages here
    if (ogg_page_continued(&page))
    {
        while(ogg_page_continued(&page))
        {
            page = getPage();
            if (getSize(page) == 0)
            {
                klk_log(KLKLOG_DEBUG, "EOS detected for theora reader");
                break;
            }
            data.add(page2Data(page));
        }
    }

    // we assume that the first packet (page) at a stream is the
    // header and should be stored
    //BOOST_ASSERT(m_streams.find(serial) != m_streams.end());
    if (!m_streams[serial].m_was_header)
    {
        m_streams[serial].m_was_header = true;
        Locker lock(&m_header_lock);
        m_header.add(data);
    }
}

// Reads a data portion in the form of OGG page
ogg_page TheoraReader::getPage()
{
    ogg_page page;
    memset(&page, 0, sizeof(page));

    // FIXME!!! not optimal code here
    size_t buff_size = 4096;
    BinaryData data(buff_size);
    while(ogg_sync_pageout(&m_state, &page) != 1)
    {

        char* buffer = ogg_sync_buffer(&m_state, buff_size);
        BOOST_ASSERT(buffer);

        m_sock->recv(data);

        if (data.empty())
        {
            // EOS
            break;
        }

        // copy data
        memcpy(buffer, data.toVoid(), data.size());

        if (ogg_sync_wrote(&m_state,  data.size()) != 0)
        {
            throw Exception(__FILE__, __LINE__, "ogg_sync_wrote() failed");
        }
    }

    return page;
}
