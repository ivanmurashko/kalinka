/**
   @file dvbresources.h
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

#ifndef KLK_DVBRESOURCES_H
#define KLK_DVBRESOURCES_H

#include "ifactory.h"
#include "baseresources.h"

namespace klk
{
    namespace dvb
    {
        /**
           @brief DVB resources

           DVB Resources

           @ingroup grDVB
        */
        class Resources : public dev::BaseResources
        {
        public:
            /**
               Constructor
            */
            explicit Resources(IFactory* factory);

            /**
               Destructor
            */
            virtual ~Resources();

            /**
               Inits DVB resources
            */
            virtual void initDevs();
        private:
            /**
               Adds DVB device info

               @param[in] adapter - adapter number
               @param[in] frontend - frontend's number

               @exception @ref klk::Exception
            */
            void addDevInfo(u_int adapter, u_int frontend);

            /**
               Checks DVB device

               @param[in] name - the dev name to be checked

               @exception @ref klk::Exception
            */
            void checkDev(const std::string& name);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Resources(const Resources& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Resources& operator=(const Resources& value);
        };
    }
}

#endif //KLK_DVBRESOURCES_H
