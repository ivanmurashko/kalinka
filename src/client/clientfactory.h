/**
   @file clientfactory.h
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
   - 2007/06/30 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CLIENTFACTORY_H
#define KLK_CLIENTFACTORY_H

#include "client.h"
#include "moduleprocessor.h"
#include "factory.h"
#include "garbage.h"

namespace klk
{
    namespace cli
    {
        /**
           @brief The CLI client factory

           The CLI client factory

           @ingroup grClient
        */
        class Factory : public base::Factory
        {
        public:
            /**
               Destructor
            */
            ~Factory();

            /**
               Gets unique instance of the factory
               Pattern Singleton

               Replaces constructor
            */
            static Factory* instance();

            /**
               Replaces the destructor
            */
            static void destroy();

            /**
               Get an instance of the daemon client

               @return the pointer of the object

               @exception klk::Exception
            */
            Client* getClient();

            /**
               Retrives module processor

               @return a pointer to the object

               @exception klk::Exception
            */
            ModuleProcessor* getModuleProcessor();

            /**
               Gets garbage collector

               @return a pointer to the object

               @exception klk::Exception
            */
            Garbage* getGarbage();
        private:
            static Factory* m_instance; ///< the instance
            Client* m_client; ///< client
            ModuleProcessor* m_modprocessor; ///< module processor
            Garbage* m_garbage; ///< garbage collector
        private:
            /**
               Constructor
            */
            Factory();
        };
    }
}

#endif //KLK_CLIENTFACTORY_H
