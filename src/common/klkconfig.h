/**
   @file klkconfig.h
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
   - 2008/08/30 created by ipp (Ivan Murashko)
   - 2009/01/04 changed all error processing to exceptions
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2011/01/01 file name was changed config.h =>> klkconfig.h by ipp
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_KLKCONFIG_H
#define KLK_KLKCONFIG_H

#include <sys/types.h>

#include <string>

#include <boost/function/function1.hpp>

#include "iconfig.h"
#include "thread.h"

namespace klk
{
    /**
       Config option processor

       The processor should take an option value as an argument
       and set it into config

       @ingroup grConfig
    */
    typedef boost::function1<void,
        const std::string&> OptionProcessor;

    /**
       @brief IOption impl.

       IOption interface realization

       @ingroup grConfig
    */
    class Option : public IOption
    {
    public:
        /**
           Constructor
        */
        Option(const std::string& name,
               const std::string& summary,
               const std::string& description,
               const std::string& defvalue,
               OptionProcessor processor);

        /**
           Destructor
        */
        virtual ~Option(){}
    private:
        std::string m_name; ///< option's name
        std::string m_summary; ///< option's summary
        std::string m_description; ///< option's description
        std::string m_value; ///< value
        OptionProcessor m_processor; ///< processor

        /**
           Gets option's name

           @return the option name
        */
        virtual const std::string getName()
            const throw() {return m_name;}

        /**
           Gets option's summary

           @return the option summary

           @note used mainly in @ref grClient "CLI client"
        */
        virtual const std::string getSummary()
            const throw() {return m_summary;}

        /**
           Gets option's description

           @return the option's description

           @note used mainly in @ref grClient "CLI client"
        */
        virtual const std::string getDescription()
            const throw() {return m_description;}

        /**
           @copydoc IOption::setValue()
        */
        virtual void setValue(const std::string& value);

        /**
           Gets the option value

           @return the option's value
        */
        virtual const std::string getValue()
            const throw() {return m_value;}
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        Option(const Option& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        Option& operator=(const Option& value);
    };

    /**
       @brief The DB info

       The DB info

       @ingroup grConfig
    */
    class DBInfo : public IDBInfo
    {
    public:
        /**
           Constructor
        */
        DBInfo();

        /**
           Destructor
        */
        virtual ~DBInfo(){}

        /**
           Sets db addr option values

           @param[in] opt - the data to be parsed and set

           @exception @ref klk::Exception
        */
        void setDBAddr(const std::string& opt);
    private:
        std::string m_host; ///< db host name
        std::string m_dbname; ///< db name
        std::string m_user; ///< db user account name
        std::string m_pwd; ///< db user account password
        u_int m_port; ///< db port

        /**
           Gets DB hostname

           @return the value
        */
        virtual const std::string getHost() const throw();

        /**
           Gets DB port

           @return the value
        */
        virtual u_int getPort() const throw();

        /**
           Gets DB name

           @return the value
        */
        virtual const std::string getDBName() const throw();

        /**
           Gets user account name

           @return the value
        */
        virtual const std::string getUserName() const throw();

        /**
           Gets password

           @return the value
        */
        virtual const std::string getUserPwd() const throw();
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        DBInfo(const DBInfo& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        DBInfo& operator=(const DBInfo& value);
    };

    /**
       @brief SNMP info

       SNMP info

       @ingroup grConfig
    */
    class SNMPInfo : public ISNMPInfo
    {
    public:
        /**
           Constructor
        */
        SNMPInfo();

        /**
           Destructor
        */
        virtual ~SNMPInfo(){}

        /**
           Sets receiver

           @param[in] receiver - the value to be set

           @exception @ref klk::Exception
        */
        void setReceiver(const std::string& receiver);

        /**
           Sets community

           @param[in] community - the value to be set

           @exception @ref klk::Exception
        */
        void setCommunity(const std::string& community);
    private:
        std::string m_receiver; ///< SNMP receiver
        std::string m_community; ///< SNMP community
        int m_port; ///< receiver port

        /**
           Gets SNMP receiver for net-snmp initialization

           @return the value, for example localhost, localhost:161
        */
        virtual const std::string getReceiver()
            const throw() {return m_receiver;}

        /**
           Gets SNMP community for net-snmp initialization

           @return the value, for example public
        */
        virtual const std::string getCommunity()
            const throw() {return m_community;}

        /**
           Gets the receiver port
        */
        virtual const int getReceiverPort()
            const throw() {return m_port;}
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        SNMPInfo(const SNMPInfo& value);


        /**
           Assigment operator
           @param[in] value - the copy param
        */
        SNMPInfo& operator=(const SNMPInfo& value);
    };

    /**
       @brief Configuration info

       The class keeps info about Kalinka mediaserver configuration

       @ingroup grConfig
    */
    class Config : public IConfig, public klk::Mutex
    {
    public:
        /**
           Constructor
        */
        Config();

        /**
           Destructor
        */
        virtual ~Config();

        /**
           Sets mediaserver host name

           @param[in] hostname - the host name to be set

           @exception @ref klk::Exception
        */
        void setHostName(const std::string& hostname);

        /**
           @copydoc IConfig::setPath()
        */
        virtual void setPath(const std::string& path);

        /**
           Gets config file path

           @return the path
        */
        virtual const std::string getPath() const throw();

        /**
           @copydoc IConfig::load()
        */
        virtual void load();

        /**
           @copydoc IConfig::save()
        */
        virtual void save();

        /**
           Gets mediaserver host name

           @return the value
        */
        virtual const std::string getHostName() const throw();

        /**
           Gets DB info

           @return the info
        */
        virtual const IDBInfo* getDBInfo()
            const throw() {return &m_dbinfo;}

        /**
           Gets SNMP info
        */
        virtual const ISNMPInfo* getSNMPInfo()
            const throw() {return &m_snmpinfo;}

        /**
           Retrives the options list

           @return the list
        */
        virtual const OptionList getOptions()
            const throw() {return m_options;}
    private:
        std::string m_hostname; ///< the mediaserver host name
        std::string m_config_path; ///< path to config file
        DBInfo m_dbinfo; ///< dbinfo
        SNMPInfo m_snmpinfo; ///< snmp info
        OptionList m_options; ///< options list

        /**
           Fills options
        */
        void fillOptionList() throw();

        /**
           Sets an option value

           @param[in] value - the value to bes set

           @exception @ref klk::Exception
        */
        void setOptionValue(std::string value);
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        Config(const Config& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        Config& operator=(const Config& value);
    };
};

#endif //KLK_KLKCONFIG_H
