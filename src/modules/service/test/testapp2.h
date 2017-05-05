/**
   @file testapp2.h
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
   - 2020/07/25 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTAPP2_H
#define KLK_TESTAPP2_H

#include "testapp.h"

namespace klk
{
    namespace srv
    {
        namespace test
        {
            /**
               @brief First test application

               First test application

               @ingroup grSysModuleTest
            */
            class Application2 : public BaseApplication
            {
            public:
                /// Constructor
                Application2(IFactory* factory);

                /// Destructor
                virtual ~Application2(){}
            private:
                /// @copydoc klk::Module::getName
                virtual const std::string getName() const throw();
            private:
                /// Fake copy constructor
                Application2(const Application2&);

                /// Fake assigment operator
                Application2& operator=(const Application2&);
            };
        }
    }
}

#endif //KLK_TESTAPP2_H
