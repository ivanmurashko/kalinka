/**
   @file testhelpmodule.h
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

#ifndef KLK_TESTHELPMODULE_H
#define KLK_TESTHELPMODULE_H

#include "moduledb.h"
#include "testmodule.h"

namespace klk
{
    namespace db
    {
        /**
           @brief Test module for DB checker

           Test module for DB checker tests

           @ingroup grCheckDBTest
        */
        class TestModule : public ModuleWithDB
        {
        public:
            /**
               Constructor
            */
            TestModule(IFactory *factory);

            /**
               Destructor
            */
            virtual ~TestModule(){}

            /**
               Retrive DB update count
            */
            const u_int getCount() const throw(){return m_count;}
        private:
            u_int m_count; ///< count

            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw()
            {return "dbchecktest";}

            /**
               @copydoc IModule::getType
            */
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Each db related module should process
               @ref klk::msg::id::CHANGEDB message

               The method will be called for the procesing

               @param[in] msg - the input message

               @exception @ref klk::Exception
            */
            virtual void processDB(const IMessagePtr& msg);
        private:
            /**
               Copy constructor
            */
            TestModule(const TestModule&);

            /**
               Assignment opearator
            */
            TestModule& operator=(const TestModule&);
        };
    }
}


#endif //KLK_TESTHELPMODULE_H
