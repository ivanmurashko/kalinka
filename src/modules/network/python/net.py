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
import klk

import uuid

from defines import *

import MySQLdb

class Interface(common.Object):
    """ The class represents a network iface aka eth0, lo etc.
    """
    
    def __init__(self, uuid, name, addr, addr_uuid):
        """ Constructor
        @param[in] name - iface name
        @param[in] uuid - iface uuid (id in the DB)
        @param[in] addr - the address assigned to the interface
        """
        common.Object.__init__(self, uuid, name)
        self._addr = addr
        self._addr_uuid = addr_uuid

    @property
    def addr(self):
        """ Retrives IP address asssigned to the iface
        """
        return self._addr

class Route(common.Object):
    """ The class holds info about a network route
    """

    def __init__(self, uuid, name, \
                     iface, addr, port, protocol):
        """ Constructor
        @param[in] name - route name
        @param[in] uuid - route uuid (id in the DB)
        @param[in] iface - iface assosiated with the route
        @param[in] addr - the address assigned to the route        
        @param[in] port - the port assigned to the route        
        @param[in] protocol - the protocol info (uuid, name)
        """
        common.Object.__init__(self, uuid, name)
        self._iface = iface
        self._addr = addr
        self._port = port
        self._protocol = protocol

    @property
    def addr(self):
        """ Retrives IP address asssigned to the route
        """
        return self._addr

    @property
    def port(self):
        """ Retrives IP port asssigned to the route
        """
        return self._port

    @property
    def interface(self):
        """ Retrives interface associated with the route
        """
        return self._iface

    @property
    def protocol(self):
        """ Retrives protocol assosiated with the route
        """
        return self._protocol

    @property
    def inuse(self):
        """ Is the route in use or not
        @return
        - True the route is in use by a module
        - False - the route is free for using
        """
        result = False
        try:
            db = klk.Factory().getDB()
            conn = db.connect()
            cursor = conn.cursor()
            return_value = 0
            args = (self.uuid, return_value)
            cursor.callproc('klk_network_route_getlock',  args )
            cursor.execute('SELECT @_klk_network_route_getlock_1')
            rows = cursor.fetchone()
            if rows[0] != 0:
                result = True 
            cursor.close()
        except:
            return False
        return result

class Net(common.Module):
    """ Network module
    """
    
    def __init__(self, server):
        """ Constructor
        @param[in] server - the parent object (the mediaserver 
        that keeps the network module)
        """
        common.Module.__init__(self, UUID, NAME, server)

    @property
    def interfaces(self):        
        """ Retrives a dict with network interfaces info. Key is the iface name
        """
        result = dict()
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            args = (self._server.uuid, )
            cursor.callproc('klk_network_resources_list',  args )
            rows = cursor.fetchall()
            for row in rows:
                name = row[0]
                uuid = row[2]
                addr = row[4]
                addr_uuid = row[6]
                result[name] = Interface(uuid, name, addr, addr_uuid)
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL connection error %d: %s" % (e.args[0], e.args[1])
        return result

    @property
    def routes(self):
        """ Retrives a dict with routes. Key is the route name
        """        
        routes = dict()
        # First of all get all ifaces
        ifaces = self.interfaces
        for iface in ifaces.values():
            try:
                db = self.getDB()
                conn = db.connect()
                cursor = conn.cursor()
                args = (iface.uuid, )
                cursor.callproc('klk_network_route_get',  args )
                rows = cursor.fetchall()
                for row in rows:
                    # klk_network_routes.name,
                    # klk_network_routes.route,
                    # klk_network_routes.host,
                    # klk_network_routes.port,
                    # klk_network_routes.protocol,
                    # klk_network_protocols.proto_name
                    name = row[0]
                    uuid = row[1]
                    addr = row[2]
                    port = row[3]
                    proto_uuid = row[4]
                    proto_name = row[5]
                    routes[name] = Route(uuid, name, iface, addr, port, \
                                             common.Object(proto_uuid, proto_name))
                cursor.close()
            except MySQLdb.Error, e:
                raise common.KLKException,\
                    "MySQL error %d: %s" % (e.args[0], e.args[1])
            
        return routes

    def getItemByUUID(self, uuid):
        """ Retrives a route item by its uuid
        @param[in] uuid - the uuid to be used
        """        
        # First of all get all ifaces
        ifaces = self.interfaces
        for iface in ifaces.values():
            try:
                db = self.getDB()
                conn = db.connect()
                cursor = conn.cursor()
                args = (iface.uuid, )
                cursor.callproc('klk_network_route_get',  args )
                rows = cursor.fetchall()
                for row in rows:
                    # klk_network_routes.name,
                    # klk_network_routes.route,
                    # klk_network_routes.host,
                    # klk_network_routes.port,
                    # klk_network_routes.protocol,
                    # klk_network_protocols.proto_name
                    if row[1] == uuid:
                        name = row[0]
                        addr = row[2]
                        port = row[3]
                        proto_uuid = row[4]
                        proto_name = row[5]
                        cursor.close()
                        return Route(uuid, name, iface, addr, port, \
                                         common.Object(proto_uuid, proto_name))
                cursor.close()
            except MySQLdb.Error, e:
                raise common.KLKException,\
                    "MySQL error %d: %s" % (e.args[0], e.args[1])
        # no item with the requested uuid was found
        raise common.KLKException,\
            "There is no network route with the requested uuid: %s", uuid

    def addRoute(self, name, iface, addr, port, proto):
        """ Adds a new route
        @param[in] name - the human readable route name
        @param[in] iface - the route's iface. see getInterfaces() for the list of possible ifaces
        @param[in] addr - the IP address for the route
        @param[in] port - the port assigned to the route
        @param[in] proto - the protocol uuid from netproto namespace (see netproto.py 
        for defenitions
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            # INOUT route VARCHAR(40),
            # IN name VARCHAR(40), 
            # IN address VARCHAR(40), 
            # IN host VARCHAR(50),
            # IN port INTEGER,
            # IN protocol VARCHAR(40),
            # OUT return_value INT 

            return_value = 0
            args = (None,  name, iface._addr_uuid, addr, port, proto, return_value )
            cursor.callproc('klk_network_route_add',  args )
            cursor.execute('SELECT @_klk_network_route_add_6')
            rows = cursor.fetchone()
            if rows[0] != 0:
                print rows[0]
                raise common.KLKException,\
                    "MySQL error: cannot add a route"

            conn.commit()            
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])
        
        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)

    def delRoute(self, name):
        """ Removes a route by its name
        @param[in] name - the human readable route name
        """
        routes = self.routes
        if name in routes.keys():
            route = routes[name]
            try:
                db = self.getDB()
                conn = db.connect()
                cursor = conn.cursor()
                return_value = 0
                args = (route.uuid, return_value, )
                cursor.callproc('klk_network_route_delete',  args )
                cursor.execute('SELECT @_klk_network_route_delete_1')
                rows = cursor.fetchone()
                if rows[0] != 0:
                    print rows[0]
                    raise common.KLKException,\
                        "MySQL error: cannot delete a route"

                conn.commit()
                cursor.close()
            except MySQLdb.Error, e:
                raise common.KLKException,\
                    "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)
