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

class FileItem(common.Object):
    """ Retresent a file item
    File description consists of object name, uuid, file object type
    (is there a regular file or folder) and the file path itself
    """
    
    def __init__(self, uuid, name, path,  type_name):
        """Constructs a file item
        @param[in] uuid - the item's uuid
        @param[in] name - the item's name
        @param[in] type_name - the item type
        @param[in] path - the file path
        """
        common.Object.__init__(self, uuid, name)
        self._type_name = type_name
        self._path = path

    @property
    def type_name(self):
        """ @return the file object type name
        """
        return self._type_name

    @property
    def path(self):
        """ @return the file object path
        """
        return self._path

class File(common.Module):
    """ File module
    """
    
    def __init__(self, server):
        """ Constructor
        @param[in] server - the parent object (the mediaserver 
        that keeps the network module)
        """
        common.Module.__init__(self, UUID, NAME, server)

    def addFile(self, name, path, type_name):
        """ Adds a new file
        @param[in] name - the internal name for operation inside KLK
        @param[in] path - the file path
        @param[in] type_name - the file type name
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()        
            uuid = None
            return_value = 0
            args = (uuid, name, path, self._server.uuid, type_name, return_value, )
            cursor.callproc('klk_file_add_cli',  args )
            cursor.execute('SELECT @_klk_file_add_cli_5')
            rows = cursor.fetchone()
            if rows[0] != 0:
                print rows[0]
                raise common.KLKException,\
                    "MySQL error: cannot add a file"

            conn.commit()
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)

    def delFile(self, name):
        """ Deletes a file by its name
        @param[in] name - the file item name to be deleted
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()            
            return_value = 0
            args = (name, return_value, )
            cursor.callproc('klk_file_del_cli',  args )
            cursor.execute('SELECT @_klk_file_del_cli_1')
            rows = cursor.fetchone()
            if rows[0] != 0:
                print rows[0]
                raise common.KLKException,\
                    "MySQL error: cannot delete a file"

            conn.commit()
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)

    @property
    def files(self):
        """ Retrives a dict with file items. Key is the item name
        """        
        items = dict()
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            args = (self._server.uuid, )
            cursor.callproc('klk_file_list',  args )
            rows = cursor.fetchall()
            for row in rows:
                # klk_file.file, 
                # klk_file.name, 
                # klk_file.file_path, 
                # klk_file.type_uuid,
                # klk_file_type.type_name
                uuid = row[0]
                name = row[1]
                path = row[2]
                type_name = row[4]
                items[name] = FileItem(uuid, name, path, type_name)
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])
        return items

    def getItemByUUID(self, uuid):
        """ return file item by its uuid
        @param[in] uuid - the requested uuid
        throw exception if there is no any file with the requested uuid
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            args = (self._server.uuid, )
            cursor.callproc('klk_file_list',  args )
            rows = cursor.fetchall()
            for row in rows:
                # klk_file.file, 
                # klk_file.name, 
                # klk_file.file_path, 
                # klk_file.type_uuid,
                # klk_file_type.type_name
                if row[0] == uuid:
                    name = row[1]
                    path = row[2]
                    type_name = row[4]
                    cursor.close()                
                    return FileItem(uuid, name, path, type_name)
            cursor.close()                
            # no item with the requested uuid was found
            raise common.KLKException,\
                "There is no file with the requested uuid: %s", uuid
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])
