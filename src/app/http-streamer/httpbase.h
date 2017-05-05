/**
   @file httpbase.h
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
   - 2009/03/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_HTTPBASE_H
#define KLK_HTTPBASE_H

#include "thread.h"

namespace klk
{
    namespace http
    {
        class Factory;

        /**
           @brief Base class for HTTP related objects

           The base base is the base class for route input/output bases

           @ingroup grROUTE
        */
        class Base
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            Base(Factory* factory);

            /**
               Destructor
            */
            virtual ~Base();
        protected:
            /**
               Retrives HTTP factory

               @return pointer to the factory

               exception klk::Exception
            */
            Factory* getFactory();
        private:
            Factory* m_httpfactory; ///< http factory
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Base& operator=(const Base& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Base(const Base& value);
        };

        /**
           Base container class

           @ingroup grHTTP
        */
        class BaseContainer : public Base
        {
        public:
            /**
               Constructor
            */
            BaseContainer(Factory* factory);

            /**
               Destructor
            */
            virtual ~BaseContainer();

            /**
               Stops container
            */
            void stop() throw();
        protected:
            mutable klk::Mutex m_lock; ///< locker
            klk::Trigger m_stop; ///< stop trigger
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            BaseContainer(const BaseContainer& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            BaseContainer& operator=(const BaseContainer& value);
        };
    }
}

#endif //KLK_HTTPBASE_H
