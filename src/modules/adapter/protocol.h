/**
   @file protocol.h
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
   - 2008/11/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2010/04/27 ICE by ZeroC is used as IPC framework
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ADAPTER_PROTOCOL_H
#define KLK_ADAPTER_PROTOCOL_H

#include <Ice/Ice.h>

#include "errors.h"
#include "ifactory.h"
#include "db.h"

namespace klk
{
    namespace adapter
    {
        /**
           @brief Protocol for @ref Adapter

           Protocol for @ref Adapter.
           There is a base class for all other protocols.
           The class provides common method to do RPC. In othere words
           there is an RPC wrapper.

           @ingroup grAdapterModuleObjects
        */
        template <typename T> class Protocol
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory instance
               @param[in] object_id - the object id to be used for the protocol
               @param[in] main_module_id - the id of main module. The default
               module is klk::MODULE_COMMON_ID but sometimes
               we need to send a message to a not main application
            */
            Protocol(IFactory* factory, const std::string& object_id,
                     const std::string& main_module_id) :
            m_proxy(NULL), m_communicator(NULL), m_factory(factory)
            {
                BOOST_ASSERT(m_factory);

                // retrive endpoint information
                // from DB
                db::DB db(m_factory);
                db.connect();

                db::Parameters params;
                params.add("@host", db.getHostUUID());
                params.add("@module", main_module_id);
                params.add("@object", object_id);
                params.add("@endpoint");
                params.add("@return_value");

                db::Result res = db.callSimple("klk_adapter_get", params);
                if (res["@return_value"].toInt() != 0)
                {
                    throw Exception(__FILE__, __LINE__,
                                    "DB error while adapter setting read "
                                    "for object '%s'", object_id.c_str());
                }

                if (res["@endpoint"].isNull() == true)
                {
                    throw Exception(__FILE__, __LINE__,
                                    "Could not get endpoint for '%s' object",
                                    object_id.c_str());
                }

                const std::string endpoint = res["@endpoint"].toString();

                // Init ice communicator
                int argc = 0;
                char* argv[] = {NULL};
                m_communicator = Ice::initialize(argc, argv);

                m_communicator->getProperties()->setProperty(
                    "Ice.ThreadPool.Client.Size", "5");
                m_communicator->getProperties()->setProperty(
                    "Ice.ThreadPool.Client.SizeMax", "10");

                try
                {
                    Ice::ObjectPrx base_proxy =
                        m_communicator->stringToProxy(endpoint);
                    // retrive Ice object
                    m_proxy = T::checkedCast(base_proxy);
                    if (!m_proxy)
                    {
                        throw Exception(__FILE__, __LINE__,
                                        "Invalid ICE proxy");
                    }
               }
                catch (const Ice::Exception& err)
                {
                    destroy();
                    throw Exception(__FILE__, __LINE__,
                                    "ICE exception: %s",
                                    err.what());
                }
                catch (const char* msg)
                {
                    destroy();
                    BOOST_ASSERT(msg);
                    throw Exception(__FILE__, __LINE__,
                                    "ICE exception: %s",
                                    msg);
                }
                catch(...)
                {
                    destroy();
                    throw;
                }
            }


            /// Destructor
            virtual ~Protocol()
            {
                destroy();
            }

            /**
               Checks connection

               @return
               - @ref klk::OK - the connection to the object is established
               - @ref klk::ERROR - no connection top the object

               @exception klk::Exception
            */
            Result checkConnection()
            {
                if (!m_proxy)
                {
                    // not initialized
                    return klk::ERROR;
                }

                /*
                  From Ice doc
                  4.14 Operations on Object
                  All interfaces support the ice_ping operation.
                  That operation is useful for debugging because it
                  provides a basic reachability test for an object:
                  if the object exists and a message can successfully
                  be dispatched to the object, ice_ping simply returns
                  without error. If the object cannot be reached or
                  does not exist, ice_ping throws a run-time exception
                  that provides the reason for the failure.
                */

                try
                {
                    m_proxy->ice_ping();
                }
                catch (const Ice::Exception& err)
                {
                    // check connection failed
                    return ERROR;
                }

                return OK;
            }
        protected:
            T m_proxy; ///< the proxy object
        private:
            Ice::CommunicatorPtr m_communicator; ///< ice communicator pointer
            IFactory* const m_factory; /// main factory

            /// Destroys proxy object
            void destroy() throw()
            {
                m_proxy = NULL;

                if (m_communicator)
                {
                    // communicator keeps an internal ref counter
                    // thus it's safe to destroy it
                    try
                    {
                        m_communicator->destroy();
                    }
                    catch (const Ice::Exception& err)
                    {
                        klk_log(KLKLOG_ERROR, "Error while ICE cleanup: %s",
                                err.what());
                    }
                    catch (...)
                    {
                        klk_log(KLKLOG_ERROR,
                                "Error while ICE cleanup: unknown");
                    }
                    m_communicator = NULL;
                }
            }
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Protocol(const Protocol& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Protocol& operator=(const Protocol& value);
        };
    };
};

#endif //KLK_ADAPTER_PROTOCOL_H
