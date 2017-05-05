/**
   @file testapp.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>

#include "testapp.h"
#include "utils.h"
#include "testdefines.h"

using namespace klk;
using namespace klk::srv::test;

//
// BaseApplication class
//

/// @copydoc klk::app::Module
BaseApplication::BaseApplication(IFactory* factory,
                                 const std::string& modid,
                                 const std::string& setmsgid,
                                 const std::string& showmsgid) :
    klk::app::Module(factory, modid, setmsgid, showmsgid)
{
}

// Updates runnig count at the
// klk::srv::test::RESULTDIR folder. File name is equal to
// module name
void BaseApplication::preMainLoop()
{
    app::Module::preMainLoop();

    // note: mkdir should be done in klk::srv::TestService class
    BOOST_ASSERT(base::Utils::isDir(RESULTDIR));

    int count = 0;
    const std::string file = RESULTDIR + "/" + getName();
    if (base::Utils::fileExist(file))
    {
        BinaryData data = base::Utils::readWholeDataFromFile(file);
        count = boost::lexical_cast<int>(data.toString());
    }
    count++;
    // save result
    BinaryData data(boost::lexical_cast<std::string>(count));
    // remove prev data FIXME!!! bad code
    base::Utils::unlink(file);
    // write the data to the file
    base::Utils::saveData2File(file, data);
}
