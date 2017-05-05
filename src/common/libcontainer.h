/**
   @file libcontainer.h
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
   - 2008/10/22 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_LIBCONTAINER_H
#define KLK_LIBCONTAINER_H

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>

#include "thread.h"

namespace klk
{
    /**
       @brief The class holds an opened lib

       LibContainer

       @ingroup grCommon
    */
    class LibContainer
    {
    public:
        /**
           Constructor

           @param[in] path - the path to the lib
        */
        LibContainer(const std::string& path);

        /**
           Destructor
        */
        ~LibContainer();

        /**
           Opens the lib

           @exception @ref klk::Exception
        */
        void open();

        /**
           Do dlsym call

           @param[in] fname - the function name

           @return the pointer to a function or NULL if tehre was an error
        */
        void* sym(const std::string& fname);
    private:
        std::string m_path; ///< path to the lib
        void* m_handle; ///< the lib handle
        static klk::Mutex m_lock; ///< lock mutex

        /**
           Copy constructor
        */
        LibContainer(const LibContainer&);

        /**
           The = operator
        */
        LibContainer& operator=(const LibContainer&);
    };

/**
   Shared pointer for containers
*/
    typedef boost::shared_ptr<LibContainer> LibContainerPtr;

/**
   The libs holder container (map)

   the key is the lib path
*/
    typedef std::map<std::string, LibContainerPtr> LibContainerMap;

}

#endif //KLK_LIBCONTAINER_H
