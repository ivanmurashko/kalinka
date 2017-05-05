/**
   @file infocontainer.h
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
   - 2010/11/26 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_INFOCONTAINER_H
#define KLK_INFOCONTAINER_H

#include <boost/bind.hpp>

#include "info.h"
#include "exception.h"

namespace klk
{
    namespace mod
    {
        /** @addtogroup grModuleInfo
            @{
        */

        /// Info compare functor
        template <class T> struct InfoLess : public
            std::binary_function< boost::shared_ptr<T>,
                                  boost::shared_ptr<T>, bool>
        {
            /// The smart pointer for the processed type
            typedef boost::shared_ptr<T> TPtr;

            /**
               @brief Compare functor itself

               It compares 2 infos by its uuids
            */
            bool operator()(const TPtr i1, const TPtr i2)
            {
                BOOST_ASSERT(i1);
                BOOST_ASSERT(i2);
                return (std::less<std::string>()(i1->getUUID(), i2->getUUID()));
            }
        };


        /// Functor to find an info by its UUID
        template <class T = Info> struct FindInfoByUUID :
            public std::binary_function<T, std::string, bool>
        {
            /// The smart pointer for the processed type
            typedef boost::shared_ptr<T> TPtr;

            /**
               Finder itself
            */
            bool operator()(const TPtr info,
                            const std::string uuid)
            {
                BOOST_ASSERT(info);
                return (info->getUUID() == uuid);
            }
        };

        /// Helper functor to find an info by name
        struct FindInfoByName :
            public std::binary_function<InfoPtr, std::string, bool>
        {
            bool operator()(const InfoPtr info,
                            const std::string name)
            {
                BOOST_ASSERT(info);
                return (info->getName() == name);
            }
        };

        /**
           Module info selector type
        */
        typedef enum
        {
            SELECT_ALL = 0,
            SELECT_USED = 1,
            SELECT_NOT_USED = 2
        } InfoSelector;

        /**
           Module info type of value
        */
        typedef enum
        {
            NAME = 0,
            UUID = 1
        } InfoValueType;

        /**
           @brief Container with module info

           There is a wrapper class that holds a module info
           and provides an access to the info in thread-safe manner
        */
        template <typename T > class InfoContainer
        {
            friend class InfoTest;
        public:
            /// The smart pointer for the processed type
            typedef boost::shared_ptr<T> TPtr;

            /// Internal container type
            typedef std::set< TPtr, InfoLess<T> > InfoSet;

            /// The list with stored info
            typedef std::list< TPtr > InfoList;

            /// Constructor
            InfoContainer() :
            m_lock(), m_container()
            {
            }

            /// Destructor
            virtual ~InfoContainer(){}

            /// Clears the container
            void clear() throw()
            {
                Locker lock(&m_lock);
                m_container.clear();
            }

            /**
               Checks is the container empty or not

               @return
               - true
               - false
            */
            const bool empty() const throw()
            {
                Locker lock(&m_lock);
                return m_container.empty();
            }

            /// @return the size of the container
            const size_t size() const throw()
            {
                Locker lock(&m_lock);
                return m_container.size();
            }

            /**
               Retrives a list with values from the container

               @param[in] selector - the selector
               @param[in] type - the type

               @return the list with values
            */
            const StringList getValues(InfoSelector selector,
                                       InfoValueType type) const
            {
                StringList res;

                Locker lock(&m_lock);
                for (InfoSetConstIterator item = m_container.begin();
                     item != m_container.end(); item++)
                {
                    if (selector == SELECT_USED)
                    {
                        if (!(*item)->isInUse())
                            continue;
                    }
                    else if (selector == SELECT_NOT_USED)
                    {
                        if ((*item)->isInUse())
                            continue;
                    }
                    else
                    {
                        BOOST_ASSERT(selector == SELECT_ALL);
                    }

                    // selctor is ok
                    switch (type)
                    {
                    case NAME:
                        res.push_back((*item)->getName());
                        break;
                    case UUID:
                        res.push_back((*item)->getUUID());
                        break;
                    default:
                        break;
                    }
                }

                return res;
            }

            /**
               Finds an info inside the container by its UUID

               @param[in] uuid - the UUID to be used

               @return the info

               @exception klk::Exception if the info can not be found
            */
            const TPtr getInfoByUUID(const std::string& uuid) const
            {
                BOOST_ASSERT(uuid.empty() == false);
                Locker lock(&m_lock);

                InfoSetConstIterator i =
                    std::find_if(m_container.begin(), m_container.end(),
                                 boost::bind(FindInfoByUUID<T>(),
                                             _1, uuid));
                if (i == m_container.end())
                {
                    throw Exception(__FILE__, __LINE__,
                                    "Cannot find module info with uuid '%s'",
                                    uuid.c_str());
                }

                return  *i;
            }

            /**
               Checks the set at the argument and retrives set
               with elements that missing at the argument (set)

               @param[in] set - the set to be checked

               @return the set
            */
            const InfoSet getDel(const InfoSet& set) const
            {
                InfoSet result;
                Locker lock(&m_lock);
                // result are elements that are present in m_container
                // but are not in set
                std::set_difference(m_container.begin(), m_container.end(),
                                    set.begin(), set.end(),
                                    std::inserter(result, result.end()),
                                    InfoLess<T>());
                return result;
            }

            /**
               Checks the set at the argument and retrives set
               with new elements that that should be added to m_set

               @param[in] set - the set to be checked

               @return the set
            */
            const InfoSet getAdd(const InfoSet& set) const
            {
                InfoSet result;
                Locker lock(&m_lock);
                // result are elements that are present in set
                // but are not in m_container
                std::set_difference(set.begin(), set.end(),
                                    m_container.begin(), m_container.end(),
                                    std::inserter(result, result.end()),
                                    InfoLess<T>());
                return result;
            }

            /**
               Deletes entries in the argument from the set

               @param[in] set - to be checked
               @param[in] ignore_not_used - should we ignore not used elements

               @exception klk::Exception
            */
            void del(const InfoSet& set, bool ignore_not_used = true)
            {
                std::for_each(set.begin(), set.end(),
                              boost::bind(&InfoContainer::delElem,
                                          this, _1, ignore_not_used));
            }

            /**
               Deletes an element from the list

               @param[in] elem - to be checked
               @param[in] ignore_not_used - should we ignore not used elements

               @exception klk::Exception
            */
            void delElem(const TPtr& elem, bool ignore_not_used = true)
            {
                BOOST_ASSERT(elem);
                if (ignore_not_used && elem->isInUse())
                {
                    // ignore
                    klk_log(KLKLOG_DEBUG,
                            "Module info '%s' is in use and cannot be deleted.",
                            elem->getName().c_str());
                }
                else
                {
                    // remove it
                    Locker lock(&m_lock);
                    m_container.erase(elem);
                }
            }

            /**
               Adds entries in the argument from the set

               @param[in] set - to be checked
               @param[in] update - the update flag do update
               existent items(true) or not (false)

               @exception klk::Exception
            */
            void add(const InfoSet& set, bool update)
            {
                std::for_each(set.begin(), set.end(),
                              boost::bind(&InfoContainer::addElem,
                                          this, _1, update));
            }

            /**
               Adds an element

               @param[in] elem - to be checked
               @param[in] update - the update flag do update
               existent items(true) or not (false)

               @exception klk::Exception
            */
            void addElem(const TPtr& elem, bool update)
            {
                BOOST_ASSERT(elem);
                Locker lock(&m_lock);
                typename InfoSet::iterator find = m_container.find(elem);
                if (find == m_container.end())
                {
                    m_container.insert(elem);
                }
                else if (update)
                {
                    (*find)->updateInfo(elem);
                }
            }

            /**
               Retrives a list with module specific info

               @return the list
            */
            const InfoList getInfoList() const
            {
                Locker lock(&m_lock);
                return InfoList(m_container.begin(), m_container.end());
            }
        private:
            /// The info container const iterator
            typedef typename InfoSet::const_iterator InfoSetConstIterator;

            mutable Mutex m_lock; ///< locker
            InfoSet m_container; ///< container with module info
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            InfoContainer(const InfoContainer& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            InfoContainer& operator=(const InfoContainer& value);
        };

        /** @} */
    }
}

#endif //KLK_INFOCONTAINER_H
