/**
   @file adapter.h
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
   - 2008/08/22 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ADAPTER_H
#define KLK_ADAPTER_H

#include <Ice/Ice.h>

#include <map>
#include <list>

#include "module.h"
#include "scheduler.h"

namespace klk
{
    namespace adapter
    {
        /**
           @brief The message core class implementation

           The message core class implementation

           @ingroup grAdapterModule
        */

class Adapter : public klk::Module
        {
            friend class TestAdapter;
        public:
            /**
               Constructor

               @param[in] factory the module factory
            */
            explicit Adapter(IFactory *factory);

            /// Destructor
            virtual ~Adapter();

            /**
               Register a remote message for processing

               @param[in] msg_id - the message's id to be registered
               @param[in] mod_id - the module's id that do the registration
               @param[in] msg_type - the message's type

               @exception klk::Exception
            */
            void registerRemoteMessage(const std::string& msg_id,
                                       const std::string& mod_id,
                                       msg::Type msg_type);
        private:
            /// Remote messages container
            typedef std::map< std::string, StringList > RemoteMessageMap;

            Ice::CommunicatorPtr m_communicator; ///< ice communicator pointer
            StringList m_adapters;  ///< list of initialized adapters
            RemoteMessageMap m_remote_messages; ///< remote messages
            Mutex m_remote_mutex; ///< mutex for remote messages

            /// @copydoc klk::IModule::registerProcessors
            void registerProcessors();

            ///  @copydoc klk::Module::preMainLoop
            virtual void preMainLoop();

            ///  @copydoc klk::Module::postMainLoop
            virtual void postMainLoop() throw();

            /// @copydoc klk::IModule::getName
            virtual const std::string getName() const throw();

            /// @copydoc klk::IModule::getType
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Inits the specified object adapter

               @param[in] object_id the object adapter to be initialized

               @note the valid ids are defined at @ref grAdapterModuleObjects
               "the following group"
            */
            void initObjectAdapter(const std::string& object_id);

            /**
               Updates DB settings for object adapter specified at the argument

               @param[in] object_id the object adapter to be initialized
               @param[in] connection_string - connection string to be set.
               Empty string means that NULL has to be set at the DB: there
               is no endpoint for the object.

               @note the valid ids are defined at @ref grAdapterModuleObjects
               "the following group"
            */
            void updateDB4ObjectAdapter(const std::string& object_id,
                                        const std::string& connection_string);

            /**
               Creates ICE object ptr by object identifier

               @param[in] object_id the object identifier to be used

               @return the smart pointer of the object
            */
            const Ice::ObjectPtr createObject(const std::string& object_id);

            /**
               Processor for remote sync message

               @param[in] - input message
               @param[out] - output result message
            */
            void processRemoteSync(const IMessagePtr& in, const IMessagePtr& out);

            /**
               Processor for remote async message

               @param[in] - input message
            */
            void processRemoteASync(const IMessagePtr& in);

            /**
               Retrieves remote receivers list (list of module ids) by the input message

               @param[in] input message

               @return the list
            */
            const StringList getReceiverList(const IMessagePtr& in);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Adapter(const Adapter& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Adapter& operator=(const Adapter& value);
        };
    };
};

#endif //KLK_ADAPTER_H

