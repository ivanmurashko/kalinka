/**
   @file testfactory.h
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
   - 2007/07/03 created by ipp
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTFACTORY_H
#define KLK_TESTFACTORY_H

#include <string>
#include <map>

#include "factory.h"
#include "libcontainer.h"

/** @defgroup grTest Unit tests for Kalinka Mediaserver
    @{
    @brief The group includes all necessary functions and macro for unit testing

    we use CppUnit framework for the unit testing facility

*/

namespace klk
{
    namespace test
    {
        /**
           @brief Factory realization

           @ingroup grTest
        */
        class Factory : public base::Factory
        {
        public:
            /**
               Destructor
            */
            virtual ~Factory();

            /**
               Gets an unique instance of the class

               "Replaces" constructor
            */
            static Factory* instance();

            /**
               Gets the list of all modules in the form of std::string
            */
            std::string getTestIDs();

            /**
               Reset prev state (especially the db
            */
            void reset();

            /**
               Gets's module factory interface

               @return the module's factory
            */
            virtual IModuleFactory* getModuleFactory();

            /// Replaces the destructor
            static void destroy();
        private:
            static Factory *m_instance; ///< instance
            std::string m_ids; ///< tests ids
            LibContainerMap m_handles; ///< handles map

            /**
               Constructor
            */
            Factory();

            /**
               Creates all objects
            */
            void createAll();

            /**
               Inits a module for testing

               @param[in] fname - the file path
            */
            void initModUTest(const std::string& fname);

            /**
               Gets make function for unit test initialization

               @param[in] libpath - the lib path

               @return a result of dlsym or NULL if tehre was an error
            */
            void* getModUTestFun(const std::string& libpath);
        };
    }
}

/** @} */


#endif //KLK_FACTORY_H
