"""@package net
   @file net.py
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
   - 2010/12/04 created by ipp (Ivan Murashko)
"""

import klk.common as common
import klk.adapter

import MySQLdb

from defines import *

class HTTPSourceItem(common.Object):
    """ Represent an http source item
    HTTPSrc description consists of object name, uuid, file object type
    (is there a regular file or folder) and the file path itself
    """
    
    def __init__(self, uuid, name, address, login, passwd):
        """Constructs a file item
        @param[in] uuid - the item's uuid
        @param[in] name - the item's name
        @param[in] address - the http address where the source can be got
        @param[in] login - the HTTP login. Empty value means no auth.
        @param[in] passwd - the HTTP passwd
        """
        common.Object.__init__(self, uuid, name)
        self._address = address
        self._login = login
        self._passwd = passwd

    @property
    def address(self):
        """ @return the http source address (http link)
        """
        return self._address

    @property
    def login(self):
        """ @return the http source login
        """
        return self._login

    @property
    def passwd(self):
        """ @return the http source password
        """
        return self._passwd

class HTTPSource(common.Module):
    """ HTTP Source module
    """
    
    def __init__(self, server):
        """ Constructor
        @param[in] server - the parent object (the mediaserver 
        that keeps the network module)
        """
        common.Module.__init__(self, UUID, NAME, server)

    def addSource(self, name, address, login, passwd):
        """ Adds a new http source
        @param[in] name - the item's name
        @param[in] address - the http address where the source can be got
        @param[in] login - the HTTP login. Empty value means no auth.
        @param[in] passwd - the HTTP passwd
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()        
            uuid = None
            return_value = 0
            description = '%s: %s' % ( name, address )
            args = (uuid, name, address, login, \
                        passwd, description, return_value)
            cursor.callproc('klk_httpsrc_add',  args )
            cursor.execute('SELECT @_klk_httpsrc_add_6')
            rows = cursor.fetchone()
            if rows[0] != 0:
                print rows[0]
                raise common.KLKException,\
                    "MySQL error: cannot add an http source"

            conn.commit()
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)

    def delSource(self, name):
        """ Deletes a source by its name
        @param[in] name - the http source item name to be deleted
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()            
            return_value = 0
            args = (name, return_value, )
            cursor.callproc('klk_httpsrc_del_cli',  args )
            cursor.execute('SELECT @_klk_httpsrc_del_cli_1')
            rows = cursor.fetchone()
            if rows[0] != 0:
                print rows[0]
                raise common.KLKException,\
                    "MySQL error: cannot delete an http source"

            conn.commit()
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)

    @property
    def sources(self):
        """ Retrives a dict with HTTP source items. Key is the item's name
        """        
        items = dict()
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            args = ()
            cursor.callproc('klk_httpsrc_list',  args )
            rows = cursor.fetchall()
            for row in rows:
                # SELECT 
                # httpsrc AS uuid, 
                # title AS name, 
                # address AS addr, 
                # login AS login, 
                # passwd AS password 
                #FROM klk_httpsrc;
                uuid = row[0]
                name = row[1]
                address = row[2]
                login = row[3]
                passwd = row[4]
                items[name] = HTTPSourceItem(uuid, name, address, login, passwd)
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])
        return items

