/**
   @file streamerutils.h
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
   - 2009/08/02 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STREAMERUTILS_H
#define KLK_STREAMERUTILS_H

#include <string>

#include "ifactory.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {
            /**
               @brief Several DVB streamer utils

               Several DVB streamer utils

               @ingroup grStream
            */
            class Utils
            {
            public:
                /**
                   Constructor

                   @param[in] factory
                */
                Utils(IFactory* factory);

                /**
                   Destructor
                */
                ~Utils(){}

                /**
                   Adds a station with specified name

                   @param[in] station - the station name to be added
                   @param[in] route - the route name to be assigned
                   @param[in] priority

                   @exception klk::Exception
                */
                void addStation(const std::string& station,
                                const std::string& route,
                                const char* priority = NULL);

                /**
                   Deletes a station with specified name

                   @param[in] station - the station name to be deleted

                   @exception klk::Exception
                */
                void delStation(const std::string& station);
            private:
                IFactory* const m_factory; ///< factory
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                Utils(const Utils& value);


                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                Utils& operator=(const Utils& value);
            };
        }
    }
}

#endif //KLK_STREAMERUTILS_H
