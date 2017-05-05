/**
   @file httpfactory.h
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
   - 2009/03/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_HTTPFACTORY_H
#define KLK_HTTPFACTORY_H

#include <map>

#include <boost/function/function1.hpp>
#include <boost/shared_ptr.hpp>

#include "scheduler.h"
#include "inthread.h"
#include "outthread.h"
#include "conthread.h"
#include "thread.h"

namespace klk
{
    namespace http
    {
        /**
           Scheduler pointer
        */
        typedef boost::shared_ptr<base::Scheduler> SchedulerPtr;

        /**
           @brief HTTP factory

           The class is used for getting common objects

           @ingroup grHTTP
        */
        class Factory
        {
            friend class TestStartup;
            friend class TestUDP;
        public:
            /**
               Constructor
            */
            Factory();

            /**
               Destructor
            */
            virtual ~Factory();

            /**
               Inits factory usage

               @exception klk::Exception
            */
            void init();

            /**
               Stops factory usage
            */
            void stop() throw();

            /**
               Retrives pointer to scheduler

               @return pointer to the scheduler

               @exception klk::Exception
            */
            const SchedulerPtr getScheduler() const;

            /**
               Setups out rout info

               @param[in] route - the info to be set

               @exception klk::Exception
            */
            void setOutRoute(const RouteInfoPtr& route);

            /**
               Retrives ouput thread

               @return the thread
            */
            const OutThreadPtr getOutThread() const;

            /**
               Retrives stop thread

               @return the thread
            */
            const StopThreadPtr getStopThread() const;

            /**
               Retrives input thread container

               @return the container

               @exception klk::Exception
            */
            const InThreadContainerPtr getInThreadContainer() const;

            /**
               Retrives connection thread container

               @return the container

               @exception klk::Exception
            */
            const ConnectThreadContainerPtr getConnectThreadContainer() const;

            /**
               Gets reader by the media type (HTTP) uuid

               @param[in] media_type - the media type
               @param[in] sock - the socket for reading

               @return the reader

               @exception klk::Exception (for example unsupported media type)
            */
            const IReaderPtr getReader(const std::string& media_type,
                                       const klk::ISocketPtr& sock) const;

            /**
               Created an input thread from input info

               @param[in] info - the info to be used

               @return the pointer

               @note There can be several types of input threads:
               - klkhttp::InTCPThread
               - klkhttp::InUDPThread

               @exception klk::Exception (for example unsupported net protocol)
            */
            const InThreadPtr getInThread(const InputInfoPtr& info);
        private:
            /**
               Make functor
            */
            typedef boost::function1<IReaderPtr, const ISocketPtr&>
                MakeReaderFunctor;

            /**
               Reader factories storage
            */
            typedef std::map<std::string, MakeReaderFunctor> MakeReaderStorage;

            mutable klk::Mutex m_lock; ///< main locker
            klk::Trigger m_stop; ///< stop event trigger
            SchedulerPtr m_scheduler; ///< scheduler
            StopThreadPtr m_stopthread; ///< stop thread
            OutThreadPtr m_outthread; ///< out thread
            InThreadContainerPtr m_inthreads; ///< input threads list
            ConnectThreadContainerPtr m_conthreads; ///< connection threads
            MakeReaderStorage m_readers; ///< readers functor

            /**
               Clears all internal data
            */
            void clear() throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Factory(const Factory& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Factory& operator=(const Factory& value);
        };
    }
}

#endif //KLK_HTTPFACTORY_H
