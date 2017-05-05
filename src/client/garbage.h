/**
   @file garbage.h
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
   - 2009/05/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_GARBAGE_H
#define KLK_GARBAGE_H

#include <list>
#include <string>

#include "clidriver.h"

namespace klk
{
    namespace cli
    {
        /**
           @brief Garbage collector

           Garbage collector and memory allocator

           @ingroup grClient
        */
        class Garbage
        {
        public:
            /**
               Constructor
            */
            Garbage();

            /**
               Destructor
            */
            virtual ~Garbage();

            /**
               Allocs data for usage

               @param[in] data - the data to be cloned

               @return the c data

               @exception klk::Exception
            */
            char* allocData(const std::string& data);

            /**
               Unregister an entry

               @param[in] entry - the entry to be unregistered

               @exception klk::Exception
            */
            void freeEntry(struct ast_cli_entry* entry);

            /**
               Allocates an entry and fills it with zeros

               @exception klk::Exception
            */
            struct ast_cli_entry* allocEntry();
        private:
            /**
               The list for garbage
            */
            typedef std::list<void*> GarbageCollectorList;

            GarbageCollectorList m_garbage; ///< garbage collector - FIXME!!!
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Garbage(const Garbage& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Garbage& operator=(const Garbage& value);
        };
    }
}

#endif //KLK_GARBAGE_H
