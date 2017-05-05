/**
   @file resources.h
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
   - 2008/09/05 created by ipp (Ivan Murashko)
   - 2009/03/02 BaseResources class introduced by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_RESOURCES_H
#define KLK_RESOURCES_H

#include "baseresources.h"

namespace klk
{
    namespace dev
    {
        /**
           @brief Resources info

           The class keeps info about resources

           @ingroup grResources
        */
        class Resources : public BaseResources
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory object
            */
            explicit Resources(IFactory* factory);

            /**
               Destructor
            */
            virtual ~Resources();

            /**
               Checks main devices

               That are used by all modules

               @exception @ref klk::Exception
            */
            virtual void initDevs();
        private:
            /**
               Copy constructor
            */
            Resources(const Resources&);

            /**
               Assignment opearator
            */
            Resources& operator=(const Resources&);
        };
    }
}

#endif //KLK_RESOURCES_H
