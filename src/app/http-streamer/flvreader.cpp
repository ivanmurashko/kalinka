/**
   @file flvreader.cpp
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
   - 2009/04/28 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

/*
 * yamdi.c
 *
 * Copyright (c) 2007-2008, Ingo Oppermann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * -----------------------------------------------------------------------------
 *
 * Compile with:
 * gcc yamdi.c -o yamdi -Wall -O2
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "flvreader.h"
#include "exception.h"
#include "defines.h"

using namespace klk;
using namespace klk::http;

// Constructor
FLVReader::FLVReader(const ISocketPtr& sock) :
    Reader(sock, WAITINTERVAL), m_header(13), m_real_header()
{
    setHeader();
}

// Destructor
FLVReader::~FLVReader()
{
}

// retrives content type string
const std::string FLVReader::getContentType() const throw()
{
    return std::string("application/flash-video");
}

// Retrives header
const BinaryData FLVReader::getHeader() const
{
    return m_header;
}

// Sets header
void FLVReader::setHeader()
{
    /**
       The first frame of a FLV stream is always a metadata
       which begins with 'F' (0x46), 'L' (0x4C), 'V' (0x56)
       and other header informations.
       and all following audio/video frames are treated as
       frame body which complies following format:
       TagType (1 byte) (18 for metadata, 8 for audio, 9 for video)
       DataSize(3 bytess) (Length of data, excluding header's length)
       TimeStamp (3 bytes) (you need to put your time stamp here)
       TimestampExtended (1 byte, i've never used this field :) )
       streamID(1 byte) ( always 0)
       data (Audo/video data)
    */

    // test header
    u_char header[] = {0x46, 0x4C, 0x56, /*FLV*/
                       0x01, /*version*/
                       0x05, /*both audio and video*/
                       0x00, 0x00, 0x00, 0x09, /*Data Offset*/
                       0x00, 0x00, 0x00, 0x00  /*First lastTagSize*/
    };

    BOOST_ASSERT(m_header.size() == 13);
    memcpy (m_header.toVoid(), header, 13);
}

//
// Yamdi data
//

#define FLV_UI32(x) (int)(((x[0]) << 24) + ((x[1]) << 16) + ((x[2]) << 8) + (x[3]))
#define FLV_UI24(x) (int)(((x[0]) << 16) + ((x[1]) << 8) + (x[2]))

#define FLV_AUDIODATA	8
#define FLV_VIDEODATA	9
#define FLV_SCRIPTDATAOBJECT	18

typedef struct {
    unsigned char signature[3];
    unsigned char version;
    unsigned char flags;
    unsigned char headersize[4];
} FLVFileHeader_t;

typedef struct {
    unsigned char type;
    unsigned char datasize[3];
    unsigned char timestamp[3];
    unsigned char timestamp_ex;
    unsigned char streamid[3];
} FLVTag_t;

// Reads a data portion
void FLVReader::getData(BinaryData& data)
{
    if (m_real_header.empty())
    {
        // get the header first
        size_t header_size = sizeof(FLVFileHeader_t);
        BinaryData header(header_size);
        m_sock->recvAll(header);
        FLVFileHeader_t* flvheader =
            static_cast<FLVFileHeader_t*>(header.toVoid());
        size_t real_header_size = FLV_UI32(flvheader->headersize) + 4;
        BOOST_ASSERT(real_header_size > header_size);
        BinaryData rest(real_header_size - header_size);
        m_sock->recvAll(rest);
        m_real_header.add(header);
        m_real_header.add(rest);
#if 0
        klk_log(KLKLOG_DEBUG, "FLV header size: %d, real_size: %d",
                header_size, m_real_header.size());
#endif
    }

    // Now read a tag
    size_t tag_header_size = sizeof(FLVTag_t);
    BinaryData tag(tag_header_size);
    m_sock->recvAll(tag);
    BOOST_ASSERT(tag.size() == tag_header_size);
    // determine data size
    FLVTag_t* flvtag = static_cast<FLVTag_t*>(tag.toVoid());

    // Die Groesse des Tags (Header + Data) + PreviousTagSize
    size_t real_data_size =  FLV_UI24(flvtag->datasize) + 4;
    BinaryData real_data(real_data_size);
    m_sock->recvAll(real_data);
    if(flvtag->type != FLV_VIDEODATA && flvtag->type != FLV_AUDIODATA &&
       flvtag->type != FLV_SCRIPTDATAOBJECT)
    {
        // broken package
        increaseBrokenCount();
        klk_log(KLKLOG_DEBUG, "FLV '%d' data size: %d, real_data_size: %d",
                flvtag->type, real_data_size, real_data.size());

    }
    BOOST_ASSERT(real_data.size() == real_data_size);

    BOOST_ASSERT(data.empty() == true);
    data.add(tag);
    data.add(real_data);
}

