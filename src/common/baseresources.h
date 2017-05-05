/**
   @file baseresources.h
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
   - 2008/09/05 created by ipp (Ivan Murashko)
   - 2009/03/02 Resources->BaseResources by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_BASERESOURCES_H
#define KLK_BASERESOURCES_H

#include <sys/types.h>

#include <list>

#include "dev.h"
#include "ifactory.h"
#include "thread.h"

namespace klk
{
    namespace dev
    {
        /**
           @brief Base class for resources info

           The class keeps info about resources

           @ingroup grResources
        */
        class BaseResources : public IResources
        {
            friend class ResourcesTest;
        public:
            /**
               Constructor

               @param[in] factory - the factory object
               @param[in] type - the resources type
            */
            explicit BaseResources(IFactory* factory,
                                   const std::string& type);

            /**
               Destructor
            */
            virtual ~BaseResources();

            /**
               Check is there the specified dev type

               @param[in] type - the dev's typ[e to be checked
            */
            virtual bool hasDev(const std::string& type) const;

            /**
               Gets the resource by its id

               @param[in] uuid - the resource_id

               @return the dev ptr. The pointer is empty
               if the resource was not found

               @exception klk::Exception
            */
            virtual const IDevPtr getResourceByUUID(const std::string& uuid)
                const;

            /**
               Gets the resource by its name

               @param[in] name - the resource's name

               @return the dev ptr. The pointer is empty
               if the resource was not found

               @exception klk::Exception
            */
            virtual const IDevPtr getResourceByName(const std::string& name)
                const;

            /**
               Gets a list of devices by it's type

               @param[in] type - the dev's type

               @return the list
            */
            virtual const IDevList getResourceByType(const std::string& type)
                const;

            /**
               Adds a dev to the list

               @param[in] dev - the dev to be added

               @exception klk::Exception
            */
            virtual void addDev(const IDevPtr& dev);

            /**
               @copydoc klk::IResources::updateDB()
            */
            virtual void updateDB(const std::string& type);

            /**
               @copydoc klk::IResources::add()
            */
            virtual void add(const IResourcesPtr& resources);

            /**
               Retrives the type of the resources

               @return the type
            */
            virtual const std::string getType() const throw(){return m_type;}
        protected:
            mutable  Mutex m_lock; ///< the locker
            IFactory* const m_factory; ///< factory

            /**
               Retrives dev list

               @return the dev list
            */
            const IDevList getDevList() const;

            /**
               Retrives sub resources list

               @return the dev list
            */
            const IResourcesList getResourcesList() const;

            /**
               Clears internal info
            */
            void clear() throw();
        private:
            std::string m_type; ///< type of resources that the container holds
            IDevList m_devlist; ///< list with dev info
            IResourcesList m_reslist; ///< list with sub resources info

            /**
               Removes missing resources from the DB

               @param[in] bus_uuid - the bus's uuid
               @param[in] type - the resource type

               @exception @ref klk::Exception
            */
            void removeMissing(const std::string& bus_uuid,
                               const std::string& type);
        private:
            /**
               Copy constructor
            */
            BaseResources(const BaseResources&);

            /**
               Assignment opearator
            */
            BaseResources& operator=(const BaseResources&);
        };
    }
}

#endif //KLK_BASERESOURCES_H
