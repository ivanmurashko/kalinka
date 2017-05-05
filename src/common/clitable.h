/**
   @file clitable.h
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
   - 2009/11/22 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CLITABLE_H
#define KLK_CLITABLE_H

#include "stringlist.h"

namespace klk
{
    namespace cli
    {
        /**
           @brief Represents a CLI table

           The table consists of std::string objects and can be printed
           with alignment columns

           @note the class IS NOT thread safe. Dont use one instance
           in different threads

           @ingroup grCLI
        */
        class Table
        {
        public:
            /**
               Constructor
            */
            Table();

            /**
               Destructor
            */
            ~Table();

            /**
               Adds a row to the table

               @param[in] row - the row to b eadded

               @exception klk::Exception
            */
            void addRow(const StringList& row);

            /**
               Retrives the formated represination of the table for displaying

               @return the formated string to be displayed
            */
            const std::string formatOutput() const;
        private:
            /**
               The storage container
            */
            typedef std::list<StringList> Storage;

            /**
               Container for alignment info
            */
            typedef std::list<size_t> AlignmentInfo;

            Storage m_storage; ///< storage container
            AlignmentInfo m_alignment; ///< alignment info

            /**
               Retrives a formated info from a row

               @param[in] initial - the accumulated value (result)
               @param[in] row - the row info container

               @return the formated row
            */
            const std::string formatRow(const std::string& initial,
                                        const StringList& row) const;
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Table(const Table& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Table& operator=(const Table& value);
        };
    }
}

#endif //KLK_CLITABLE_H
