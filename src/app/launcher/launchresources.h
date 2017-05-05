/**
   @file launchresources.h
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
   - 2010/09/18 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_LAUNCHRESOURCES_H
#define KLK_LAUNCHRESOURCES_H

#include "ifactory.h"
#include "iresources.h"

#include "adapter/resourcesprotocol.h"

namespace klk
{
    namespace launcher
    {
        /**
           @brief Launch resources

           The klk::IResources interface implementation that
           got all info about resources via RPC

           @ingroup grLauncher
        */
        class Resources : public klk::IResources
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory to get resources
            */
            Resources(klk::IFactory* factory);

            /// Destructor
            virtual ~Resources();
        private:
            klk::IFactory* m_factory; ///< the factory instance
            mutable klk::adapter::ResourcesProtocol m_proto; ///< RPC connector to resources info

            /// @copydoc klk::IResources::getType()
            virtual const std::string getType() const throw() ;

            /// @copydoc klk::IResources::hasDev()
            virtual bool hasDev(const std::string& type) const;

            /// @copydoc klk::IResources::getResourceByUUID()
            virtual const klk::IDevPtr getResourceByUUID(const std::string& uuid) const ;

            /// @copydoc klk::IResources::getResourceByName()
            virtual const klk::IDevPtr getResourceByName(const std::string& name) const;

            /// @copydoc klk::IResources::getResourceByType()
            virtual const klk::IDevList  getResourceByType(const std::string& type) const ;

            /**
               @copydoc klk::IResources::addDev()

               @note the method should not be called. The resources can be added only in the
               main application (mediaserver)
            */
            virtual void addDev(const klk::IDevPtr& dev) ;

            /// @copydoc klk::IResources::updateDB()
            virtual void updateDB(const std::string& type);

            /**
               @copydoc klk::IResources::add()

               @note the method should not be called. The resources can be added only in the
               main application (mediaserver)
            */
            virtual void add(const klk::IResourcesPtr& resources);

            /// @copydoc klk::IResources::initDevs()
            virtual void initDevs();
        private:
            /// Fake copy constructor
            Resources(const Resources&);

            /// Fake assigment operator
            Resources& operator=(const Resources&);
        };

    }
}

#endif //KLK_LAUNCHRESOURCES_H
