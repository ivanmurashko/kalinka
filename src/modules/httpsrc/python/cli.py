"""@package cli
   @file cli.py
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
   - 2011/01/06 created by ipp (Ivan Murashko)
"""

import os.path

import klk.common
import klk

import httpsrc
import defines

class CLI(klk.common.CLI):
    """ CLI for operations with HTTP sources
    """
    
    def __init__(self, parent):
        """ Initialize the CLI for servers
        @param[in] parent - the parent CLI instance
        """

        klk.common.CLI.__init__(self, parent, 'httpsrc', \
                                'Operations with HTTP sources (web cameras)')

    def do_show(self, line):
        """ Prints the list with available HTTP sources
        """
        sources = self.getModule().sources
        if len(sources) == 0:
            print 'There is no any assigned http sources yet\n'
            return

        item = ['name', 'address', 'login', 'password']
        table = [item, ]
        
        for source in sources.values():
            login = 'set' if len(source.login) else 'not set'
            pwd = 'set' if len(source.login) else 'not set'
            item = [source.name, source.address, login, pwd]
            table.append(item)

        self.cli_print(table)

    def do_add(self, line):
        """ Adds a new HTTP source
        Usage add <name> <address> [<login> <password>]
        where
        name - the source name
        address - the source http(s) address
        login - the authentication login (optional parameter)
        password - the authentication password (optional parameter) 
        """

        args = line.split()
        login = ''
        passwd = ''
        if len(args) < 2 or len(args) > 4:
            print 'Incorrect number of arguments'
            return

        name = args[0]
        addr = args[1]
        if len(args) > 2:
            login = args[2]
        if len(args) > 3:
            passwd = args[3]

        # check the args
        sources = self.getModule().sources
        if name in sources.keys():
            print 'Source name \'%s\' has been already assigned' % name
            return
            
        for item in sources.values():
            if item.address == addr:
                print 'Source addr \'%s\' has been already assigned to \'%s\'' \
                    % (item.address, item.name)
                return
                
        self.getModule().addSource(name, addr, login, passwd )
        print '\'%s\' assigned to \'%s\'' % (name, addr)

    def complete_add(self, text, line, begidx, endidx):
        """ Add command completion        
        """
        sources = self.getModule().sources
        args = line.split()

        #print '%d: %s\n' % (len(args), args)
        #print 'Text len: %d\n' % len(text)

        count = len(args)
        if len(text) > 0:
            count = count - 1

        # first arg is the name
        if count == 1:
            return [i for i in self.getSourceName4Add() if i.startswith(text) ]            
        
        # second arg is addr name
        if count == 2:
            return [i for i in ['http://',] if i.startswith(text) ]

        # third arg is login
        if count == 3:
            return [i for i in ['login',] if i.startswith(text) ]

        # forth arg is port
        if count == 4:
            return [i for i in ['passwd',] if i.startswith(text) ]

    def do_delete(self, line):
        """ Deletes the specified HTTP source
        Usage delete <name> 
        where
        <name> is an internal Kalinka HTTP source name
        """
        args = line.split()

        if len(args) != 1:
            print 'Usage: delete <source name>\n'
            return

        sources = self.getModule().sources
        
        #
        # check arguments
        #
        
        name = args[0]
        if name not in sources.keys():
            print 'The source name \'%s\' has not been assigned' % name
            return

        try:
            self.getModule().delSource(name)
        except klk.common.KLKException, e:
            print 'Error while delete source: %s. %s' % (name, str(e))
            return

        print 'Source \'%s\' has been deleted' % name


    def complete_delete(self, text, line, begidx, endidx):
        """ Delete command completion        
        """
        return [i for i in self.getModule().sources.keys() if i.startswith(text) ]

    def getModule(self):
        """ @return the file module instance
        """
        factory = klk.Factory()
        server = factory.servers[factory.config.server]
        httpsrc_module = server.modules[defines.NAME]
        return httpsrc_module


    def getSourceName4Add(self):
        """ @return a list with possible routes names for add operations
        """
        result = []
        source_names =  self.getModule().sources.keys()

        for item in source_names:
            name = klk.common.suggestCLIName(item)
            if name not in source_names:                
                result.append(name)

        if len(result) == 0:
            result.append('source1')

        return result
