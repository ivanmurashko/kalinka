"""@package common
   @file common.py
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
   - 2010/12/03 created by ipp (Ivan Murashko)
"""

import MySQLdb
import cmd
import os
import sys
import re

import paths
import factory

def getModules():
    """ retrives a list with klk modules names
    @return the list with module names    
    """
    result = []
    # look for all possible children
    for root, dirnames, filenames in os.walk(paths.ROOT):
        for filename in filenames:
            if filename == '__init__.py':
                # collect such paths
                module_name = root.replace(paths.ROOT + '/', '') .replace('/', '.')
                result.append(module_name)
    return result

class KLKException(Exception):
    """KLK exception"""
    pass

class Object(object):
    """ Base class for most part of Kalinka mediaserver objects 
    @note we derive it from object to be able use properties
    """

    def __init__(self, uuid,  name):
        """ Each Kalinka object has a human readable name and unique uuid that 
        are set via the constructor
        """
        self._uuid = uuid
        self._name = name

    @property
    def name(self):
        """ @return the human readable name for the object
        """
        return self._name

    @property
    def uuid(self):
        """ @return  the object's uuid
        """
        return self._uuid

class Module(Object):
    """ Base class for all modules
    """

    def __init__(self, uuid, name, server):
        """ The module constructor
        Each module has uuid, name and server instance
        """
        Object.__init__(self, uuid, name)
        self._server = server

    def getDB(self):
        """ @return DB connector instance
        """
        return factory.Factory().getDB()

class Application(Module):
    """ The application class instance
    """
    
    def __init__(self, uuid, name, server):
        """ Application is same to module but additionally it has na application uuid
        to retrive application specific parameters
        """
        Module.__init__(self, uuid, name, server)
        self._app_uuid = None

    @property
    def app_uuid(self):
        """ Retrieves application uuid. The uuid is used later to get application settings
        """
        if self._app_uuid == None:
            try:
                db = self.getDB()
                conn = db.connect()
                cursor = conn.cursor()
                app_uuid = ''
                args = (self.uuid, self._server.uuid, app_uuid)
                cursor.callproc('klk_application_uuid_get',  args )
                cursor.execute('SELECT @_klk_application_uuid_get_2')
                rows = cursor.fetchone()
                self._app_uuid = rows[0]
            except MySQLdb.Error, e:
                raise KLKException,\
                    "MySQL error %d: %s" % (e.args[0], e.args[1])
        return self._app_uuid

    def activate(self, description):
        """ Activates the application
        @param[in] description - the application description
        """
        if self.app_uuid != None:
            raise KLKException,\
                    "Application '%s' has been already activated" % \
                    self.name
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            app_uuid = ''

            # INOUT application VARCHAR(40),
            # IN module VARCHAR(40),
            # IN name VARCHAR(50),
            # IN enabled TINYINT,
            # IN status TINYINT,
            # IN priority INTEGER,
            # IN host VARCHAR(40),
            # IN description VARCHAR(255),
            # OUT return_value INTEGER
            app_uuid = None
            res = None
            args = (app_uuid, self.uuid, self.name, 1, 0, 0, \
                        self._server.uuid, description, res)
            cursor.callproc('klk_application_add',  args )
            cursor.execute('SELECT @_klk_application_add_0, @_klk_application_add_8')
            rows = cursor.fetchone()
            if rows[1] != 0:
                raise common.KLKException,\
                    "MySQL error: cannot activate application: %s" % self.name
            self._app_uuid = rows[0]
            conn.commit()            
            cursor.close()
        except MySQLdb.Error, e:
            raise KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

class CLI(cmd.Cmd):
    """ Base class for CLI sub menus
    """

    def __init__(self, parent, name, docstring):
        """ Inits the CLI
        @param[in] parent - the parent CLI
        @param[in] name - the sub CLI name
        @param[in] docstring - the documentation for the CLI sub menu
        """
        
        self.prompt = '%s:%s> ' % (parent.prompt[:-2], name)

        self._name = name
        self._docstring = docstring

        cmd.Cmd.__init__(self)

    def cli_print(self, table):
        """ Prints a CLI info with aligment
        @param[in] table - the table with data to be printed
        """
        for row in table:
            result = ''
            for i in range(len(row)):
                aligment = max( [x[i] for x in [map (len, x) for x in table]] ) + 1
                result = result + row[i].ljust(aligment)
            print result


    def do_exit(self, line):
        """ Exit the sub CLI
        """
        return True

    def do_quit(self, line):
        """ Quit the CLI programm
        """
        print 'Thank you for using Kalinka mediaserver CLI'
        sys.exit(0)

    def printhelp_cmd(self):
        print ' %s\n' % self._docstring

    @property
    def name(self):
        """ CLI name
        """
        return self._name

def suggestCLIName(prefix):
    """ Suggest a new name for CLI
    @param[in] prefix - the name prefix to be used
    Examples 
    route => route1
    route123 => route124 (124 = 123 + 1)
    """
    count = 1
    start = prefix
    parse = re.search('(.*)([^\d])([\d]+)$', prefix)
    if parse:
        start = parse.group(1) + parse.group(2)
        count = int(parse.group(3)) + 1
    return '%s%d' % (start, count)
