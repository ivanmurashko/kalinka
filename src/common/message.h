/**
   @file message.h
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
   - 2008/09/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MESSAGE_H
#define KLK_MESSAGE_H

#include <map>
#include <string>

#include <boost/static_assert.hpp>

#include "basemessage.h"

namespace klk
{
    /**
       @brief Base class for messages

       Base class for messages

       @ingroup grModules
    */
    class Message : public BaseMessage
    {
    public:
        /**
           Constructor

           @param[in] id - the message id
           @param[in] uuid - the message UUID
        */
        Message(const std::string& id, msg::UUID uuid);

        /**
           Destructor
        */
        virtual ~Message(){}

        /**
           Gets the list with modules IDs that should get the message

           @return the list
        */
        virtual const StringList getReceiverList() const throw();

        /**
           Adds a new receiver to the list

           @param[in] id - the receiver's id
        */
        virtual void addReceiver(const std::string& id);

        /**
           Clears receiver list
        */
        virtual void clearReceiverList() throw();

        /**
           @brief Converts the message into another storage

           The idea is to provide a possibility to store message
           content into another class that is out of the
           klk::IMessage -> klk::Message hierarchy.

           We provide a default template for the convertation
           We do the template specialization in the place where
           we know concrete class we want to convert to.

           As result we can just add an additional method to the
           class by demand.

           @note we use a fake parameter to use function overloading to have
           several functions with same name but different return type

           @note the default realization will produce compile error
           it should never be called

           @return the specified the object of the specified type
        */
        template <typename T> const T convertTo(T* = NULL) const
        {
            BOOST_STATIC_ASSERT(sizeof(T) == 0);
        }
    private:
        /**
           @brief String map class

           There is a container storage for simple data:
           pairs key -> value
        */
        typedef std::map<std::string, std::string> StringMap;

        /**
           @brief StringList map class

           There is a container storage for list data:
           pairs key -> list with values
        */
        typedef std::map<std::string, StringList> StringListMap;


        StringMap m_values; ///< message specific data
        StringListMap m_lists; ///< message specific data
        StringList m_receivers; ///< receivers list

        /**
           Checks message specific data

           @param[in] key - the key name (data id)

           @return
           - true - the key keeps data
           - false - no data for the key
        */
        virtual bool hasValue(const std::string& key) const;

        /**
           Gets message specific data (simple value)

           @param[in] key - the key name (data id)

           @return the data
        */
        virtual const std::string getValue(const std::string& key) const;

        /**
           Gets message specific data (list)

           @param[in] key - the key name (data id)

           @return the data
        */
        virtual const StringList getList(const std::string& key) const;


        /**
           Sets message specific data (simple value)

           @param[in] data - the data to be set
           @param[in] key - the key name (data id)
        */
        virtual void setData(const std::string& key, const std::string& data);

        /**
           Sets message specific data (list data)

           @param[in] data - the data to be set
           @param[in] key - the key name (data id)
        */
        virtual void setData(const std::string& key, const StringList& data);
    private:
        /**
           Copy constructor
        */
        Message(const Message&);

        /**
           Assignment opearator
        */
        Message& operator=(const Message&);
    };

};

#endif //KLK_MESSAGE_H
