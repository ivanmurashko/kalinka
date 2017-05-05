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

import ipaddr

import klk.common
import klk

import proto
import defines

class CLI(klk.common.CLI):
    """ CLI for operations with network routes
    """
    
    def __init__(self, parent):
        """ Initialize the CLI for servers
        @param[in] parent - the parent CLI instance
        """

        klk.common.CLI.__init__(self, parent, 'net', \
                                'Operations with network routes')

        self._protocols = dict()
        self._protocols[proto.TCPIP_NAME] = proto.TCPIP
        self._protocols[proto.UDP_NAME] = proto.UDP

    def do_show(self, line):
        """ Prints the list with available network routes
        """
        routes = self.getModule().routes
        if len(routes) == 0:
            print 'There is no any assigned network route yet\n'
            return

        item = ['name', 'dev', 'address', 'port', 'protocol', 'info']
        table = [item, ]
        
        for route in routes.values():
            inuse = 'in use' if route.inuse else ''
            item = [route.name, route.interface.name, route.addr, str(route.port), \
                        route.protocol.name, inuse]
            table.append(item)

        self.cli_print(table)

    def do_add(self, line):
        """ Adds a new network route
        """
        args = line.split()
        if len(args) == 4:
            args.append(proto.TCPIP_NAME)

        if len(args) != 5:
            print 'Usage: add <route name> <iface name> <IP addr> <port> <protocol>\n'
            return

        netmodule = self.getModule()
        routes = netmodule.routes
        
        #
        # check arguments
        #
        
        name = args[0]
        if name in routes.keys():
            print 'The route name \'%s\' has been already assigned' % name
            return
        
        interfaces = netmodule.interfaces
        
        iface_name = args[1]
        if not iface_name in interfaces.keys():
            print 'Incorrect interface name. Possible values are: ', \
                netmodule.interfaces.keys(), '\n'
            return
        iface = interfaces[iface_name]

        addr = args[2]
        try:
            ipaddr.IPv4Network(addr)
        except ipaddr.AddressValueError:
            print 'Incorrect ip address: %s' % addr
            return

        try:
            port = int(args[3])
            if port < 1 or port > 0xFFFF:
                print 'Incorrect port value. It should be in the following range: %d - %d' % \
                    (1, 0xFFFF)
                return
        except:
            print 'Incorrect port value:%s' % args[3]
            return

        protocol = None
        if args[4] in self._protocols.keys():
            protocol = self._protocols[ args[4] ]
        if protocol is None:
            print 'Incorrect protocol. Possible values are: ', \
                self._protocols.keys(), '\n'
            return

        if ipaddr.IPv4Network(addr).IsMulticast() and protocol != proto.UDP:
            print 'Incorrect protocol. \'%s\' is only possible for multicast address: %s\n' % \
                (proto.UDP_NAME, addr)
            return            
        
        try:
            netmodule.addRoute(name, iface, addr, port, protocol)
        except klk.common.KLKException, e:
            print 'Error while add a new route: %s. %s' % (name, str(e))
            return

        print 'Route \'%s\' has been added' % name
 

    def complete_add(self, text, line, begidx, endidx):
        """ Add command completion        
        """

        netmodule = self.getModule()
        routes = netmodule.routes
        args = line.split()

        #print '%d: %s\n' % (len(args), args)
        #print 'Text len: %d\n' % len(text)

        count = len(args)
        if len(text) > 0:
            count = count - 1

        # first arg is the route name
        if count == 1:
            return [i for i in self.getRouteName4Add() if i.startswith(text) ]            
        
        # second arg is iface name
        if count == 2:
            return [i for i in netmodule.interfaces.keys() if i.startswith(text) ]

        # third arg is ip address
        if count == 3:
            iface_name = args[2]
            if iface_name not in netmodule.interfaces.keys():
                return []
            iface = netmodule.interfaces[iface_name]
            addrs = (iface.addr, '239.0.0.1') 
            return [i for i in addrs if i.startswith(text) ]

        # forth arg is port
        if count == 4:
            ports = self.getPorts4Add(args[3])
            return [i for i in ports if i.startswith(text) ]

        # last arg is protocol
        if count == 5:
            protocols = self._protocols.keys()
            try:
                if ipaddr.IPv4Network(args[3]).IsMulticast():
                    protocols = [proto.UDP_NAME, ]
            finally:
                return [i for i in  protocols if i.startswith(text)]

    def do_delete(self, line):
        """ Deletes a previously assigned network route
        """

        args = line.split()

        if len(args) != 1:
            print 'Usage: delete <route name>\n'
            return

        netmodule = self.getModule()
        routes = netmodule.routes
        
        #
        # check arguments
        #
        
        name = args[0]
        if name not in routes.keys():
            print 'The route name \'%s\' has not been assigned' % name
            return

        try:
            netmodule.delRoute(name)
        except klk.common.KLKException, e:
            print 'Error while delete route: %s. %s' % (name, str(e))
            return

        print 'Route \'%s\' has been deleted' % name


    def complete_delete(self, text, line, begidx, endidx):
        """ Delete command completion        
        """
        return [i for i in self.getModule().routes.keys() if i.startswith(text) ]

    
    def getModule(self):
        """ @return the network module instance
        """
        factory = klk.Factory()
        server = factory.servers[factory.config.server]
        netmodule = server.modules[defines.NAME]
        return netmodule

    def getRouteName4Add(self):
        """ @return a list with possible routes names for add operations
        """
        result = []
        route_names =  self.getModule().routes.keys()

        for item in route_names:
            name = klk.common.suggestCLIName(item)
            if name not in route_names:                
                result.append(name)

        if len(result) == 0:
            result.append('route1')

        return result

    def getPorts4Add(self, addr):
        """ @return a list with ports for the specified address (not assigned ports)
        """        
        assigned_ports = []
        for route in self.getModule().routes.values():
            if route.addr == addr:
                assigned_ports.append(route.port)
        if len(assigned_ports) == 0:
            return ['20000',]
        maxport = max(assigned_ports)
        return [str(maxport+1),]
