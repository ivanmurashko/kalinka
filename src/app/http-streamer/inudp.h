/**
   @file inudp.h
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
   - 2009/05/22 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_INUDP_H
#define KLK_INUDP_H

#include "inthread.h"

namespace klk
{
    namespace http
    {
        /**
           @brief Input thread that accepts UDP connections

           The class implemets input thread that accepts
           UDP connections

           @ingroup grHTTP

        */
        class InUDPThread : public InThread
        {
        public:
            /**
               Constructor

               @param[in] factory - the http factory
               @param[in] info - the input info container
            */
            InUDPThread(Factory* factory, const InputInfoPtr& info);

            /**
               Destructor
            */
            virtual ~InUDPThread();
        private:
            klk::Semaphore m_sem; ///< the event
            klk::Mutex m_init_lock; ///< reader init locker

            /**
               @copydoc IThread::stop()
            */
            virtual void stop() throw();

            /**
               @copydoc IThread::init()
            */
            virtual void init();

            /**
               Main loop (data processing)
               for UDP input connections

               It looks for output connections and does not read anything
               until we don't have any output (connection thread)
               for the input
            */
            virtual void doLoop();

            /**
               Inits reader for UDP connections
            */
            virtual void initReader();

            /**
               Increases connection count

               It also does some IPC with klkhttp::ConnectThread
            */
            virtual void increaseConnectionCount();

            /**
               Decreases connection count

               It also does some IPC with klkhttp::ConnectThread
            */
            virtual void decreaseConnectionCount();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            InUDPThread(const InUDPThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InUDPThread& operator=(const InUDPThread& value);
        };
    }
}


#endif //KLK_INUDP_H
