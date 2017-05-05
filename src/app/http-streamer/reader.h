/**
   @file reader.h
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

#ifndef KLK_READER_H
#define KLK_READER_H

#include <sys/types.h>

#include <string>

#include <boost/shared_ptr.hpp>

#include "socket/socket.h"
#include "errors.h"
#include "thread.h"

namespace klk
{
    namespace http
    {

        /** @defgroup grHTTPReader Readers
            @{

            The streamer application supports various types of media data.
            Data in different formats transfert in different manner. For example
            mpegts has fix packet size but FLV does not. HTTP streamer application
            should not send incomplete data packet to receiver. Thus we need something that
            incapsulate data read and processing for different media formats.

            @ingroup grHTTP
        */

        /**
           @brief Reader interface

           Reader interface
        */
        class IReader
        {
        public:
            /**
               Destructor
            */
            virtual ~IReader(){}

            /**
               Retrives content type for HTTP response

               @return the type string
            */
            virtual const std::string getContentType() const throw() = 0;

            /**
               Retrives header

               @return the header

               @exception klk::Exception
            */
            virtual const BinaryData getHeader() const = 0;

            /**
               Reads a data portion

               @param[out] data - the data container

               @exception klk::Exception
            */
            virtual void getData(BinaryData& data) = 0;

            /**
               Retrives peer name

               @return the peername

               @exception klk::Exception
            */
            virtual const std::string getPeerName() const = 0;

            /**
               Stop accept data
            */
            virtual void stop() throw() = 0;

            /**
               @see klk::Socket::checkData
            */
            virtual const Result checkData() = 0;

            /**
               Retrives broken packages count

               @return the count
            */
            virtual const u_long getBrokenCount() const = 0;

            /**
               Retrives rate

               @return the rate
            */
            virtual const double getRate() const = 0;
        };

        /**
           Smart pointer for IReader
        */
        typedef boost::shared_ptr<IReader> IReaderPtr;


        /**
           @brief Base reader class

           Base reader class
        */
        class Reader : public IReader
        {
            friend class TestUDP;
        public:
            /**
               Constructor

               @param[in] sock - the socket
               @param[in] wait_interval - the wait interval
            */
            Reader(const ISocketPtr& sock, const time_t wait_interval);

            /**
               Destructor
            */
            virtual ~Reader();
        protected:
            ISocketPtr m_sock; ///< socket

            /**
               Increases broken package count
            */
            void increaseBrokenCount();
        private:
            mutable Mutex m_broken_count_lock; ///< broken count lock
            u_long m_broken_count; ///< broken package count
            time_t m_wait_interval; ///< wait interval for incomming connections

            /**
               Stop accept data
            */
            virtual void stop() throw();

            /**
               Retrives peer name

               @return the peername

               @exception klk::Exception
            */
            virtual const std::string getPeerName() const;

            /**
               @see klk::Socket::checkData
            */
            virtual const Result checkData();

            /**
               Retrives broken packages count

               @return the count
            */
            virtual const u_long getBrokenCount() const;

            /**
               Retrives rate

               @return the rate
            */
            virtual const double getRate() const;
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Reader(const Reader& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Reader& operator=(const Reader& value);
        };

        /** @} */

    }
}

#endif //KLK_READER_H
