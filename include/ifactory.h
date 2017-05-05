/**
   @file ifactory.h
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

#ifndef KLK_IFACTORY_H
#define KLK_IFACTORY_H

#include <string>

#include "imodule.h"
#include "imessage.h"
#include "iresources.h"
#include "isnmp.h"
#include "common.h"
#include "errors.h"
#include "iconfig.h"
#include "ithread.h"

namespace klk
{
    /** @defgroup grFactory Mediaserver's factory

        The group keeps defenitions for mediaserver's factories

        @{
    */

    /**
       @brief Messages factory

       Factory object for messages creation
    */
    class IMessageFactory
    {
    public:
        /**
           Destructor
        */
        virtual ~IMessageFactory(){}

        /**
           Creates the message by its id

           @param[in] id - the message id

           @return pointer to the message

           @exception klk::Exception - there was an error while retriving
        */
        virtual IMessagePtr getMessage(const std::string& id) = 0;

        /**
           Checks if the message register or not

           @param[in] id - the message id

           @return
           - true
           - false
        */
        virtual bool hasMessage(const std::string& id)= 0;


        /**
           Gets response message

           construct it from the input arg

           @param[in] request - the request message

           @return the response message

           @exception @ref Result - there was an error while retriving
        */
        virtual IMessagePtr getResponse(const IMessagePtr& request) const = 0;

        /**
           Register a message

           @param[in] msg_id - the message's id to be registered
           @param[in] mod_id - the module's id that do the registration
           @param[in] msg_type - the message's type

           @exception klk::Exception
        */
        virtual void registerMessage(const std::string& msg_id,
                                     const std::string& mod_id,
                                     msg::Type msg_type) = 0;

        /**
           UnRegister a message

           @param[in] msg_id - the message's id to be unregistered
           @param[in] mod_id - the module's id that do the unregistration
        */
        virtual void unregisterMessage(const std::string& msg_id,
                                       const std::string& mod_id) throw() = 0;

        /**
           Generates an uuid for a message. Each message has an unique uuid

           @note FIXME!!! is it really necessary to have it

           @return the generated uuid
        */
        virtual msg::UUID generateUUID() = 0;
    };

    /**
       @brief Module factory

       Loads/Unloads modules
    */
    class IModuleFactory
    {
    public:
        /**
           Destructor
        */
        virtual ~IModuleFactory(){}

        /**
           Loads a module by its id

           @param[in] id the module's id

           @exception klk::Exception
        */
        virtual void load(const std::string& id) = 0;

        /**
           Unloads a module by its id

           @param[in] id the module's id

           @exception klk::Exception
        */
        virtual void unload(const std::string& id) = 0;

        /**
           Unloads all loaded modules

           @exception klk::Exception
        */
        virtual void unloadAll() = 0;

        /**
           Gets module by its id

           @param[in] id the module's id

           @return the pointer to a module or NULL if there was an error

           @exception klk::Exception
        */
        virtual const IModulePtr getModule(const std::string& id) = 0;

        /**
           Gets module by its id

           @param[in] id the module's id

           @return the pointer to a module or NULL if there was an error

           @note const method

           @exception klk::Exception
        */
        virtual const IModulePtr getModule(const std::string& id) const = 0;

        /**
           Gets resources by module's id

           @param[in] id the module's id

           @return the pointer to resources or NULL if the resource
           was not found

           @exception klk::Exception
        */
        virtual const IResourcesPtr getResources(const std::string& id) = 0;

        /**
           Sends a message for processing

           @param[in] msg the pointer to the message that has to be sent

           @exception klk::Exception

           @note the message type and receivers have to be set by the caller
        */
        virtual void sendMessage(const IMessagePtr& msg) = 0;

        /**
           @brief Retrieves modules list

           Retrives all modules in the form of list

           @return the list of modules

           @note the method do the modules registration
           for all new modules and is used by CLI utility

           @note FIXME!!! remote it during #191 ticket refactoring
        */
        virtual const ModuleList getModules() = 0;

        /**
           @brief Retrieves modules list

           Retrives all modules in the form of list

           @return the list of modules

           @see getModules()

           @note the method DO NOT the modules registration
           for all new modules
        */
        virtual const ModuleList getModulesWithoutRegistration() = 0;

        /**
           Checks if the module loaded or not

           @param[in] id - the module id to be tested

           @return
           - true
           - false
        */
        virtual bool isLoaded(const std::string& id) = 0;

        /**
           Adds a dependency between modules

           @param[in] child - the child element
           @param[in] parent - the parent element

           @return
           - @ref klk::OK - the dependency was successfully added
           - @ref klk::ERROR - the dependency can not be added
        */
        virtual Result addDependency(const std::string& child,
                                     const std::string& parent) = 0;

        /**
           Removes a dependency between modules

           @param[in] child - the child element
           @param[in] parent - the parent element
        */
        virtual void rmDependency(const std::string& child,
                                  const std::string& parent) = 0;
    };

    /**
       @brief Mediaserver's factory

       The object is used for getting all mediaserver's components
    */
    class IFactory
    {
    public:
        /**
           Destructor
        */
        virtual ~IFactory(){}

        /**
           Gets an interfaces to config info

           @return the interface

           @exception klk::Exception
        */
        virtual IConfig* getConfig() = 0;

        /**
           Gets SNMP interface

           @return the interface

           @exception klk::Exception
        */
        virtual ISNMP* getSNMP() = 0;

        /**
           Gets an interfaces to resources info

           @return the interface

           @exception klk::Exception
        */
        virtual IResources* getResources() = 0;

        /**
           Gets an interfaces to resources info

           @return the interface

           @exception klk::Exception

           @note const method
        */
        virtual const IResources* getResources() const = 0;

        /**
           Gets's module factory interface

           @return the module's factory
        */
        virtual IModuleFactory* getModuleFactory() = 0;

        /**
           Gets's module factory interface

           @return the module's factory

           @note const method
        */
        virtual const IModuleFactory* getModuleFactory() const = 0;

        /**
           Gets's module factory interface

           @return the module's factory
        */
        virtual IMessageFactory* getMessageFactory() = 0;

        /**
           Gets startup/stop trigger
        */
        virtual ITrigger* getEventTrigger() = 0;

        /**
           Retrives the application module id

           Each application (even the main mediaserver) is assosiated
           with a module via klk_applications table.

           The method returns the application module id.

           @note The info is used by adapter module to setup endpoint
           info for communication with the application

           @return the module id
        */
        virtual const std::string getMainModuleId() const = 0;
    };

    /** @} */
}

extern "C"
{
    /**
       Resource factory function

       If the module library return the function this means
       that the module has specific resources
       that should be initialized

       @param[in] factory - the factory for resource usage

       @return the module itself

       @ingroup grFactory
    */
    klk::IResourcesPtr klk_resources_get(klk::IFactory *factory);

    /**
       Module factory function

       each module lib should define it

       @param[in] factory - the factory for module usage

       @return the module itself

       @ingroup grFactory
    */
    klk::IModulePtr klk_module_get(klk::IFactory *factory);

#ifdef DEBUG
    /**
       The function inits module's unit test

       @exception klk::Exception

       @ingroup grTest
    */
    void klk_module_test_init();
#endif //DEBUG
}

#endif //KLK_IFACTORY_H
