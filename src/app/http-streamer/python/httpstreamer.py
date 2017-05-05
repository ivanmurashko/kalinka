"""@package httpstreamer
   @file httpstreamer.py
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

from defines import *

import klk.common as common
import klk.net as net
import klk.adapter
import klk.factory

import MySQLdb

class InputRoute(common.Object):
    """ Holds info about input route
    """
    
    def __init__(self, net_route, path, media_uuid, media_name):
        """ Inits the route with values
        """
        common.Object.__init__(self, net_route.uuid, net_route.name)
        self._net_route = net_route
        self._path = path
        self._media_type = common.Object(media_uuid, media_name)

    @property
    def route(self):
        """ @return net route
        """
        return self._net_route

    @property
    def path(self):
        """ Return http path
        """
        return self._path
    
    @property 
    def media_type(self):
        """ Return media type info
        """
        return self._media_type

class HTTPStreamer(common.Application):
    """ HTTP streamer application module
    """

    def __init__(self, server):
        """ Constructor
        @param[in] server - the parent object (the mediaserver 
        that keeps the module)
        """        
        common.Application.__init__(self, UUID, NAME, server)

    @property
    def output_route(self):
        """ Retrieves output info (server bind info - address and port)
        """
        out_route_uuid = ''
        app_uuid = self.app_uuid
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            return_value = 0
            args = (app_uuid, out_route_uuid, return_value)
            cursor.callproc('klk_app_http_streamer_get',  args )
            cursor.execute('SELECT @_klk_app_http_streamer_get_1, @_klk_app_http_streamer_get_2')
            rows = cursor.fetchone()
            if rows[1] != 0:
                return None
            out_route_uuid = rows[0]
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        return self.getNetRoute(out_route_uuid)

    @output_route.setter
    def output_route(self, route):
        """ Output route setter. Sets a value for the output route
        @param[in] route - the network route object to be set
        """
        if self.app_uuid == None:
            self.activate('HTTP Streamer application')
        app_uuid = self.app_uuid

        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            return_value = 0
            args = (app_uuid, route.uuid, return_value)
            cursor.callproc('klk_app_http_streamer_update',  args )
            cursor.execute('SELECT @_klk_app_http_streamer_update_2')
            rows = cursor.fetchone()
            if rows[0] != 0:
                raise common.KLKException,\
                    "MySQL error: httpstreamer output route cannot be set"

            conn.commit()
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

    def getNetRoute(self, uuid):
        """ Retrieves network route by its uuid
        """
        netmodule = self._server.modules[net.NAME]
        for route in netmodule.routes.values():
            if route.uuid == uuid:
                return route
        return None
                
    @property 
    def input_routes(self):
        """ Retrieves info about input routes - aka network route + path + media info
        """
        result = dict()
        app_uuid = self.app_uuid
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            args = (app_uuid, )
            cursor.callproc('klk_app_http_streamer_route_get',  args )
            rows = cursor.fetchall()
            cursor.close()
            for row in rows:
	        # klk_app_http_streamer_route.in_route,
	        # klk_app_http_streamer_route.out_path,
		# klk_media_types.name,
		# klk_app_http_streamer_media_types.uuid                
                route = self.getNetRoute(row[0])
                path = row[1]
                media_name = row[2]
                media_uuid = row[3]
                result[route.name] = InputRoute(route, path, media_uuid, media_name)
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        return result
        
    def delInputRoute(self, name):
        """ Deletes an input route by its name
        """
        routes = self.input_routes
        if name in routes.keys():
            route = routes[name]
            try:
                db = self.getDB()
                conn = db.connect()
                cursor = conn.cursor()
                return_value = 0
                args = (name, return_value, )
                cursor.callproc('klk_app_http_streamer_route_del_byname',  args )
                cursor.execute('SELECT @_klk_app_http_streamer_route_del_byname_1')
                rows = cursor.fetchone()
                if rows[0] != 0:
                    print rows[0]
                    raise common.KLKException,\
                        "MySQL error: cannot delete a http streamer input route"

                conn.commit()
                cursor.close()
            except MySQLdb.Error, e:
                raise common.KLKException,\
                    "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)


    def addInputRoute(self, net_route, path, media_type):
        """ Adds an input route
        """
        app_uuid = self.app_uuid
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            
            # INOUT uuid VARCHAR(40),
            # IN application VARCHAR(40),
            # IN in_route VARCHAR(40),	
            # IN out_path VARCHAR(40),
            # IN media_type VARCHAR(40),
            # OUT return_value INT 
            uuid = None
            return_value = 0
            args = (uuid, app_uuid, net_route.uuid, path, media_type, return_value, )
            cursor.callproc('klk_app_http_streamer_route_add',  args )
            cursor.execute('SELECT @_klk_app_http_streamer_route_add_5')
            rows = cursor.fetchone()
            if rows[0] != 0:
                print rows[0]
                raise common.KLKException,\
                    "MySQL error: cannot add a http streamer input route"
            
            conn.commit()
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])
        
        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)
