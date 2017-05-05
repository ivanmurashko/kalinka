/**
   @file serverfactory.h
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/07/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SERVERFACTORY_H
#define KLK_SERVERFACTORY_H

#include "factory.h"
#include "server.h"

namespace klk
{
    namespace server
    {
        /**
           @brief Server factory

           Mediaserver application (daemon) factory

           @note pattern singleton (Meyers singleton)

           @ingroup grServer
        */
        class Factory : public base::Factory
        {
        public:
            /**
               Destructor
            */
            virtual ~Factory(){}

            /**
               Gets unique instance of the factory

               Replaces constructor
            */
            static Factory * instance();

            /**
               Get an instance of the daemon server

               @return the pointer of the object
            */
            ServerDaemon* getServer();
        private:
            ServerDaemon *m_server; ///< server

            /**
               Constructor
            */
            Factory();

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Factory(const Factory& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Factory& operator=(const Factory& value);
        };
    }
}

#endif //KLK_SERVERFACTORY_H
