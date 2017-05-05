/**
   @file launchbase.h
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
   - 2010/10/01 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_LAUNCHBASE_H
#define KLK_LAUNCHBASE_H

#include "ifactory.h"

namespace klk
{
    namespace launcher
    {
        /**
           @brief Helper class

           The class keeps several usefull methods that
           are used by another launcher classes

           @ingroup grLauncher
        */
        class Base
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory instance
            */
            explicit Base(IFactory* factory);

            /// Destructor
            virtual ~Base(){}
        protected:
            /**
               Check is the module process locally or remotelly

               @param[in] id - the module id to be checked

               @return
               - true the module is processed locally
               - false - the module is processed remotelly
            */
            bool isLocal(const std::string& id) const;

            /**
               Checks if the module specified as the argument is the main module id

               @param[in] id - the module id to be checked

               @return
               - true there is an id of main module
               - false - there is not an id of main module
            */
            bool isMainModule(const std::string& id) const
            {
                return (id == m_main_module_id);
            }
        private:
            /**
               The flag shows that the class uses the main (mediaserver)
               application factory

               @see klk::MODULE_COMMON_ID
            */
            const bool m_local;

            const std::string m_main_module_id; ///< main module id

            /**
               There is a list of modules that should be loaded
               locally. All others should be loaded remotely
            */
            StringList m_local_load_list;
        private:
            /// Fake copy constructor
            Base(const Base&);

            /// Fake assigment operator
            Base& operator=(const Base&);
        };
    }
}

#endif //KLK_LAUNCHBASE_H
