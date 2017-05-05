"""@package server
   @file server.py
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
   - 2010/12/05 created by ipp (Ivan Murashko)
"""
import common
import factory
import klk

import os
import cmd

class Server(common.Object):
    """ The class represents a kalinka mediaserver 
    """
    
    def __init__(self, factory, uuid, name):
        """ Constructor
        """
        self._modules = {}
        modlist = common.getModules()
        for module_name in modlist:
            try:
                instance = __import__(module_name, globals(), locals(), ['*'], -1)
                if instance:
                    createFunc = getattr(instance, 'createModuleInstance')
                    if createFunc:
                        klkmod = createFunc(self)
                        self._modules[klkmod.name] = klkmod
            except (AttributeError, ImportError):
               #ignore import errors here
                pass

        common.Object.__init__(self, uuid, name)
        self._factory = factory

    @property
    def modules(self):
        """ Retrives a modules dict
        """
        return self._modules

    
class CLI(common.CLI):
    """ CLI for operations with servers

    There are the following operations possible
    show
    add
    delete
    set
    """
    
    def __init__(self, parent):
        """ Initialize the CLI for servers
        @param[in] parent - the parent CLI instance
        """

        common.CLI.__init__(self, parent, 'server', \
                                'Operations with servers (show, set default etc)')

        self._factory = klk.Factory()


    def do_show(self, line):
        """ Prints the list with available Servers
        """

        names = self.getServerNames()
        if len(names) == 0:
            print 'There is no any assigned servers yet\n'
            return

        item = ['name', 'connection', 'choosen']
        table = [item, ]
        
        for name in names:
            item = [name, '', '']
            table.append(item)

        self.cli_print(table)

    def do_add(self, line):
        """ Adds a new server
        usage: add <server_name>
        where 
        <server_name> is the internal Kalinka server name
        """
        # initial check
        args = line.split()
        if len(args) != 1:
            print 'Usage: add <server name> \n'
            return
        name = args[0]

        names = self.getServerNames()
        if name in names:
            print 'The server \'%s\' has been already added\n' % (name)
            return            
        
        try:
            self._factory.addServer(name)
        except klk.common.KLKException, e:
            print 'Error while add a new server: %s. %s' % (name, str(e))
            return
        print 'Server \'%s\' has been added' % name

        

    def complete_add(self, text, line, begidx, endidx):
        """ Add command completion        
        """
        pass

    def do_delete(self, line):
        """ Deletes the specified server
        Usage delete <server_name> 
        where
        <server_name> is the internal Kalinka server name
        """
        # initial check
        args = line.split()
        if len(args) != 1:
            print 'Usage: delete <server name> \n'
            return
        name = args[0]

        names = self.getServerNames()
        if name not in names:
            print 'The server \'%s\' is unknown\n' % (name)
            return            
        
        try:
            self._factory.delServer(name)
        except klk.common.KLKException, e:
            print 'Error while delete the server: %s. %s' % (name, str(e))
            return
        print 'Server \'%s\' has been deleted' % name


    def complete_delete(self, text, line, begidx, endidx):
        """ Delete command completion 
        @note to prevent a mistake in deletion
        the command does not have a completion
        """
        pass
        
    def getServerNames(self):
        """ Return a list with existent server names
        """
        names = []
        servers = self._factory.servers
        for server in servers.values():
            names.append(server.name)
        return names
