"""@package testnet
   @file testnet.py
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

import klk.net
import klk.net.proto
import klk.db
import klk.server

import unittest
import os
import sys

import nettestdefines

class NetTest(unittest.TestCase):
    """ Network module unit test
    """

    def setUp(self):
        """ Creates test data
        """

        # Factory object
        config_file = os.path.join(klk.paths.INSTALL_PATH, 'etc/klk/klktest.conf')
        config = klk.config.Config(config_file)
        self._factory = klk.Factory()
        self._factory.config = config

        db = klk.db.DB(config)
        db.recreateDB()
        # fill DB with test data
        db.executeScript('testnetwork.sql')

    def testInterfaces(self):
        """ Network module: Do interfaces retrieval test
        """
        server = self._factory.servers[self._factory.config.server]
        self.assert_(server)        
        ifaces = server.modules[klk.net.NAME].interfaces
        self.assert_(ifaces)        
        self.assert_(len(ifaces) == 1)
        self.assert_(nettestdefines.IFACE_NAME in ifaces.keys())
        iface = ifaces[nettestdefines.IFACE_NAME]
        self.assert_(iface.name == nettestdefines.IFACE_NAME)
        self.assert_(len(iface.uuid) > 10)
        self.assert_(iface.addr == nettestdefines.IFACE_ADDR)
        # for key, iface in ifaces.iteritems():
        #     print '\n', key, '=>', \
        #         iface.name, ', ', iface.uuid, ', ', iface.addr, '\n'

    def testRoutes(self):
        """ Network module: routes info test
        """

        # initially we have 3 routes
        server = self._factory.servers[self._factory.config.server]
        self.assert_(server)        
        netmodule = server.modules[klk.net.NAME]
        routes = netmodule.routes
        self.assert_(routes)        
        self.assert_(len(routes) == 3)
        self.checkMain()

        # add additional route
        iface = netmodule.interfaces[nettestdefines.IFACE_NAME]
        name = 'route_new'
        addr = nettestdefines.IFACE_ADDR
        port = 54000
        proto = klk.net.proto.TCPIP
        netmodule.addRoute(name, iface, addr, port, proto)
        
        # check the addon
        routes = netmodule.routes
        self.assert_(routes)        
        self.assert_(len(routes) == 4)
        self.checkMain()
        self.assert_(name in routes.keys())
        route = routes[name]
        self.assert_(route.name == name)
        self.assert_(len(route.uuid) > 10)
        self.assert_(route.addr == addr)
        self.assert_(route.port == port)
        self.assert_(route.inuse == False)
        self.assert_(route.protocol.uuid == klk.net.proto.TCPIP)
        
        # check the del
        netmodule.delRoute(name)
        routes = netmodule.routes
        self.assert_(routes)        
        self.assert_(len(routes) == 3)
        self.checkMain()

    def checkMain(self):
        """ Checks that at least 3 routers are available
        """
        server = self._factory.servers[self._factory.config.server]
        self.assert_(server)        
        routes = server.modules[klk.net.NAME].routes
        self.assert_(routes)        
        self.assert_(len(routes) >= 3)
        self.assert_(nettestdefines.ROUTE1_NAME in routes.keys())
        self.assert_(nettestdefines.ROUTE2_NAME in routes.keys())
        self.assert_(nettestdefines.ROUTE3_NAME in routes.keys())

        route1 = routes[nettestdefines.ROUTE1_NAME]
        self.assert_(route1.name == nettestdefines.ROUTE1_NAME)
        self.assert_(route1.uuid == nettestdefines.ROUTE1_UUID)
        self.assert_(route1.addr == nettestdefines.ROUTE1_ADDR)
        self.assert_(str(route1.port) == nettestdefines.ROUTE1_PORT)
        self.assert_(route1.inuse == False)
        self.assert_(route1.protocol.uuid == klk.net.proto.TCPIP)

        route2 = routes[nettestdefines.ROUTE2_NAME]
        self.assert_(route2.name == nettestdefines.ROUTE2_NAME)
        self.assert_(route2.uuid == nettestdefines.ROUTE2_UUID)
        self.assert_(route2.addr == nettestdefines.ROUTE2_ADDR)
        self.assert_(str(route2.port) == nettestdefines.ROUTE2_PORT)
        self.assert_(route2.protocol.uuid == klk.net.proto.UDP)
        self.assert_(route2.inuse == False)        

        route3 = routes[nettestdefines.ROUTE3_NAME]
        self.assert_(route3.name == nettestdefines.ROUTE3_NAME)
        self.assert_(route3.uuid == nettestdefines.ROUTE3_UUID)
        self.assert_(route3.addr == nettestdefines.ROUTE3_ADDR)
        self.assert_(str(route3.port) == nettestdefines.ROUTE3_PORT)
        self.assert_(route3.protocol.uuid == klk.net.proto.UDP)
        # the only this route is locked
        self.assert_(route3.inuse == True)
        
        # check getItemByUUID
        uuid = route1.uuid
        checkroute = server.modules[klk.net.NAME].getItemByUUID(uuid)
        self.assert_(checkroute.name == nettestdefines.ROUTE1_NAME)
        self.assert_(checkroute.uuid == nettestdefines.ROUTE1_UUID)
        self.assert_(checkroute.addr == nettestdefines.ROUTE1_ADDR)
        self.assert_(str(checkroute.port) == nettestdefines.ROUTE1_PORT)
        self.assert_(checkroute.inuse == False)
        self.assert_(checkroute.protocol.uuid == klk.net.proto.TCPIP)
