/**
   @file checkdb.h
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
   - 2009/03/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CHECKDB_H
#define KLK_CHECKDB_H

#include "moduledb.h"
#include "modulescheduler.h"

namespace klk
{
    namespace db
    {
        /** @defgroup grCheckDB DB checker

            MySQL database what is used by kalinka mediaserver does not
            provide a notification mechanism as postgresql does:
            http://developer.postgresql.org/pgdocs/postgres/libpq-notify.html
            Thus there is no possibility to know about @ref grDB "DB" changes made
            by an external application, for instance our web application.

            The module provides a notification mechanism about
            @ref grDB "our database" modifications.

            The module peridiocally retrive info about modules that have changed data
            at the DB and send them a notification message

            @ingroup grModule

            @{
        */

        /**
           @brief The DB checker module

           The DB checker module
        */
        class CheckDB : public Module
        {
        public:
            /**
               Constructor

               @param[in] factory the module factory
            */
            CheckDB(IFactory *factory);

            /**
               Destructor
            */
            virtual ~CheckDB();
        private:
            int m_last; ///< last processed id at the DB

            /**
               Register all processors

               @exception @ref klk::Exception
            */
            virtual void registerProcessors();

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
               Checks DB changes

               @exception klk::Exception
            */
            void checkDB();
        private:
            /**
               Copy constructor
            */
            CheckDB(const CheckDB&);

            /**
               Assignment operator
            */
            CheckDB& operator=(const CheckDB&);
        };

        /** @} */

    }
}

#endif //KLK_CHECKDB_H

