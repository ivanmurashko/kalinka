/**
   @file iconfig.h
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
   - 2008/11/21 created by ipp (Ivan Murashko)
   - 2009/01/04 changed all error processing to exceptions
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ICONFIG_H
#define KLK_ICONFIG_H

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

namespace klk
{
    /** @defgroup grConfig Configuration

        The group keeps defenitions for mediaserver's config and
        configuration related interfaces.

        @ingroup grCommon

        @{
    */

    /**
       @brief Config option

       Config file option
    */
    class IOption
    {
    public:
        /**
           Destructor
        */
        virtual ~IOption(){}

        /**
           Gets option's name

           @return the option name
        */
        virtual const std::string getName() const throw() = 0;

        /**
           Gets option's summary

           @return the option summary

           @note used mainly in @ref grClient "CLI client"
        */
        virtual const std::string getSummary() const throw() = 0;

        /**
           Gets option's description

           @return the option's description

           @note used mainly in @ref grClient "CLI client"
        */
        virtual const std::string getDescription() const throw() = 0;

        /**
           Sets the option's value

           @param[in] value - the value to be set

           @exception klk::Exception
        */
        virtual void setValue(const std::string& value) = 0;

        /**
           Gets the option value

           @return the option's value
        */
        virtual const std::string getValue() const throw() = 0;
    };

    /**
       Option smart pointer
    */
    typedef boost::shared_ptr<IOption> IOptionPtr;

    /**
       Option's list
    */
    typedef std::list<IOptionPtr> OptionList;

    /**
       @brief DB connection info

       DB connection info
    */
    class IDBInfo
    {
    public:
        /**
           Destructor
        */
        virtual ~IDBInfo(){}

        /**
           Gets DB hostname

           @return the value
        */
        virtual const std::string getHost() const throw() = 0;

        /**
           Gets DB port

           @return the value
        */
        virtual u_int getPort() const throw() = 0;

        /**
           Gets DB name

           @return the value
        */
        virtual const std::string getDBName() const throw() = 0;

        /**
           Gets user account name

           @return the value
        */
        virtual const std::string getUserName() const throw() = 0;

        /**
           Gets password

           @return the value
        */
        virtual const std::string getUserPwd() const throw() = 0;
    };

    /**
       @brief SNMP related info

       info for SNMP initialization
    */
    class ISNMPInfo
    {
    public:
        /**
           Destructor
        */
        virtual ~ISNMPInfo(){}

        /**
           Gets SNMP receiver for net-snmp initialization (aka peername)

           @return the value, for example localhost, localhost:161
        */
        virtual const std::string getReceiver() const throw() = 0;

        /**
           Gets the receiver port
        */
        virtual const int getReceiverPort() const throw() = 0;

        /**
           Gets SNMP community for net-snmp initialization

           @return the value, for example public
        */
        virtual const std::string getCommunity() const throw() = 0;
    };

    /**
       @brief Configuration info

       The class provides an interface to configuartion info
    */
    class IConfig
    {
    public:
        /**
           Destructor
        */
        virtual ~IConfig(){}

        /**
           Sets config file path

           @param[in] path - the path to be set

           @exception klk::Exception there was an error
           (file doesn't exist or something else)
        */
        virtual void setPath(const std::string& path) = 0;

        /**
           Gets config file path

           @return the path
        */
        virtual const std::string getPath() const throw() = 0;

        /**
           Loads config from the file

           @exception @ref klk::Exception
        */
        virtual void load() = 0;

        /**
           Saves the config

           @exception klk::Exception
        */
        virtual void save() = 0;

        /**
           Gets mediaserver host name

           @return the value
        */
        virtual const std::string getHostName() const throw() = 0;

        /**
           Gets DB info

           @return the db info
        */
        virtual const IDBInfo* getDBInfo() const throw() = 0;

        /**
           Gets SNMP info

           @return the pointer to the SNMP info
        */
        virtual const ISNMPInfo* getSNMPInfo() const throw() = 0;

        /**
           Retrives the options list

           @return the list
        */
        virtual const OptionList getOptions() const throw() = 0;
    };
    /** @} */
}

#endif //KLK_ICONFIG_H
