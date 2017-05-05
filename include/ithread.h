/**
   @file ithread.h
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
   - 2008/10/25 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ITHREAD_H
#define KLK_ITHREAD_H

#include <boost/shared_ptr.hpp>

namespace klk
{
    /**
       Thread smart pointer
    */
    typedef boost::shared_ptr<class IThread> IThreadPtr;

    /**
       @brief The base class for all threads

       The base class for all threads

       @ingroup grCommon
    */
    class IThread
    {
    public:
        /**
           Destructor
        */
        virtual ~IThread(){}

        /**
           Do init before startup

           @exception klk::Exception - the init failed
        */
        virtual void init() = 0;

        /**
           Starts the thread

           @exception klk::Exception - there was an error at
           the thread startup (execution)

           @callgraph
        */
        virtual void start() = 0;

        /**
           Stops the thread

           @callgraph
        */
        virtual void stop() throw() = 0;

        /**
           Check if the thread stopped

           @return
           - true - the thread has been stopped
           - false - the thread is still active

           @exception klk::Exception - there was an error during check
        */
        virtual bool isStopped() = 0;
    };

    /**
       @brief The event trigger

       The event trigger

       @ingroup grCommon
    */
    class ITrigger
    {
    public:
        /**
           Destructor
        */
        virtual ~ITrigger(){}

        /**
           Is it stopped or not

           @return
           - true - the module has to be stopped
           - false - the module not need to be stopped

           @exception @ref klk::Exception - there was an error during check
        */
        virtual bool isStopped() = 0;

        /**
           Do the stop for the trigger
        */
        virtual void stop() throw() = 0;

        /**
           Wait for the stop event

           @exception @ref klk::Exception - there was an error during check
        */
        virtual void wait() = 0;
    };
}

#endif //KLK_ITHREAD_H
