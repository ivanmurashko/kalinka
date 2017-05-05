"""@package config
   @file config.py
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

import common
import paths

import re

class DBConfig():
    """ The class keeps info about DB connection to the 
    Kalinka main database
    """
    def __init__(self):
        """ Constructor
        """
        #default values for properties
        self.host_name = 'localhost'
        self.port = 0
        self.name = 'kalinka'
        self.user_name = 'root'
        self.user_password = ''


    def updateData(self, text):
        """  Parses data string and extracts db connection settings
        from it.

        The data format is tyhe following:
        user:passwd@host:port/dbname/
        """
        # parse user:passwd@host:port/dbname/
        setting_m = re.search('([^@]+)@([^/]+)/([^/]+)', text)
        if setting_m:
            self.setUserInfo(setting_m.group(1))
            self.setConnectionInfo(setting_m.group(2))
            self.name = setting_m.group(3)               
        else:
            raise common.KLKException,  'Invalid Database setting value: ' + text

    def setUserInfo(self, text):
        """ Sets user info realted fields from the string pass as the arg
        """
        m = re.search('([^:]+):([^:]+)', text)
        if m:
            self.user_name = m.group(1)
            self.user_password = m.group(2)
        else:
            self.user_name = text
            self.user_password = ''             

    def setConnectionInfo(self, text):
        """ Sets connection info realted fields from the string pass as the arg
        """
        m = re.search('([^:]+):([\d]+)', text)
        if m:
            self.host_name = m.group(1)
            self.port = int(m.group(2))
        else:
            self.host_name = text
            self.port = 0


class Config():
    """ Configuration info for Kalinka media server
    It provides 2 proiperty
    - server - the current server name from config file
    - db - the db connection settings
    """
    
    def __init__(self, path = None):
        """ Initializes the config with default values
        """
        self.server = ''
        self.db = DBConfig()
        if path:
            self.load(path)

    def load(self, path = None):
        """ Load data from a config file
        @param[in] path - the path to config file
        """
        if path is None or len(path) == 0:
            path = paths.CONFIG

        config = None
        try:
            config = open(path, "r")
            for line in config.readlines():
                item = line.strip(' \r\n\t')
                if item[0:1] == '#':
                    continue
                option_m = re.search('([^\t ]+)[\t ]+([^\t ]+)', item)
                if not option_m:
                    continue
                elif option_m.group(1).lower() == 'Database'.lower():
                    self.db.updateData(option_m.group(2))
                elif option_m.group(1).lower() == 'HostName'.lower():
                    self.server = option_m.group(2)
            config.close()
        except Exception, exc:
            if config is not None:
                config.close()
            raise common.KLKException,  'There was an error while retrive info from : ' + \
                path + '. ' +  str(exc)
        
