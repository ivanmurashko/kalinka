/**
   @file safelist.h
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
   - 2009/03/11 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SAFELIST_H
#define KLK_SAFELIST_H

#include <list>
#include <numeric>

#include "thread.h"

namespace klk
{
    namespace http
    {
        /**
           @brief The safe list wrapper

           The safe list

           @ingroup grHTTP
        */
        template<class T> class SafeList
        {
        public:
            /**
               Constructor
            */
        SafeList() : m_lock(), m_list()
            {
            }

            /**
               Destructor
            */
            virtual ~SafeList(){}

            /**
               Safely adds an element
            */
            void push_back(const T& t)
            {
                klk::Locker lock(&m_lock);
                m_list.push_back(t);
            }

            /**
               Tests is the list empty or not
            */
            const bool empty() const throw()
            {
                klk::Locker lock(&m_lock);
                return m_list.empty();
            }

            /**
               Retrive an element from pop
            */
            const T retrive_front()
            {
                klk::Locker lock(&m_lock);
                const T t = m_list.front();
                m_list.pop_front();
                return t;
            }

            /**
               Accumulator class
            */
            struct Accumulator
            {
                size_t operator() (size_t initial,
                                   T element) {
                    return initial + element.size();
                }
            };

            /**
               Retrieves the total data size stored at the list

               @return the sum of sizes for all elements
            */
            size_t size()
            {
                klk::Locker lock(&m_lock);
                return std::accumulate(
                    m_list.begin(),
                    m_list.end(), 0,
                    Accumulator());
            }
        private:
            mutable klk::Mutex m_lock; ///< locker
            std::list<T> m_list; ///< list with data
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SafeList(const SafeList& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SafeList& operator=(const SafeList& value);
        };
    }
}
#endif //KLK_SAFELIST_H
