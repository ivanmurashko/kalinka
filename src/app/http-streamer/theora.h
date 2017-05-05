/**
   @file theora.h
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

#ifndef KLK_THEORA_H
#define KLK_THEORA_H

#include <map>
#include <ogg/ogg.h>

#include "reader.h"


namespace klk
{
    namespace http
    {
        /**
           @brief Theora (ogg) format reader

           There is a theora (ogg) format reader. It consists of packets with variable length
        */
        class TheoraReader : public Reader
        {
            friend class TestTheora;
        public:
            /**
               Destructor
            */
            virtual ~TheoraReader();

            /**
               Make function

               @param[in] sock the socket for costructor

               @return the created object
            */
            static const IReaderPtr make(const ISocketPtr& sock)
            {
                return IReaderPtr(new TheoraReader(sock));
            }
        private:
            /**
               Info about a stream
            */
            struct StreamInfo
            {
                bool m_was_header; ///< was there a header for the stream

                /**
                   Default constructor
                */
                StreamInfo() : m_was_header(false){}
            };

            /**
               The map with info about streams
            */
            typedef std::map<int, StreamInfo> StreamInfoMap;

            BinaryData m_header; ///< header
            ogg_sync_state m_state; ///< ogg state
            mutable Mutex m_header_lock;  ///< the header lock
            StreamInfoMap m_streams; ///< info about streams

            /**
               Constructor

               @param[in] sock - the socket with data to read
            */
            TheoraReader(const ISocketPtr& sock);

            /**
               @copydoc klk::http::IReader::getContentType
            */
            virtual const std::string getContentType() const throw();

            /**
               @copydoc klk::http::IReader::getHeader
            */
            virtual const BinaryData getHeader() const;

            /**
               @copydoc klk::http::IReader::getData
            */
            virtual void getData(BinaryData& data) ;

            /**
               Reads a data portion in the form of OGG page

               @return ogg page
             */
            ogg_page getPage();

            /**
               Extracts binary data from page structure

               @param[in] page - the page info holder

               @return the binary data from the page
            */
            static const BinaryData page2Data(const ogg_page& page);

            /**
               Retrives data size from the page

               @return
               - zero  - EOS
               - real size in other cases

               @exception klk::Exception if there was an error
            */
            static size_t getSize(const ogg_page& page);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TheoraReader(const TheoraReader& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TheoraReader& operator=(const TheoraReader& value);
        };
    }
}

#endif //KLK_THEORA_H
