/**
   @file testmodfactory.h
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
   - 2008/12/08 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTMODFACTORY_H
#define KLK_TESTMODFACTORY_H

#include "modfactory.h"

namespace klk
{
    namespace test
    {
        /**
           @brief Module factory for unit tests

           ModuleFactory
        */
        class ModuleFactory : public klk::ModuleFactory
        {
        public:
            /**
               Constructor

               @param[in] factory - the main factory for getting
               all necessary objects
            */
            ModuleFactory(IFactory* factory);

            /**
               Destructor
            */
            virtual ~ModuleFactory(){}

            /**
               Adds a module to the known modules list

               @param[in] module the module to be added
            */
            void addDebug(const IModulePtr& module);

            /**
               Clears all modules and internal state
            */
            void clear();

            /**
               @copydoc IModuleFactory::unloadAll

               @note it also does an additional check that all modules
               were really unloaded
            */
            virtual void unloadAll();
        private:
            /**
               Gets resources by module's id

               @param[in] id the module's id

               @return the pointer to  resources or NULL
               if the resources can not be found

               @note - there is an internal method that does not do any locks
            */
            virtual const IResourcesPtr getResources(const std::string& id);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ModuleFactory& operator=(const ModuleFactory& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ModuleFactory(const ModuleFactory& value);
        };
    }
}

#endif //KLK_TESTMODFACTORY_H
