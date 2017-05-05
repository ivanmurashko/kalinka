"""@package db
   @file db.py
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


import os

import MySQLdb

import paths
import common


class DB():
    """ Interface to Kalinka database
    """
    def __init__(self, config):
        """ Constructor
        @param[in] path - the path to config file with DB connection info
        """
        self._conn = None
        self._config = config.db

    def __del__(self):
        """ Destructor
        Closes the connection to DB if it was established
        """
        if self._conn is not None:
            self._conn.close()
        self._conn = None

    def connect(self):
        """ Connects to the DB        
        """
        if self._conn is None:
            try:
                self._conn = MySQLdb.connect (
                    host = self._config.host_name,
                    port = self._config.port,
                    user = self._config.user_name,
                    passwd = self._config.user_password,
                    db = self._config.name)
            except MySQLdb.Error, e:
                raise common.KLKException,\
                    "MySQL connection error %d: %s" % (e.args[0], e.args[1])
        return self._conn

    def executeScript(self, path):
        """ Executes an SQL script from a file (sets as path arg)
        """        
        cmd = paths.MYSQL + ' -u'  + self._config.user_name + \
            ' -p' + self._config.user_password  +  ' -h' +  self._config.host_name + \
            ' ' + self._config.name + \
           ' < ' + os.path.join(paths.SQLDATA_PATH, path)
        try:
            os.system(cmd)
        except Exception, err:
            raise common.KLKException,  \
                'There was an error while execute SQL script from : ' + \
                path + '. Error: ' +  str(err) 

    def recreateDB(self):
        """ Creates the DB internal structure. Clears existent data
        """
        scripts = ( 'server.sql', 'checkdb.sql', 'adapter.sql', 'network.sql', 'file.sql', \
                        'httpsrc.sql', 'ieee1394.sql', 'transcode.sql', 'http-streamer.sql')
        for script in scripts:
            self.executeScript(script)

