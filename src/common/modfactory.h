/**
   @file modfactory.h
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
   - 2008/07/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MODFACTORY_H
#define KLK_MODFACTORY_H

#include <map>
#include <string>

#include "ifactory.h"
#include "scheduler.h"
#include "libcontainer.h"
#include "mod/dep.h"


namespace klk
{
    /** @defgroup grModuleFactory Module factory

        The mediaserver module factory

        @ingroup grModule

        @{
    */

    /**
       @brief The helper class for ModuleFactory

       The class gets module by its id from a shared library
    */
    class LibFactory : public Mutex
    {
    public:
        /**
           Constructor
        */
        LibFactory(IFactory *factory);

        /**
           Destructor
        */
        ~LibFactory();

        /**
           Gets module by its id
        */
        const IModulePtr getModule(const std::string& id);

        /**
           Gets resources by modules id
        */
        const IResourcesPtr getResources(const std::string& id);
    private:
        IFactory * const m_factory; ///< module factory
        LibContainerMap m_handles; ///< the libs handles

        /**
           Gets module lib path by the module id

           @param[in] id - the module id

           @exception klk::Result there was an error during the path retriving

           @return the path
        */
        std::string getPath(const std::string& id) const;


        /**
           Gets make function pointer by the module id

           @param[in] id - the module id
           @param[in] signature - the function signature(name)

           @exception klk::Exception there was an error during the lib retriving

           @return the path
        */
        void* getMakeFun(const std::string& id,
                         const std::string& signature);


        /**
           Gets module by its id from a modules description

           @param[in] id the module's id

           @exception klk::Result there was an error during the path retriving

           @return the path
        */
        std::string getPathFromDescription(const std::string& id)
            const;

        /**
           Gets module by its id from a modules description

           @param[in] id the module's id
           @param[in] fname - the file name for test

           @exception klk::Result there was an error during the path retriving

           @return the pointer to a module or NULL if there was an error
        */
        std::string getPathFromDescription(const std::string& id,
                                           const std::string& fname)  const;
    };

    /**
       @brief IModuleFactory impl.

       module factory implementation

       @ingroup grModule
    */
    class ModuleFactory : public IModuleFactory
    {
    public:
        /**
           Constructor

           @param[in] factory - the main factory for getting
           all necessary objects
        */
        ModuleFactory(IFactory *factory);

        /**
           Destructor
        */
        virtual ~ModuleFactory();

        /// @copydoc klk::IModuleFactory::load
        virtual void load(const std::string& id);

        /// @copydoc klk::IModuleFactory::unload
        virtual void unload(const std::string& id);

        /// @copydoc IModuleFactory::unloadAll
        virtual void unloadAll();

        /**
           Gets module by its id

           @param[in] id the module's id

           @return the pointer to a module or NULL if there was an error
        */
        virtual const IModulePtr getModule(const std::string& id);

        /**
           Gets module by its id

           @exception @ref klk::Exception
        */
        virtual const IModulePtr getModule(const std::string& id) const;

        /**
           Gets resources by module's id

           @param[in] id the module's id

           @return the pointer to resources or NULL
           if the resource was not found

           @exception @ref klk::Exception
        */
        virtual const IResourcesPtr getResources(const std::string& id);


        /// @copydoc IModuleFactory::sendMessage()
        virtual  void sendMessage(const IMessagePtr& msg);

        /// @copydoc klk::IModuleFactory::isLoaded
        virtual bool isLoaded(const std::string& id);
    protected:
        mutable klk::Mutex m_lock; // locker
        ModuleList m_modules; ///< list of known modules

        /**
           Resources map
        */
        typedef std::map<std::string, IResourcesPtr> ResourcesMap;

        ResourcesMap m_resources; ///< map with known resources

        /**
           Adds a module to the known modules list

           @param[in] module the module to be added

           @note - there is an internal method that does not do any locks

           @exception @ref klk::Exception
        */
        void addModule(const IModulePtr& module);

        /**
           Adds resources to the known resources list

           @param[in] modid - the module's id
           @param[in] res - the resources to be added

           @note - there is an internal method that does not do any locks

           @exception @ref klk::Exception
        */
        void addResources(const std::string& modid,
                          const IResourcesPtr& res);

        /**
           Gets scheduler when it's necessary

           @note use it instead of m_scheduler direct usage
        */
        base::Scheduler* getScheduler();
    private:
        IFactory * const m_factory; ///< main factory
        LibFactory *m_libfactory; ///< libfactory
        base::Scheduler *m_scheduler; ///< the module scheduler
        mod::Dependency m_dependency; ///< the dependency

        /// @copydoc klk::IModuleFactory::getModules
        virtual const ModuleList getModules();

        /// @copydoc klk::IModuleFactory::getModulesWithoutRegistration
        virtual const ModuleList getModulesWithoutRegistration();

        /**
           Gets libfactory when it's necessary

           @note use it instead of _libfactory direct usage
        */
        LibFactory* getLibFactory();

        /**
           Gets module by its id

           @param[in] id the module's id

           @return the pointer to a module or NULL if there was an error

           @note - there is an internal method that does not do any locks
        */
        IModulePtr getModuleUnsafe(const std::string& id);

        /**
           Gets resources by module's id

           @param[in] id the module's id

           @return the pointer to  resources or NULL
           if the resources can not be found

           @note - there is an internal method that does not do any locks
        */
        IResourcesPtr getResourcesUnsafe(const std::string& id);

        /**
           Adds a dependency between modules

           @param[in] child - the child element
           @param[in] parent - the parent element

           @return
           - @ref klk::OK - the dependency was successfully added
           - @ref klk::ERROR - the dependency can not be added
        */
        virtual klk::Result addDependency(const std::string& child,
                                         const std::string& parent);

        /**
           Removes a dependency between modules

           @param[in] child - the child element
           @param[in] parent - the parent element
        */
        virtual void rmDependency(const std::string& child,
                                  const std::string& parent);


        /**
           @brief Gets a module instance by its id

           There is an internal method to retrieve (or create) module instance
           during startup procedure

           @param[in] id - the module id

           @return the module instance
        */
        const IModulePtr getModule4Start(const std::string& id);

        /**
           Starts module thread

           @param[in] module - the module
        */
        void startModuleThread(const IModulePtr& module);

        /**
           Retrieves a list with available module ids

           The list is gotten from XML files with modules descriptions

           @return the list with ids
        */
        static StringList getModuleIDs();
    };

    /** @} */

}

#endif //KLK_MODFACTORY_H
