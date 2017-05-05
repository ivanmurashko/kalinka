/**
   @file db.h
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

#ifndef KLK_DB_H
#define KLK_DB_H

#include <mysql.h>

#include <map>

#include "ifactory.h"
#include "stringlist.h"
#include "stringwrapper.h"
#include "errors.h"


namespace klk
{
    namespace db
    {
        /** @defgroup grDB DB connections

            There are several classes that are used for db connections

            @{
        */

        /**
           Parameter pair
        */
        typedef std::pair<std::string, std::string> Pair;

        /**
           Parameters map
        */
        typedef std::list<Pair> ParameterMap;

        /**
           @brief Class that holds sql query params

           Class that holds sql query params
        */
        class Parameters
        {
        public:
            static const char* null; ///< NULL for sql statements
        public:
            /**
               Constructor
            */
            Parameters(){}

            /**
               Destructor
            */
            virtual ~Parameters(){}

            /**
               Gets list of parameters
            */
            const ParameterMap* get() const throw() {return &m_params;}

            /**
               Adds a std::string as a parameter

               @param[in] key - the key for the parameter
               @param[in] param - parameter to be added

               @note use @ref db::Parameters::null if you want to add NULL

               @exception @ref klk::Exception if the key is not valid
            */
            void add(const std::string& key,
                     const char *param = null)
                ;

            /**
               Adds a std::string as a parameter

               @param[in] param - parameter to be added
               @param[in] key - the key for the parameter

               @exception @ref klk::Exception if the key is not valid
            */
            void add(const std::string& key, const std::string& param)
                ;

            /**
               Adds an int as a parameter

               @param[in] param - parameter to be added
               @param[in] key - the key for the parameter

               @exception @ref klk::Exception if the key is not valid
            */
            void add(const std::string& key, int param)
                ;

            /**
               Clears parameters list
            */
            void clear() throw(){m_params.clear();}
        private:
            ParameterMap m_params; ///< parameters

            /**
               Checks the key

               The key should start with '@' and
               should be unique

               @exception @ref klk::Exception
            */
            void checkKey(const std::string& key);
        private:
            /**
               Copy constructor
            */
            Parameters(const Parameters&);

            /**
               Assignment opearator
            */
            Parameters& operator=(const Parameters&);
        };

        /**
           @brief Class that holds sql query results

           Class that holds sql query results
        */
        class Result
        {
        public:
            /**
               Constructor
            */
            Result();

            /**
               Copy constructor
            */
            Result(const Result& value);

            /**
               Assignment opearator
            */
            Result& operator=(const Result& value);

            /**
               Destructor
            */
            virtual ~Result(){}

            /**
               Gets a member

               @param[in] key - the key for the member

               @note const method

               @exception @ref klk::Exception
            */
            const StringWrapper operator[](const std::string& key) const;

            /**
               Adds an element

               @param[in] value - the value to be added
               @param[in] key - the key for the value

               @exception @ref klk::Exception
            */
            void push_back(const std::string& key,
                           const std::string& value);

            /**
               Adds an NULL (wrong) element
               @param[in] key - the key for the value

               @exception @ref klk::Exception
            */
            void push_back_null(const std::string& key) throw();

            /**
               Check if at least one of fields and it's value matches the
               specified pair

               @return
               - true at least one result match
               - false - no such records
            */
            bool operator==(const Pair& pair) const throw();
        private:
            /**
               Result map
            */
            typedef std::map<std::string, StringWrapper> ResultMap;
            ResultMap m_results; ///< parameters
        };

        /**
           Vector with DB results
        */
        typedef std::vector<Result> ResultVector;

        /**
           @brief DB select response

           Select response
        */
        class Answer
        {
        public:
            /**
               Constructor

               @param[in] result - the result
            */
            Answer(MYSQL_RES *result);

            /**
               Destructor
            */
            ~Answer();

            /**
               Gets row with next result

               @return
               - @ref klk::OK all is OK
               - @ref klk::ERROR no more rows or an error
            */
            klk::Result fetchRow() throw();

            /**
               Gets value from the specified column

               @param[in] colnum column number

               @return the value

               @exception @ref klk::Exception
            */
            const char* getColumn(size_t colnum) const;

            /**
               Gets name for the specified column

               @param[in] colnum column number

               @return the name value

               @exception @ref klk::Exception
            */
            const std::string getColumnName(size_t colnum) const;

            /**
               Gets fields count number

               @return the value
            */
            size_t getCount() const throw();
        private:
            MYSQL_RES *m_result; ///< MySQL result
            MYSQL_ROW m_row; ///< result row
            MYSQL_FIELD* m_fields; // result fields
        };

        /**
           @brief The DB interface

           The DB interface
        */
        class DB
        {
#ifdef DEBUG
            friend class DBTest;
#endif //DEBUG
        public:
            /**
               Constructor

               @param[in] factory - the factory interface

               @exception @ref klk::Exception - send when an error
               during initialization
            */
            explicit DB(IFactory* factory);

            /**
               Destructor
            */
            virtual ~DB();

            /**
               Connects to the DB

               @exception @ref klk::Exception
            */
            void connect();

            /**
               Executes a stored procedure (without select statement)

               @param[in] query - the stored procedure name
               @param[in] params - the parameters

               @return @ref db::Result

               @exception @ref klk::Exception
            */
            const Result callSimple(const std::string& query,
                                    const Parameters& params);

            /**
               Executes a stored procedure

               @param[in] query - the stored procedure name
               @param[in] params - the parameters
               @param[out] result - the pointer to result
               can be NULL - this means to don't store results

               @return the result vector

               @exception @ref klk::Exception
            */
            const ResultVector callSelect(const std::string& query,
                                          const Parameters& params,
                                          Result* result);

            /**
               Gets mediaserver host UUID

               @return the UUID

               @exception @ref klk::Exception
            */
            const std::string getHostUUID();

            /**
               Reset the db internal variables

               @note used in @ref grTest "Our utests" only

               FIXME!!!
            */
            void reset() throw();
        private:
            IFactory * const m_factory; ///< the config interface
            MYSQL m_dbhandle; ///< MySQL DB's handle
            MYSQL *m_connection; ///< connection handle
            std::string m_hostuuid; ///< mediaserver host's uuid

            /**
               Sends an error message to our log

               @param[in] query the query that produces the error
               (can be empty)
            */
            void dispResult(const std::string& query) throw();

            /**
               Safe request to the db

               @param[in] query - the query that has to be executed

               @exception @ref klk::Exception
            */
            void safeQuery(const std::string& query);

            /**
               Gets sql query from parameters

               @param[in] query - the query
               @param[in] params - the parameters

               @returns the query std::string
            */
            const std::string createQuery(const std::string& query,
                                          const Parameters& params)
                const throw();

            /**
               Gets result

               @param[in] answer - the answer data

               @return the result

               @exception @ref klk::Exception
            */
            const Result getResult(const Answer& answer);

            /**
               Disconnects from the DB
            */
            void disconnect() throw();
        private:
            /**
               Copy constructor
            */
            DB(const DB&);

            /**
               Assignment opearator
            */
            DB& operator=(const DB&);
        };

        /** @} */

    }
}

#endif //KLK_DB_H
