/**
   @file stopthread.h
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
   - 2009/03/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STOPTHREAD_H
#define KLK_STOPTHREAD_H

#include <boost/shared_ptr.hpp>

#include "thread.h"
#include "conthread.h"
#include "safelist.h"

namespace klk
{
    namespace http
    {
        /**
           @brief Stop thread

           The thread handles stop events from anothers threads

           @ingroup grHTTP
        */
        class StopThread : public Thread
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            StopThread(Factory* factory);

            /**
               Destructor
            */
            virtual ~StopThread();

            /**
               Adds a stop thread

               @param[in] thread - the thread to be added
            */
            void addConnectThread(const ConnectThreadPtr& thread);
        private:
            /**
               Data safe list
            */
            typedef SafeList<ConnectThreadPtr> DataList;

            DataList m_data; ///< data list
            klk::Event m_wait; ///< wait event

            /**
               @copydoc IThread::start()
            */
            virtual void start();

            /**
               @copydoc IThread::stop()
            */
            virtual void stop() throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            StopThread(const StopThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            StopThread& operator=(const StopThread& value);
        };

        /**
           Stop thread smart pointer
        */
        typedef boost::shared_ptr<StopThread> StopThreadPtr;
    }
}

#endif //KLK_STOPTHREAD_H
