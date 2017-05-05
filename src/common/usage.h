/**
   @file usage.h
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
   - 2009/05/20 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_USAGE_H
#define KLK_USAGE_H

#include <sys/time.h>
#include <sys/resource.h>

#include <boost/shared_ptr.hpp>

#include "thread.h"

namespace klk
{
    /**
       @brief Usage info collector

       The class collects info about
       system resources usage

       @ingroup grCommon
    */
    class Usage : public Mutex
    {
    public:
        /**
           Constructor

           @param[in] who - the who parameter from getrusage(2)
        */
        Usage(int who);

        /**
           Default constructor

           used when who usage is impossible
        */
        Usage();

        /**
           Destructor
        */
        virtual ~Usage();

        /**
           Retrive CPU usage info in the following interval
           (0, 1)

           @return the CPU usage
        */
        const double getCPUUsage();
    private:
        int m_who; ///< who param
        struct timeval m_start_time; ///< start collection time
        struct rusage m_last_usage; ///< last usage info

        /**
           Helper function to convert timeval to double

           @param[in] tv - the value to be converted

           @return the converted value

           @note FIXME!!! move to base::Utils
        */
        static const double timeval2Double(const struct timeval& tv);

        /**
           Inits usage to use

           @exception klk::Exception
        */
        void init();
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        Usage(const Usage& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        Usage& operator=(const Usage& value);
    };

    /**
       Usage smart pointer
    */
    typedef boost::shared_ptr<Usage> UsagePtr;
};

#endif //KLK_USAGE_H
