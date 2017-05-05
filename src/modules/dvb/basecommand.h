/**
   @file basecommand.h
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
   - 2009/01/17 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_BASECOMMAND_H
#define KLK_BASECOMMAND_H

#include "cli.h"
#include "stringlist.h"

namespace klk
{
    namespace dvb
    {
        /**
           @brief Base class for DVB module CLI command

           Base class for DVB module CLI command

           @ingroup grDVB
        */
        class BaseCommand : public cli::Command
        {
        public:
            /**
               Constructor

               @param[in] name - the command's name
               @param[in] summary - the summary for the command
               @param[in] usage - the usage for the command
            */
            BaseCommand(const std::string& name,
                        const std::string& summary,
                        const std::string& usage);

            /**
               Destructor
            */
            virtual ~BaseCommand(){}
        protected:
            /**
               Retrives a list with possible device names

               @return the list

               @exception @ref klk::Exception
            */
            const StringList getDevNames();

            /**
               Retrives a device by name

               @param[in] name - the dev name

               @return the dev

               @exception @ref klk::Exception
            */
            const IDevPtr getDev(const std::string& name);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            BaseCommand(const BaseCommand& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            BaseCommand& operator=(const BaseCommand& value);
        };
    }
}

#endif //KLK_BASECOMMAND_H
