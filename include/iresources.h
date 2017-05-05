/**
   @file iresources.h
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
   - 2008/10/27 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_IRESOURCES_H
#define KLK_IRESOURCES_H

#include <time.h>

#include <list>
#include <string>

#include <boost/shared_ptr.hpp>

namespace klk
{
    namespace dev
    {

        /** @defgroup grResources Resources
            @{

            Resources holders
        */

        /**
           Device state defenitions
         */
        typedef enum
        {
            IDLE = 0, ///< device in an idle state
            WORK = 1  ///< there is an activity on the device
        } State;

        /**
           Resource type
        */
        const std::string TYPE = "type";

        /**
           Resource name
        */
        const std::string NAME = "name";

        /**
           UUID key
        */
        const std::string UUID = "uuid";

        /**
           Bus UUID key
        */
        const std::string BUSUUID = "bus_uuid";

        /**
           Bandwidth for the dev
        */
        const std::string BANDWIDTH = "bandwidth";

        /**
           Data rate for the dev
        */
        const std::string RATE = "rate";
    }

    /**
       @brief The device interface

       The device info holder
    */
    class IDev
    {
    public:
        /**
           Destructor
        */
        virtual ~IDev(){}

        /**
           Retrives the dev state

           @return the state of the device
        */
        virtual klk::dev::State getState() const = 0;

        /**
           Sets device state

           @param[in] state - the state value to be set
        */
        virtual void setState(klk::dev::State state) = 0;

        /**
           Tests is there a specified parameter available or not

           @param[in] key - the key

           @return
           - true the parametr with the specified key exists
           - false the parametr with the specified key does not exist
        */
        virtual bool hasParam(const std::string& key) const = 0;

        /**
           Gets dev info by it's key

           @param[in] key - the key

           @return the parameter value

           @exception @ref klk::Exception
        */
        virtual const std::string
            getStringParam(const std::string& key) const = 0;

        /**
           Gets dev info by it's key

           @param[in] key - the key

           @return the parameter value

           @exception @ref klk::Exception
        */
        virtual const int getIntParam(const std::string& key) const = 0;

        /**
           Sets a value for a dev's parameter

           @param[in] key - the key for retriiving
           @param[in] value - the value to be set
           @param[in] mutable_flag - do update (false) the last or not (true)

           @see IDev::getIntParam
           @see IDev::getStringParam
        */
        virtual void setParam(const std::string& key,
                              const std::string& value,
                              bool mutable_flag = false) = 0;

        /**
           Sets a value for a dev's parameter

           @param[in] key - the key for retriiving
           @param[in] value - the value to be set
           @param[in] mutable_flag - do update (false) the last or not (true)

           @see IDev::getIntParam
           @see IDev::getStringParam
        */
        virtual void setParam(const std::string& key,
                              const int value,
                              bool mutable_flag = false) = 0;

        /**
           Gets last update time

           @return the time
        */
        virtual const time_t getLastUpdateTime() const = 0;

        /**
           Updates the device info (from DB for example)

           @exception @ref klk::Exception
        */
        virtual void update() = 0;

        /**
           Adds the dev info to the DB
        */
        virtual void updateDB() = 0;

        /**
           Is the 2 devs equal or not

           @param[in] dev - the second dev to be tested

           @return
           - true
           - false
        */
        virtual bool equal_to(const boost::shared_ptr<IDev>& dev) const = 0;
    };

    /**
       The smart pointer for IDev
    */
    typedef boost::shared_ptr<IDev> IDevPtr;

    /**
       The smart pointer for IDev
    */
    typedef std::list<IDevPtr> IDevList;

    /**
       The smart pointer for IResources
    */
    typedef boost::shared_ptr<class IResources> IResourcesPtr;

    /**
       Resources list defenitions
    */
    typedef std::list<IResourcesPtr> IResourcesList;

    /**
       @brief The resources interface

       The interface is used for getting all importand device info
    */
    class IResources
    {
    public:
        /**
           Destructor
        */
        virtual ~IResources(){}

        /**
           Retrives the type of the resources

           @return the type
        */
        virtual const std::string getType() const throw() = 0;

        /**
           Check is there the specified dev type

           @param[in] type - the dev's type to be checked
        */
        virtual bool hasDev(const std::string& type) const = 0;

        /**
           Gets the resource by its id

           @param[in] uuid - the resource_id

           @return the dev ptr.

           @exception klk::Exception if the resource was not found
        */
        virtual const IDevPtr
            getResourceByUUID(const std::string& uuid) const = 0;

        /**
           Gets the resource by its name

           @param[in] name - the resource's name

           @return the dev ptr


           @exception @ref klk::Exception if the resource was not found
        */
        virtual const IDevPtr
            getResourceByName(const std::string& name) const = 0;

        /**
           Gets a list of devices by it's type

           @param[in] type - the dev's type

           @return the list
        */
        virtual const IDevList
            getResourceByType(const std::string& type) const = 0;

        /**
           Adds a dev to the list

           @param[in] dev - the dev to be added

           @exception klk::Exception
        */
        virtual void addDev(const IDevPtr& dev) = 0;

        /**
           Detects resources of the specified type

           @param[in] type - the resource type

           @exception @ref klk::Exception
        */
        virtual void updateDB(const std::string& type) = 0;

        /**
           @brief Adds sub resources

           Adds a specific resources (sub type of resources)

           @param[in] resources - the sub resources to be added

           @exception klk::Exception
        */
        virtual void add(const IResourcesPtr& resources) = 0;

        /**
           @brief Checks devices

           Check devices presence and fill them with values
           from @ref grDB "DB"

           @exception klk::Exception
        */
        virtual void initDevs() = 0;
    };
    /** @} */
}

#endif //KLK_IRESOURCES_H
