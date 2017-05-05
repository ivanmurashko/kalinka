"""@package adapter
   @file adapter.py
   @author Ivan Murashko <ivan.murashko@gmail.com>

   Copyright (c) 2007-2010 Kalinka Team

   This file is part of Kalinka mediaserver.

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
   - 2010/12/25 created by ipp (Ivan Murashko)
"""

import klk.common as common

import sys
import warnings

import Ice

import ipc 

from defines import *

import MySQLdb

class Adapter(common.Module):
    """ The adapter module class
    """
    
    def __init__(self, server):
        """ Constructor
        @param[in] server - the mediaserver instance
        """
        common.Module.__init__(self, UUID, NAME, server)

    def getModulesEndpoint(self):
        """ Retrieves endpoint for manipulation with modules

        @return the endpoint string 
        """
        return self.getEndpoint(MODULES_OBJECT, MODULE_COMMON_ID)

    def getMessagesEndpoint(self, modid):
        """ Retrieves endpoint for the specified module id
        @param[in] modid - the module id to be used for the retrieval

        @return the endpoint string 
        """
        return self.getEndpoint(MESSAGES_OBJECT, modid)

    def getEndpoint(self, object_id, mod_id):
        """ Retrieves endpoint for the specified module id
        @param[in] modid - the module id to be used for the retrieval

        @return the endpoint string 
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            warnings.filterwarnings("ignore", "No data .*")

            # IN host_uuid VARCHAR(40), 
            # IN module_id VARCHAR(40), 
            # IN object_id VARCHAR(40), 
            # OUT endpoint VARCHAR(255), 
            # OUT return_value INT 

            endpoint = ''
            return_value = 0
            args = (self._server.uuid, mod_id, object_id, endpoint, return_value )
            cursor.callproc('klk_adapter_get',  args )
            cursor.execute('SELECT @_klk_adapter_get_4, @_klk_adapter_get_3')
            rows = cursor.fetchone()
            endpoint = rows[1]
            cursor.close()
            if rows[0] == 0:
                return endpoint
            elif mod_id != MODULE_COMMON_ID:
                return self.getEndpoint(object_id, MODULE_COMMON_ID)
            else:
                return None
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL connection error %d: %s" % (e.args[0], e.args[1])        

    

    def checkConnection(self, modid):
        """ Checks connection to the specified module
        @param[in] modid - the module to be checked
        @return
        - True - OK result - connection was success
        - False - there was an error
        """
        messages_endpoint = self.getMessagesEndpoint(modid)
        if not messages_endpoint:
            # there is no connection info at the DB
            return False
        result = True
        try:
            ic = Ice.initialize(None)
            messages_base = ic.stringToProxy(messages_endpoint)
            messages_proxy = ipc.IMessagesProxyPrx.checkedCast(messages_base)
            # will raise an exception if there is any trouble with connection establishing
            if not messages_proxy:
                raise RuntimeError("Invalid messages proxy")

            messages_proxy.ice_ping()
            messages_proxy = None
        except:
            result = False

        # check that the module (not application) is really loaded
        if self.isInternalModule(modid):
            modules_endpoint = self.getModulesEndpoint()
            if not modules_endpoint:
                # there is no connection info at the DB
                return False
            modules_base = ic.stringToProxy(modules_endpoint)
            modules_proxy = ipc.IModulesProxyPrx.checkedCast(modules_base)
            if not modules_proxy:
                raise RuntimeError("Invalid modules proxy")
            result = modules_proxy.isLoaded(modid)


        if ic:
            # Clean up
            ic.destroy()
        return result

    def isInternalModule(self, modid):
        """ Checks is the specified module internal (run inside klkd process)
        or launched separatelly via klklaunch
        @param[in] modid - the module id to be tested
        """
        endpoint1 = self.getMessagesEndpoint(modid)
        endpoint2 = self.getMessagesEndpoint(MODULE_COMMON_ID)
        return (endpoint1 == endpoint2) and (modid != MODULE_COMMON_ID)
            

    def sendSyncMessage(self, modid, msgid, msgargs = dict()):
        """ Sends the sync message to the specified module
        @param[in] modid - the module that receives the request
        @param[in] msgid - the message to be sent

        @return the result message

        @note will do nothing if the modid has not been started (don't have an endpoint)
        """

        if not self.checkConnection(modid):
            # we cannot send sync message to not started service
            # stop here
            return None

        ic = None
        status = 0
        endpoint = self.getMessagesEndpoint(modid)
        if not endpoint:
            return None
        result = None
        try:
            ic = Ice.initialize(None)
            base = ic.stringToProxy(endpoint)
            msgproxy = ipc.IMessagesProxyPrx.checkedCast(base)
            if not msgproxy:
                raise RuntimeError("Invalid proxy")
            inmsg = msgproxy.getMessage(msgid)
            # fill message arguments
            for (key, value) in msgargs.items():
                if type(value) is list:                    
                    inmsg.mLists[key] = value
                else:
                    inmsg.mValues[key] = value
            result = msgproxy.sendSync(inmsg)
        except Exception, e:
            print str(e)
            status =1
        if ic:
            # Clean up
            ic.destroy()
        if status:
            raise common.KLKException,\
                "Cannot send sync message"
        return result
