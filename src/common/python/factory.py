"""@package factory
   @file factory.py
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
   - 2011/01/07 created by ipp (Ivan Murashko)
"""

import server
import config
import db
import mediatypes
import common

import MySQLdb

class Factory:
    """ The class represent the kalinka mediaserver as a collection
    of several Server objects that are accessable via the common DB

    The DB connection info is pass via config file that is set via 
    __init__ method

    Pattern singleton
    """

    class __impl:
        """ Implementation of the singleton interface """
        pass

     # storage for the instance reference
    __instance = None

    def __init__(self, config_path=None):
        """ Constructor
        """
        # Check whether we already have an instance
        if Factory.__instance is None:
            # Create and remember instance
            Factory.__instance = Factory.__impl()

            # Store instance reference as the only member in the handle
            self.__dict__['_Factory__instance'] = Factory.__instance

            self.config = config.Config()
        
            if config_path:
                self.config.load(config_path)

            # fill the dict with relation between mdia types uuid and names
            # FIXME!!! make the dict fill auto
            self._mediatypes = dict()
            self._mediatypes[mediatypes.EMPTY] = mediatypes.EMPTY_NAME
            self._mediatypes[mediatypes.FLV] = mediatypes.FLV_NAME
            self._mediatypes[mediatypes.OGG] = mediatypes.OGG_NAME
            self._mediatypes[mediatypes.MPEGTS] = mediatypes.MPEGTS_NAME
            self._mediatypes[mediatypes.AUTO] = mediatypes.AUTO_NAME
            self._mediatypes[mediatypes.RTP_MPEGTS] = mediatypes.RTP_MPEGTS_NAME

    def __getattr__(self, attr):
        """ Delegate access to implementation """
        return getattr(self.__instance, attr)

    def __setattr__(self, attr, value):
        """ Delegate access to implementation """
        return setattr(self.__instance, attr, value)

    @property
    def servers(self):
        """ @return dict with available servers.
        The key is the server name
        """
        result = dict()
        try:
            database = self.getDB()
            conn = database.connect()
            cursor = conn.cursor()
            cursor.callproc('klk_host_list')
            rows = cursor.fetchall()
            for row in rows:
                result[row[1]] = server.Server(self, row[0], row[1])
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL connection error %d: %s" % (e.args[0], e.args[1])
        return result

    def addServer(self, name):
        """ Adds a new server
        @param[in] name - the server name
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            # INOUT host VARCHAR(40),
            # IN host_name VARCHAR(50),
            # IN cpu_index_typ INTEGER,
            # IN cpu_index_max INTEGER,
            # OUT return_value INTEGER

            return_value = 0
            args = (None,  name, 0, 0, return_value )
            cursor.callproc('klk_host_add',  args )
            cursor.execute('SELECT @_klk_host_add_4')
            rows = cursor.fetchone()
            if rows[0] != 0:
                print rows[0]
                raise common.KLKException,\
                    "MySQL error: cannot add a server"

            conn.commit()            
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])


    def delServer(self, name):
        """ Deletes a server with sepcifed name
        @param[in] name - the server name
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            # IN host VARCHAR(40),
            # OUT return_value INTEGER
            return_value = 0
            args = (name, return_value )
            cursor.callproc('klk_host_delete',  args )
            cursor.execute('SELECT @_klk_host_delete_4')
            rows = cursor.fetchone()
            if rows[0] != 0:
                print rows[0]
                raise common.KLKException,\
                    "MySQL error: cannot delete a server"

            conn.commit()            
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

    def getDB(self):
        """ Retrives DB instance
        """
        return db.DB(self.config)

    def getMediaTypeName(self, uuid):
        """ Retrives the media type name by its uuid
        @param[in] uuid - the uuid to be used
        """
        return self._mediatypes[uuid]

    def getMediaTypeUUID(self, name):
        """ Retrieves the media type uuid by its name
        @param[in] name - the name to be used
        """
        for uuid in self._mediatypes.keys():
            if self._mediatypes[uuid] == name:
                return uuid
        return None

