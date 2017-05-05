/**
   @file testapp.h
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
   - 2010/07/25 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTAPP_H
#define KLK_TESTAPP_H

#include "appmodule.h"

namespace klk
{
    namespace srv
    {
        namespace test
        {
            /**
               @brief Base application

               There is a base application class for service module unit tests
               It saves running count at klk::srv::test::RESULTDIR folder.
               The file name is equal to module name

            */
            class BaseApplication : public app::Module
            {
            public:
                /// @copydoc klk::app::Module
                BaseApplication(IFactory* factory,
                                const std::string& modid,
                                const std::string& setmsgid,
                                const std::string& showmsgid);

                /// Destructor
                virtual ~BaseApplication(){}
            private:
                /**
                   @copydoc klk::Module::preMainLoop

                   Additionaly updates runnig count at the
                   klk::srv::test::RESULTDIR folder. File name is equal to
                   module name
                */
                virtual void preMainLoop();

                /// @copydoc klk::ModuleWithDB::processDB
                virtual void processDB(const IMessagePtr& msg)
                {
                }
            private:
                /// Fake copy constructor
                BaseApplication(const BaseApplication&);

                /// Fake assigment operator
                BaseApplication& operator=(const BaseApplication&);
            };
        }
    }
}

#endif //KLK_TESTAPP_H
