/**
   @file flvreader.h
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

#ifndef KLK_FLVREADER_H
#define KLK_FLVREADER_H

#include "reader.h"

namespace klk
{
namespace http
{
    /**
       @brief Flv reader

       Reads FLV data portion. FLV format consists of several parts. There are
       - Fixed header
       - Data packets

       The data packets have variable length.

       @ingrou grHTTPReader
    */
    class FLVReader : public Reader
    {
    public:
        /**
           Destructor
        */
        virtual ~FLVReader();

        /**
           Make function

           @param[in] sock the socket for costructor

           @return the created object
        */
        static const IReaderPtr make(const ISocketPtr& sock)
        {
            return IReaderPtr(new FLVReader(sock));
        }
    private:
        BinaryData m_header; ///< header
        BinaryData m_real_header; ///< real header

        /**
           Constructor

           @param[in] sock - the socket
        */
        FLVReader(const ISocketPtr& sock);

        /**
           Retrives content type for HTTP response

           @return the type string
        */
        virtual const std::string getContentType() const throw();

        /**
           Retrives header

           @return the header

           @exception klk::Exception
        */
        virtual const BinaryData getHeader() const;

        /**
           Reads a data portion

           @param[out] data - the data container

           @exception klk::Exception
        */
        virtual void getData(BinaryData& data);

        /**
           Sets header

           @exception klk::Exception
        */
        void setHeader();
    private:
        /**
           Assigment operator
           @param[in] value - the copy param
        */
        FLVReader& operator=(const FLVReader& value);

        /**
           Copy constructor
           @param[in] value - the copy param
        */
        FLVReader(const FLVReader& value);
    };
}
}


#endif //KLK_FLVREADER_H
