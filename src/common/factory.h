/**
   @file factory.h
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
   - 2008/09/27 created by ipp (Ivan Murashko)
   - 2009/01/04 some exceptions were added by ipp
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_FACTORY_H
#define KLK_FACTORY_H

#include "ifactory.h"
#include "thread.h"
#include "db.h"

namespace klk
{
    namespace base
    {
        /**
           @brief Mediaserver's factory

           The object is used for getting all mediaserver's components

           @ingroup grFactory
        */
        class Factory : public IFactory
        {
        public:
            /**
               Destructor
            */
            virtual ~Factory();

            /**
               Gets startup/stop trigger
            */
            virtual ITrigger* getEventTrigger() {return &m_stop;}

            /**
               @copydoc IFactory::getConfig()
            */
            virtual IConfig* getConfig();


            /**
               @copydoc IFactory::getResources()
            */
            virtual IResources* getResources();
        protected:
            /**
               Protected constructor

               @param[in] ident - The ident argument is a string
               that  is  prepended to  every log message.
            */
            Factory(const char* ident);
        protected:
            mutable klk::Mutex m_lock; ///< locker
            IModuleFactory* m_module_factory; ///< module's factory
            IMessageFactory* m_message_factory; ///< message's factory
            IResources* m_resources; ///< resources interface
        private:
            IConfig* m_config; ///< config interface
            ISNMP* m_snmp; ///< pointer to ISNMP interface
            Trigger m_stop; ///< stop trigger


            /**
               Gets's module factory interface

               @return the module's factory
            */
            virtual IModuleFactory* getModuleFactory();

            /**
               Gets's module factory interface

               @return the module's factory

               @note the const method
            */
            virtual const IModuleFactory* getModuleFactory() const;

            /**
               Gets's module factory interface

               @return the module's factory
            */
            virtual IMessageFactory* getMessageFactory();

            /// @copydoc klk::IFactory::getResources()
            virtual const IResources* getResources() const;

            /// @copydoc klk::IFactory::getSNMP()
            virtual ISNMP* getSNMP();

            /// @copydoc klk::IFactory::getMainModuleId()
            virtual const std::string getMainModuleId() const;
        };
    }
}

#endif //KLK_FACTORY_H
