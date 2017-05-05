/**
   @file inthread.h
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
   - 2009/03/09 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_INTHREAD_H
#define KLK_INTHREAD_H

#include <list>

#include <boost/shared_ptr.hpp>

#include "routethread.h"
#include "reader.h"

namespace klk
{
    namespace http
    {
        /**
           @brief Input info container

           The class keeps input info

           @ingroup grHTTP
        */
        class InputInfo : public mod::Info
        {
        public:
            /**
               Constructor

               @param[in] route - the route info
            */
            InputInfo(const RouteInfoPtr& route);

            /**
               Destructor
            */
            virtual ~InputInfo(){}

            /**
               Retrives route info

               @return the route info

               @note it should always return not null smart pointer
               check for it is done inside constructor
            */
            const RouteInfoPtr getRouteInfo() const throw() {return m_route;}

            /**
               Retrives path

               @return the path
            */
            const std::string getPath() const throw(){return m_path;}

            /**
               Sets the path

               @param[in] path - the path to be set

               @exception klk::Exception
            */
            void setPath(const std::string& path);

            /**
               Retrives media type name

               @return the name
            */
            const std::string getMediaTypeName() const throw() {return m_mname;}

            /**
               Sets media type name

               @param[in] mname - the name to be set

               @exception klk::Exception
            */
            void setMediaTypeName(const std::string & mname);

            /**
               Retrives media type uuid

               @return the uuid
            */
            const std::string getMediaTypeUuid() const throw() {return m_muuid;}

            /**
               Sets media type uuid

               @param[in] muuid - the uuid to be set

               @exception klk::Exception
            */
            void setMediaTypeUuid(const std::string & muuid);
        private:
            SafeValue<RouteInfoPtr> m_route; ///< route info
            SafeValue<std::string> m_path; ///< path
            SafeValue<std::string> m_mname; ///< media type name
            SafeValue<std::string> m_muuid; ///< media type uuid
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            InputInfo(const InputInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InputInfo& operator=(const InputInfo& value);
        };

        /**
           Input info smart pointer
        */
        typedef boost::shared_ptr<InputInfo> InputInfoPtr;

        /**
           The list with route info
        */
        typedef std::list<InputInfoPtr> InputInfoList;


        /**
           @brief HTTP input thread

           The thread listen for incomming connections and produces
           connection threads

           @ingroup grHTTP
        */
        class InThread : public RouteThread
        {
            friend class TestUDP;
        public:
            /**
               Destructor
            */
            virtual ~InThread();

            /**
               Gets info
            */
            const InputInfoPtr getInfo() const throw();

            /**
               Retrives the reader

               @return the reader
            */
            const IReaderPtr getReader() const;

            /**
               Increases connection count
            */
            virtual void increaseConnectionCount();

            /**
               Decreases connection count
            */
            virtual void decreaseConnectionCount();

            /**
               Retrives connection count for displaying

               @return the count
            */
            const u_long getConnectionCount() const throw();

            /**
               Updates input info

               @param[in] info - the info to be set

               @exception klk::Exception
            */
            void updateInfo(const InputInfoPtr& info);
        protected:
            /**
               Constructor

               @param[in] factory - the http factory
               @param[in] info - the input info
            */
            InThread(Factory* factory, const InputInfoPtr& info);

            /**
               @copydoc IThread::stop()
            */
            virtual void stop() throw();

            /**
               Inits reader
            */
            virtual void initReader();

            /**
               Main loop (data processing)

               it's specific for different input threads types
            */
            virtual void doLoop() = 0;

            /**
               Do the main loop action
               wait for a data portion and send it to connection threads
            */
            void doLoopAction();
        private:
            mutable klk::Mutex m_reader_lock; ///< reader locker
            InputInfoPtr m_info; ///< input info container
            IReaderPtr m_reader; ///< the reader
            mutable klk::Mutex m_con_count_lock; ///< connection counter locker
            u_long m_con_count; ///< connection count

            /**
               @copydoc IThread::start()
            */
            virtual void start();

            /**
               Retrives rate

               @return the rate
            */
            virtual const double getRate() const;

            /**
               Resets reader
            */
            void resetReader();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InThread& operator=(const InThread& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            InThread(const InThread& value);
        };

        /**
           The input thread smart pointer
        */
        typedef boost::shared_ptr<InThread> InThreadPtr;


        /**
           @brief List with input threads

           List with input threads

           @ingroup grHTTP
        */
        class InThreadContainer : public BaseContainer
        {
            friend class TestStartup;
            friend class TestUDP;
        public:
            /**
               Constructor
            */
            InThreadContainer(Factory* factory);
            /**
               Destructor
            */
            virtual ~InThreadContainer();

            /**
               Will add info to be processed

               @param[in] info - the info to be added

               @exception klk::Exception
            */
            void addInfo(const InputInfoPtr& info);

            /**
               Will add info to be processed

               @param[in] info - the info to be deleted

               @exception klk::Exception
            */
            void delInfo(const InputInfoPtr& info);

            /**
               Retrives thread by path

               @param[in] path - the path

               @return shared pointer or NULL if was not found
            */
            const InThreadPtr getThreadByPath(const std::string& path);
        private:
            /**
               List with input threads
            */
            typedef std::list<InThreadPtr> InThreadList;

            InThreadList m_list; ///< list with input threads
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            InThreadContainer(const InThreadContainer& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InThreadContainer& operator=(const InThreadContainer& value);
        };

        /**
           Input thread container smart pointer
        */
        typedef boost::shared_ptr<InThreadContainer> InThreadContainerPtr;
    }
}

#endif //KLK_INTHREAD_H
