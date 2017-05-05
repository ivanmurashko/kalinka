/**
   @file modulewithinfo.h
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
   - 2009/04/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MODULEWITHINFO_H
#define KLK_MODULEWITHINFO_H

#include <boost/static_assert.hpp>

#include "mod/infocontainer.h"
#include "moduledb.h"

namespace klk
{
    /**
       @brief Module with some info stored

       The typical kailinka module operates with resources and monitors
       the resource usage. Sometimes a resource can be provided to one client only
       but info about it can be gotten by any others clients.

       For instance there can be a network route that is used in TCP/IP
       connections on a server side. We should be sure that the route is
       used in one client only to prevent bind() syscall failure

       The template class implements the base logic that is necessary for such
       resource providers. The logic includes resources lock(start)/unlock(stop)

       @ingroup grModuleInfo
    */
    template <typename T> class ModuleWithInfo : public ModuleWithDB
    {
    private:
        /// the stored info smart pointer
        typedef boost::shared_ptr<T> TPtr;
    protected:
        /// the info set
        typedef typename mod::InfoContainer<T>::InfoSet InfoSet;
    public:
        /**
           Constructor

           @param[in] factory the factory for getting all necessary objects
           @param[in] id the module id
           @param[in] startid - the start info usage message id
           @param[in] stopid - the stop info usage message id

           @note the stop event will not be processed if stopid is empty
        */
        ModuleWithInfo(IFactory *factory, const std::string& id,
                       const std::string& startid,
                       const std::string& stopid = std::string()) :
        ModuleWithDB(factory, id), m_info(),
            m_startid(startid), m_stopid(stopid)
        {
            BOOST_ASSERT(m_startid.empty() == false);
            // m_stopid can be empty
            //BOOST_ASSERT(m_stopid.empty() == false);
            BOOST_ASSERT(m_startid != m_stopid);
        }

        /// Destructor
        virtual ~ModuleWithInfo(){}

        /**
           Retrives a list with values from the container

           @param[in] selector - the selector
           @param[in] type - the type

           @return the list with values
        */
        const StringList getValues(mod::InfoSelector selector,
                                   mod::InfoValueType type) const
        {
            return m_info.getValues(selector, type);
        }
    protected:
        /**
           @copydoc klk::IModule::registerProcessors

           @note It register some specific staff
           (start/stop events)

           @exception klk::Exception
        */
        virtual void registerProcessors()
        {
            registerSync(
                m_startid,
                boost::bind(&ModuleWithInfo::doStart, this, _1, _2));
            if (!m_stopid.empty())
            {
                registerSync(
                    m_stopid,
                    boost::bind(&ModuleWithInfo::doStop, this, _1, _2));
            }

            ModuleWithDB::registerProcessors();
        }

        /**
           @brief Do the start info usage

           Locks a resource

           @see doStop()

           @param[in] in - the input message
           @param[out] out - the output message

           @exception klk::Exception

           @note the start is always specific
        */
        virtual void doStart(const IMessagePtr& in,
                             const IMessagePtr& out) = 0;

        /**
           @brief Do the stop info usage

           Unlock the resource for usage by other clients

           @param[in] in - the input message
           @param[out] out - the output message

           @exception klk::Exception
        */
        virtual void doStop(const IMessagePtr& in,
                            const IMessagePtr& out)
        {
            TPtr info = getModuleInfo(in);
            if (info->isInUse() == false)
            {
                // already tuned
                throw Exception(__FILE__, __LINE__,
                                "Failed to stop usage the module info with "
                                "name '%s'", info->getName().c_str());
            }

            info->setInUse(false);
        }
    protected:
        /**
           Process changes at the DB

           @param[in] msg - the input message

           @exception @ref klk::Exception
        */
        virtual void processDB(const IMessagePtr& msg)
        {
            BOOST_ASSERT(msg);

            klk_log(KLKLOG_DEBUG, "Processing DB change event for module '%s'",
                    getName().c_str());

            InfoSet db_set = getInfoFromDB();
            InfoSet del_set = m_info.getDel(db_set);
            // delete all elements from the del lis
            m_info.del(del_set);

            // add new elements
            // and update existent
            m_info.add(db_set, true);
        }

        /**
           Do some actions before main loop

           @exception @ref klk::Exception
        */
        virtual void preMainLoop()
        {
            BOOST_ASSERT(m_info.empty() == true);
            ModuleWithDB::preMainLoop();
        }

        /**
           Do some actions after main loop
        */
        virtual void postMainLoop() throw()
        {
            m_info.clear();
            ModuleWithDB::postMainLoop();
        }

        /**
           Retrives an list with data from @ref grDB "database"

           @return list
        */
        virtual const InfoSet getInfoFromDB() = 0;

        /**
           Finds an info by another info stored at an input message

           @param[in] in - the message

           @exception klk::Exception if the info not found
        */
        const TPtr getModuleInfo(const IMessagePtr& in) const
        {
            BOOST_ASSERT(in);
            const std::string uuid = in->getValue(msg::key::MODINFOUUID);
            return m_info.getInfoByUUID(uuid);
        }
    protected:
        typename mod::InfoContainer<T> m_info; ///< info container
    private:
        const std::string m_startid; ///< start info usage msg id
        const std::string m_stopid; ///< start info usage msg id
    };
};

#endif //KLK_MODULEWITHINFO_H
