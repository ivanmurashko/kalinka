/**
   @file snmp.h
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
   - 2008/11/20 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SNMP_H
#define KLK_SNMP_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "ifactory.h"
#include "thread.h"
#include "errors.h"

namespace klk
{
    namespace snmp
    {
        /**
           Base class for trap info

           @ingroup grSNMP
        */
        class TrapInfoBase
        {
        protected:
            size_t m_oid_len; ///< the objid lenght
            oid m_oid[MAX_OID_LEN]; ///< the objid

            /**
               Constructor

               @param[in] objid - the object ID in text form
            */
            explicit TrapInfoBase(const std::string& objid);

            /**
               Destructor
            */
            virtual ~TrapInfoBase(){}
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TrapInfoBase(const TrapInfoBase& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TrapInfoBase& operator=(const TrapInfoBase& value);
        };

        /**
           @brief Trap info

           The class holds info about a trap

           @ingroup grSNMP
        */
        class TrapInfo : public TrapInfoBase
        {
        public:
            /**
               Constructor for a string trap

               @param[in] objid - the object ID in text form
               @param[in] value - the trap value
            */
            TrapInfo(const std::string& objid, const std::string& value);

            /**
               Constructor for an integer trap

               @param[in] objid - the object ID in text form
               @param[in] value - the trap value
            */
            TrapInfo(const std::string& objid, int value);

            /**
               Adds the trap info to the SNMP PDU(protocol data unit)

               @param[in] pdu - the PDU that will be updated

               @return
               - @ref klk::OK
               - @ref klk::ERROR
            */
            Result add2PDU(netsnmp_pdu* pdu) const;

            /**
               Destructor
            */
            ~TrapInfo(){}
        private:
            std::string m_value; ///< the trap value
            char m_type; ///< the net-snmp specific type
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TrapInfo(const TrapInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TrapInfo& operator=(const TrapInfo& value);
        };

        /**
           @brief ISNMP interface realization

           ISNMP interface realization

           @ingroup grSNMP
        */
        class SNMP : public ISNMP, public Mutex
        {
        public:
            /**
               The constructor

               @param[in] config - the config info
            */
            explicit SNMP(IConfig* config);

            /**
               Destructor
            */
            virtual ~SNMP();
        private:
            IConfig* m_config; ///< configuration info

            /**
               Sends a string trap

               @param[in] objid - the trap's id
               @param[in] value - the trap's value
            */
            virtual void sendTrap(const std::string& objid,
                                  const std::string& value);

            /**
               Sends an integer trap

               @param[in] objid - the trap's id
               @param[in] value - the trap's value
            */
            virtual void sendTrap(const std::string& objid, int value);

            /**
               Sends a trap

               @param[in] trap - the trap's info
            */
            virtual void sendTrap(const TrapInfo& trap);

            /**
               Retrivies agent addr (aka localhost)

               @return the addr
            */
            in_addr_t* getAgentAddr();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SNMP(const SNMP& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SNMP& operator=(const SNMP& value);
        };
    }
}

#endif //KLK_SNMP_H
