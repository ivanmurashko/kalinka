/**
   @file httpsrc.h
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
   - 2009/11/21 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_HTTPSRC_H
#define KLK_HTTPSRC_H

#include "modulewithinfo.h"
#include "httpinfo.h"

namespace klk
{
    namespace httpsrc
    {

        /** @defgroup grHTTPSrc HTTP source module

            The module provides info about external http source

            The info includes the following
            - address: IP address (or DNS name), port and path
            - auth info: login and password

            @ingroup grModule
            @{
        */

        /**
           @brief  The HTTP source module

           HTTP source module
        */
        class HTTPSrc : public ModuleWithInfo<HTTPInfo>
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory to get all necessary objects
            */
            explicit HTTPSrc(IFactory *factory);

            /**
               Destructor
            */
            virtual ~HTTPSrc();
        private:
            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw();

            /**
               @copydoc IModule::getType
            */
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Retrives an set with data from @ref grDB "database"

               @return set
            */
            virtual const InfoSet getInfoFromDB();

            /**
               Do the start info usage for a http source

               @param[in] in - the input message
               @param[out] out - the output message

               @exception klk::Exception
            */
            virtual void doStart(const IMessagePtr& in,
                                 const IMessagePtr& out) ;

            /**
               Register all processors

               @exception klk::Exception
            */
            virtual void registerProcessors();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            HTTPSrc& operator=(const HTTPSrc& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            HTTPSrc(const HTTPSrc& value);
        };

        /** @} */
    }
}

#endif //KLK_HTTPSRC_H
