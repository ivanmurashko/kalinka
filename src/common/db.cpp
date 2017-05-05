/**
   @file db.cpp
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
   - 2008/09/01 created by ipp (Ivan Murashko)
   - 2008/01/04 DB class interface was rewriten by ipp
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mysqld_error.h>
#include <errmsg.h>
#include <string.h>

#include <sstream>

#include <boost/bind.hpp>

#include "common.h"
#include "log.h"
#include "db.h"
#include "commontraps.h"
#include "exception.h"

using namespace klk::db;

//
// Parameters class
//

// the NULL for sql statements
const char* Parameters::null = static_cast<const char*>(NULL);


// Adds a std::string as a parameter
void Parameters::add(const std::string& key, const char *param)

{
    checkKey(key);
    if (param == NULL)
    {
        m_params.push_back(Pair(key, "NULL"));
    }
    else
    {
        std::string paramstr(param);
        add(key, paramstr);
    }
}

// Adds a std::string as a parameter
// @param[in] param - parameter to be added
void Parameters::add(const std::string& key, const std::string& param)
{
    checkKey(key);
    std::string queryparam = "'" + param + "'";
    m_params.push_back(Pair(key, queryparam));
}


// Adds an int as a parameter
// @param[in] param - parameter to be added
void Parameters::add(const std::string& key, int param)
{
    checkKey(key);
    std::stringstream data;
    data << param;
    m_params.push_back(Pair(key, data.str()));
}

// check functor
struct parameter_check
{
    bool operator()(const Pair& pair, const std::string& key)
    {
        return pair.first == key;
    }
};

// Checks the key
void Parameters::checkKey(const std::string& key)
{
    ParameterMap::iterator i = std::find_if(
        m_params.begin(), m_params.end(),
        boost::bind<bool>(parameter_check(), _1, key));
    if (i != m_params.end())
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "DB parameter '%s' has been already used",
                             key.c_str());
    }
    if (*(key.begin()) != '@')
    {
        throw Exception(__FILE__, __LINE__, "Invalid db client code");
    }
}


//
// Result class
//

// Constructor
Result::Result() :
    m_results()
{
}

// Copy constructor
Result::Result(const Result& value) :
    m_results(value.m_results)
{
}

// Assignment opearator
Result& Result::operator=(const Result& value)
{
    m_results = value.m_results;
    return *this;
}

// Gets a member
const klk::StringWrapper Result::operator[](const std::string& key)
    const
{
    ResultMap::const_iterator i = m_results.find(key);
    if (i == m_results.end())
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Cannot find a DB result for key '%s'",
                             key.c_str());
    }
    return i->second;
}

// Adds an element
void Result::push_back(const std::string& key, const std::string& value)

{
    BOOST_ASSERT(m_results.find(key) == m_results.end());
    m_results.insert(ResultMap::value_type(key, klk::StringWrapper(value)));
}

// Adds an NULL (wrong) element
void Result::push_back_null(const std::string& key) throw()
{
    BOOST_ASSERT(m_results.find(key) == m_results.end());
    m_results.insert(ResultMap::value_type(
                         key,
                         klk::StringWrapper(Parameters::null)));
}

// Check if at least one of fields and it's value matches the
// specified pair
bool Result::operator==(const Pair& pair) const throw()
{
    try
    {
        ResultMap::const_iterator i = m_results.find(pair.first);
        if (i != m_results.end())
        {
            return (i->second.toString() == pair.second);
        }
    }
    catch(...)
    {
        // nothing to do
        return false;
    }
    return false;
}


//
// Answer class
//

// Constructor
// @param[in] result - the result
Answer::Answer(MYSQL_RES *result) :
    m_result(result), m_row(NULL)
{
    BOOST_ASSERT(m_result);
}

// Destructor
Answer::~Answer()
{
    if (m_result)
        mysql_free_result(m_result);
}

// Gets fields count number
size_t Answer::getCount() const throw()
{
    return mysql_num_fields(m_result);
}

// gets row with next result
klk::Result Answer::fetchRow() throw()
{
    m_row = mysql_fetch_row(m_result);
    m_fields = mysql_fetch_fields(m_result);
    if (m_row)
    {
        CHECKNOTNULL(m_fields);
	return klk::OK;
    }

    return klk::ERROR; // no more results
}

// gets value from the specified column
// @param[in] colnum column number
// @return the value or NULL if there was an error
const char* Answer::getColumn(size_t colnum) const
{
    if (colnum >= getCount())
    {
        BOOST_ASSERT(false);
    }

    const char* res = NULL;
    if (m_row)
	res = m_row[colnum];

    return res;
}

// Gets name for the specified column
const std::string Answer::getColumnName(size_t colnum) const

{
    if (colnum >= getCount())
    {
        BOOST_ASSERT(false);
    }

    const char* res = NULL;
    if (m_fields)
	res = m_fields[colnum].name;

    BOOST_ASSERT(res);

    return res;
}

//
// DB class
//

// Constructor
// @param[in] config - the config interface
DB::DB(IFactory* factory):
    m_factory(factory), m_connection(NULL), m_hostuuid("")
{
    BOOST_ASSERT(m_factory);
    mysql_init(&m_dbhandle);
}

// Destructor
DB::~DB()
{
    disconnect();
}

// Connects to the DB
void DB::connect()
{
    // infinite attempts
    while (m_connection == NULL)
    {
        m_connection =
            mysql_real_connect(
                &m_dbhandle,
                m_factory->getConfig()->getDBInfo()->getHost().c_str(),
                m_factory->getConfig()->getDBInfo()->getUserName().c_str(),
                m_factory->getConfig()->getDBInfo()->getUserPwd().c_str(),
                m_factory->getConfig()->getDBInfo()->getDBName().c_str(),
                m_factory->getConfig()->getDBInfo()->getPort(),
                NULL, CLIENT_MULTI_STATEMENTS);
        if (m_connection == NULL &&
            m_factory->getEventTrigger()->isStopped() == false)
        {
            // connection error
            dispResult("");
            // sleep for awhile
            sleep(10);
            // try to reread config
            m_factory->getConfig()->load();
        }
    }
    BOOST_ASSERT(m_connection);

    // set connection charset params
    safeQuery("SET NAMES 'utf8'");
}

// Disconnects from the DB
void DB::disconnect() throw()
{
    if (m_connection)
    {
	mysql_close(m_connection);
        m_connection = NULL;
    }
}

// Sends an error message to our log
void DB::dispResult(const std::string& query) throw()
{
    try
    {
        const std::string error = mysql_error(&m_dbhandle);
        // send trap
        m_factory->getSNMP()->sendTrap(klk::snmp::DB_FAILED,
                                       error);

        if (query.empty())
        {
            klk_log(KLKLOG_ERROR,
                    "We have the following error at MySQL DB: %s",
                    error.c_str());
        }
        else
        {
            klk_log(KLKLOG_ERROR,
                    "We have the following error at MySQL DB: %s. Query: %s",
                    error.c_str(), query.c_str());
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Error detected while display MySQL error message");
    }
}


// executes a stored procedure
const Result DB::callSimple(const std::string& query,
                            const Parameters& params)

{
    BOOST_ASSERT(query.empty() == false);

    // create and execute the query
    const std::string spquery = createQuery(query, params);
    safeQuery(spquery);

    // go throught all statements up to result
    for (u_int i = 0; i < params.get()->size(); i++)
    {
        int qr = mysql_next_result(&m_dbhandle);
        if (qr != 0)
        {
            const std::string error = mysql_error(&m_dbhandle);
            throw klk::Exception(__FILE__, __LINE__,
                                 "Incomplete DB response. "
                                 "Total parameters: %u. "
                                 "Read parameters: %u. "
                                 "Query: %s. "
                                 "Error: %s",
                                 params.get()->size(), i + 1,
                                 spquery.c_str(),
                                 error.c_str());
        }
    }

    ResultVector rv;
    int qr = 0;
    while (qr == 0)
        qr = mysql_next_result(&m_dbhandle);
    // process answers stored procedure output
    MYSQL_RES* res = mysql_store_result(&m_dbhandle);
    BOOST_ASSERT(res);
    Answer answer(res);
    klk::Result rc = answer.fetchRow();
    BOOST_ASSERT(rc == klk::OK); // at least one should be
    const Result result = getResult(answer);
    rc = answer.fetchRow();
    BOOST_ASSERT(rc == klk::ERROR); // only one

    qr = 0;
    while (qr == 0)
        qr = mysql_next_result(&m_dbhandle);

    // last one (just a check)
    BOOST_ASSERT(qr == -1);

    return result;
}

// executes a stored procedure
const ResultVector DB::callSelect(const std::string& query,
                                  const Parameters& params,
                                  Result* result)

{
    BOOST_ASSERT(query.empty() == false);

    // create and execute the query
    const std::string spquery = createQuery(query, params);
    safeQuery(spquery);

    // go throught all statements up to result
    for (u_int i = 0; i < params.get()->size(); i++)
    {
        int qr = mysql_next_result(&m_dbhandle);
        if (qr != 0)
        {
            const std::string error = mysql_error(&m_dbhandle);
            throw klk::Exception(__FILE__, __LINE__,
                                 "mysql_next_result() failed "
                                 "Res: %d. "
                                 "Query: %s. "
                                 "Error: %s",
                                 qr,
                                 spquery.c_str(),
                                 error.c_str());
        }
    }

    // is there select?
    MYSQL_RES* res = mysql_store_result(&m_dbhandle);
    BOOST_ASSERT(res);
    Answer answer(res);
    ResultVector rv;
    while (answer.fetchRow() == klk::OK)
    {
        rv.push_back(getResult(answer));
    }

    if (!params.get()->empty())
    {
        int qr = 0;
        while (qr == 0)
            qr = mysql_next_result(&m_dbhandle);
        // process answers stored procedure output
        MYSQL_RES* res = mysql_store_result(&m_dbhandle);
        BOOST_ASSERT(res);
        Answer answer(res);
        if (result)
        {
            klk::Result rc = answer.fetchRow();
            BOOST_ASSERT(rc == klk::OK); // at least one should be
            *result = getResult(answer);
            rc = answer.fetchRow();
            BOOST_ASSERT(rc == klk::ERROR); // only one
        }
    }

    int qr = 0;
    while (qr == 0)
        qr = mysql_next_result(&m_dbhandle);

    // last one (just a check)
    BOOST_ASSERT(qr == -1);

    return rv;
}


// Gets result
const Result DB::getResult(const Answer& answer)
{
    size_t count = answer.getCount();
    Result res;
    for (size_t i = 0; i < count; i++)
    {
        const char* resval = answer.getColumn(i);
        if (resval != NULL)
        {
            res.push_back(answer.getColumnName(i), resval);
        }
        else
        {
            res.push_back_null(answer.getColumnName(i));
        }
    }
    return res;
}

// Safe request to the db
// @param query query that has to be executed
void DB::safeQuery(const std::string& query)
{
    BOOST_ASSERT(query.empty() == false);
    int rc = mysql_real_query(&m_dbhandle, query.c_str(), query.size());
    if (rc)
    {
	if ((mysql_errno(&m_dbhandle) == CR_SERVER_LOST) ||
	    (mysql_errno(&m_dbhandle) == CR_SERVER_GONE_ERROR ) ||
	    (mysql_errno(&m_dbhandle) == ER_SERVER_SHUTDOWN))
	{
	    // sleep 5 sec.
	    usleep(5000000);
	    rc = mysql_real_query(&m_dbhandle, query.c_str(), query.size());
	}
    }

    if (rc != 0)
    {
	dispResult(query);
        throw klk::Exception(__FILE__, __LINE__,
                             "DB error while execute the query: " + query);
    }
}

// gets mediaserver host UUID
// @return the UUID
const std::string DB::getHostUUID()
{
    if (m_hostuuid.empty())
    {
        const std::string procname = "klk_host_get_uuid";
        Parameters param;
        BOOST_ASSERT(m_factory->getConfig()->getHostName().empty() == false);
        param.add("@host", m_factory->getConfig()->getHostName());
        param.add("@uuid", Parameters::null);
        Result result = callSimple(procname, param);
        m_hostuuid = result["@uuid"].toString();
    }
    return m_hostuuid;
}

// gets sql query from parameters
// @param[in] query - the query
// @param[in] params - the parameters
// @param[in] result - the result container
const std::string DB::createQuery(const std::string& query,
                                  const Parameters& params)
    const throw()
{
    std::string spquery;
    if (params.get()->empty())
    {

        spquery = "CALL " + query + "();";
    }
    else
    {
        std::string set;
        std::string sp = "CALL " + query + "(";
        std::string select ("SELECT ");

        for (ParameterMap::const_iterator i = params.get()->begin();
             i != params.get()->end(); i++)
        {
            set += "SET " + i->first + " = " + i->second + ";";
            if (i != params.get()->begin())
            {
                sp += ", ";
                select += ", ";
            }
            sp += i->first;
            select += i->first;
        }

        sp += ");";
        select += ";";

        spquery = set + sp + select;
    }

    klk_log(KLKLOG_DEBUG, "SQL query: %s", spquery.c_str());
    return spquery;
}

// Reset the db internal variables
void DB::reset() throw()
{
    m_hostuuid.clear();
}

