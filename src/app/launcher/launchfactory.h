/**
   @file launchfactory.h
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
   - 2010/07/18 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_LAUNCHFACTORY_H
#define KLK_LAUNCHFACTORY_H

#include "factory.h"
#include "launcher.h"

namespace klk
{
    namespace launcher
    {
        /**
           @brief The Launcher factory

           The factory is used for launcher object creation
           It uses RPC to retrive info from main factory that
           is used by server application

           @note pattern singleton

           @ingroup grLauncher
        */
        class Factory : public base::Factory
        {
        public:
            /// Destructor
            virtual ~Factory();

            /**
               Creates the factory instance

               @param[in] appname - the application name to be used
               for logger initialization

               @param[in] module_id - the application main module id

               @note the object should be created before the call
               the klk::Exception will be thrown if the object
               has not been initialized

               @return the factory instance
             */
            static Factory* instance(const std::string& appname,
                                     const std::string& module_id);

            /**
               Creates the factory instance

               @param[in] appname - the application name to be used for logger initialization

               @return the factory instance
             */
            static Factory* instance();

            /// Frees allocated resources
            static void destroy();

            /**
               Creates the launcher application

               @return the pointer to the created application instance
            */
            Launcher* getLauncher()
            {
                return &m_launcher;
            }

            /**
               @copydoc klk::IFactory::getResources

               @note return a klk::launch::Resources instance that retrive resource
               info via an RPC
            */
            virtual IResources* getResources();

            /**
               @copydoc klk::IFactory::getModuleFactory

               @note return a klk::launch::ModuleFactory instance that retrive module
               info via an RPC
            */
            virtual IModuleFactory* getModuleFactory();

            /**
               @copydoc klk::IFactory::getMessageFactory

               @note return a klk::launch::MessageFactory instance that retrive message
               info via an RPC
            */
            virtual IMessageFactory* getMessageFactory();
        private:
            /**
               Constructor

               @param[in] appname - the application name to be used
               for logger initialization

               @param[in] module_id - the application main module id
            */
            Factory(const std::string& appname, const std::string& module_id);

            /// @copydoc klk::IFactory::getMainModuleId()
            virtual const std::string getMainModuleId() const
            {
                return m_module_id;
            }


            static Factory* m_instance; ///< the factory instance

            const std::string m_module_id; ///< the application module id
            Launcher m_launcher; ///< launcher application
        private:
            /// Fake copy constructor
            Factory(const Factory&);

            /// Fake assigment operator
            Factory& operator=(const Factory&);
        };
    }
}

#endif //KLK_LAUNCHFACTORY_H
