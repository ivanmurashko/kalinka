/**
   @file statustable.cpp
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
   - 2009/05/27 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "statustable.h"
#include "snmpfactory.h"
#include "exception.h"
#include "defines.h"

// some declarations
static Netsnmp_Node_Handler table_handler;
static Netsnmp_First_Data_Point table_get_first_data;
static Netsnmp_Next_Data_Point table_get_next_data;
static Netsnmp_Free_Loop_Context table_free_loop;
static void init_table(void);

/**
   Columns
*/
typedef enum
{
    COLUMN_INDEX = 1,
    COLUMN_OUTPUTPATH = 2,
    COLUMN_INPUTADDR = 3,
    COLUMN_INPUTRATE = 4,
    COLUMN_OUTPUTRATE = 5,
    COLUMN_OUTPUTCONN = 6,
    COLUMN_BROKENPACKAGES = 7
} Column;

using namespace klk;
using namespace klk::http;

/**
   Main init function for snmpd daemon
*/
void init_klksnmphttpstreamer(void)
{
    try
    {
        // do table initialization
        init_table();
    }
    catch(const std::exception& err)
    {
        const std::string msg =
            std::string("Error during initialization: ") +
            err.what() + "\n";
        DEBUGMSGTL((SNMPID.c_str(), "%s",  msg.c_str()));
    }
    catch(...)
    {
        DEBUGMSGTL((SNMPID.c_str(), "%s",
                    "Unknown error during initialization\n"));
    }
}


/**
   Does deinitialization
*/
void deinit_klksnmphttpstreamer(void)
{
    try
    {
        SNMPFactory::instance()->destroy();
    }
    catch(const std::exception& err)
    {
        const std::string msg =
            std::string("Error during deinitialization: ") +
            err.what() + "\n";
        DEBUGMSGTL((SNMPID.c_str(), "%s",  msg.c_str()));
    }
    catch(...)
    {
        DEBUGMSGTL((SNMPID.c_str(), "%s",
                    "Unknown error during deinitialization\n"));
    }
}


/**
   Does the table initialization
*/
static void init_table(void)
{
    static oid table_oid[] = {1,3,6,1,4,1,31106,5,2};
    size_t oid_len   = OID_LENGTH(table_oid);
    netsnmp_handler_registration    *reg = NULL;
    netsnmp_iterator_info           *iinfo = NULL;
    netsnmp_table_registration_info *table_info = NULL;

    reg = netsnmp_create_handler_registration(
        SNMPID.c_str(),
        table_handler,
        table_oid,
        oid_len,
        HANDLER_CAN_RONLY
        );

    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);
    netsnmp_table_helper_add_indexes(
        table_info,
        ASN_COUNTER,  /* index: klkIndex */
        0);
    table_info->min_column = COLUMN_INDEX;
    table_info->max_column = COLUMN_BROKENPACKAGES;

    iinfo = SNMP_MALLOC_TYPEDEF(netsnmp_iterator_info);
    iinfo->get_first_data_point = table_get_first_data;
    iinfo->get_next_data_point = table_get_next_data;
    iinfo->free_loop_context_at_end = table_free_loop;
    iinfo->table_reginfo = table_info;

    netsnmp_register_table_iterator(reg, iinfo);

    DEBUGMSGTL((SNMPID.c_str(), "%s",
                "finished klkHTTPStatusTable initialization\n"));
}

/**
   Frees memory after an request
*/
static void table_free_loop(void* data_free, netsnmp_iterator_info* mydata)
{
    try
    {
        SNMPFactory::instance()->clearData();
    }
    catch(...)
    {
        DEBUGMSGTL((SNMPID.c_str(), "%s",
                    "Unknown error in table_free_loop()\n"));
    }
}


/**
    Retrives a first data portion
*/
static netsnmp_variable_list *
table_get_first_data(void **my_loop_context,
                     void **my_data_context,
                     netsnmp_variable_list *put_index_data,
                     netsnmp_iterator_info *mydata)
{
    try
    {
        SNMPFactory::instance()->retriveData();
    }
    catch(const std::exception& err)
    {
        const std::string msg =
            std::string("Error during in table_get_first_data(): ") +
            err.what() + "\n";
        DEBUGMSGTL((SNMPID.c_str(), "%s",  msg.c_str()));
    }
    catch(...)
    {
        DEBUGMSGTL((SNMPID.c_str(), "%s",
                    "Unknown error in table_get_first_data()\n"));
    }
    return table_get_next_data(my_loop_context, my_data_context,
                               put_index_data,  mydata );
}

static netsnmp_variable_list *
table_get_next_data(void **my_loop_context,
                    void **my_data_context,
                    netsnmp_variable_list *put_index_data,
                    netsnmp_iterator_info *mydata)
{
    try
    {
        snmp::TableRow *row = SNMPFactory::instance()->getNext();
        if (row != NULL)
        {
            netsnmp_variable_list *idx = put_index_data;
            snmp_set_var_typed_integer(idx, ASN_COUNTER,
                                       (*row)[COLUMN_INDEX - 1].toInt());
            idx = idx->next_variable;

            // set data context storage
            *my_data_context = static_cast<void*>(row);

            return put_index_data;
        }
    }
    catch(const std::exception& err)
    {
        const std::string msg =
            std::string("Error during in table_get_next_data(): ") +
            err.what() + "\n";
        DEBUGMSGTL((SNMPID.c_str(), "%s",  msg.c_str()));
    }
    catch(...)
    {
        DEBUGMSGTL((SNMPID.c_str(), "%s",
                    "Unknown error in table_get_next_data()\n"));
    }

    // we are at the end or an error
    return NULL;
}


/**
    Handles requests for the snmphttpstreamer table
*/
static int table_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests)
{
    try
    {

        switch (reqinfo->mode)
        {
            /*
             * Read-support (also covers GetNext requests)
             */
        case MODE_GET:
            for (netsnmp_request_info* request=requests;
                 request; request=request->next)
            {
                snmp::TableRow *row =
                    static_cast<snmp::TableRow *>(
                        netsnmp_extract_iterator_context(request));

                if (!row)
                {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }


                netsnmp_table_request_info *table_info =
                    netsnmp_extract_table_info(request);

                // check range
                if (row->size() < table_info->colnum)
                {
                    DEBUGMSGTL((SNMPID.c_str(), "%s",
                                "Error in table_handler(): "
                                "data out of range\n"));
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                // retrive value
                const StringWrapper val((*row)[table_info->colnum - 1]);
                switch (table_info->colnum)
                {
                case COLUMN_OUTPUTPATH:
                case COLUMN_INPUTADDR:
                    snmp_set_var_typed_value(
                        request->requestvb,
                        ASN_OCTET_STR,
                        reinterpret_cast<const u_char*>(val.toString().c_str()),
                        val.toString().size());
                    break;
                case COLUMN_INPUTRATE:
                case COLUMN_OUTPUTRATE:
                    snmp_set_var_typed_integer(request->requestvb, ASN_INTEGER,
                                               val.toInt());

                    break;
                case COLUMN_INDEX:
                case COLUMN_OUTPUTCONN:
                case COLUMN_BROKENPACKAGES:
                    snmp_set_var_typed_integer(request->requestvb, ASN_COUNTER,
                                               val.toInt());

                    break;
                default:
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHOBJECT);
                    break;
                }
            }
            break;
        }
    }
    catch(const std::exception& err)
    {
        const std::string msg =
            std::string("Error during in table_handler(): ") +
            err.what() + "\n";
        DEBUGMSGTL((SNMPID.c_str(), "%s",  msg.c_str()));
        return SNMP_ERR_GENERR;
    }
    catch(...)
    {
        DEBUGMSGTL((SNMPID.c_str(), "%s",
                    "Unknown error in table_handler()\n"));
        return SNMP_ERR_GENERR;
    }


    return SNMP_ERR_NOERROR;
}
