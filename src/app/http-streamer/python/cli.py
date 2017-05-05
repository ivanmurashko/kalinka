"""@package cli
   @file cli.py
   @author Ivan Murashko <ivan.murashko@gmail.com>

   Copyright (c) 2007-2011 Kalinka Team

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
   - 2011/05/14 created by ipp (Ivan Murashko)
"""

import os.path

import klk.common
import klk
import klk.net
import klk.mediatypes

import httpstreamer
import defines

class CLI(klk.common.CLI):
    """ CLI for operations with files
    """
    
    def __init__(self, parent):
        """ Initialize the CLI for servers
        @param[in] parent - the parent CLI instance
        """

        klk.common.CLI.__init__(self, parent, 'httpstreamer', \
                                'HTTP streamer application')

        # possible media types
        self._media_types = dict()
        self._media_types[ klk.mediatypes.TXT_NAME ] = klk.mediatypes.TXT
        self._media_types[ klk.mediatypes.MPEGTS_NAME ] = klk.mediatypes.MPEGTS
        self._media_types[ klk.mediatypes.RTP_MPEGTS_NAME ] = \
            klk.mediatypes.RTP_MPEGTS
        self._media_types[ klk.mediatypes.FLV_NAME ] = klk.mediatypes.FLV
        self._media_types[ klk.mediatypes.OGG_NAME ] = klk.mediatypes.OGG

    def do_set(self, line):
        """ Sets the http-streamer output route info
        Usage: set <route name>
        """
        args = line.split()
        if len(args) != 1:
            print 'Usage: set <route name>'
            return

        name = args[0]
        
        #
        # check arguments
        #
        possible_names = self.getPossibleOutrouteNames()
        if len(possible_names) == 0:
            print 'The route name \'%s\' is invalid.' % name
            print 'No network routes assigned'
            return

        if name not in possible_names:
            print 'The route name \'%s\' is invalid. Possible names:' % name
            print possible_names
            return

        netmodule = self.getNetModule()
        httpstreamer = self.getModule()
        try:
            httpstreamer.output_route = netmodule.routes[name]
        except klk.common.KLKException, e:
            print 'Error while set output route: %s.' % name
            print str(e)
            return
        
        print 'Route \'%s\' has been set as output' % name

    def complete_set(self, text, line, begidx, endidx):
        return [i for i in self.getPossibleOutrouteNames() \
                    if i.startswith(text) ] 

    def do_show(self, line):
        """ Prints the assigned httpstreamer info
        """
        httpstreamer = self.getModule()
        output_route = httpstreamer.output_route
        if output_route == None:
            print 'The http streamer application has not been configured yet'
            print 'Use set command for initial http streamer configuration'
            print 'For more info type \'help set\''
            return

        routes = httpstreamer.input_routes
        if len(routes) == 0:
            print 'There is no any assigned path yet'
            return

        item = ['source', 'destination', 'type']
        table = [item, ]
        
        for route in routes.values():
            path = route.path
            media_type = route.media_type.name
            source = '%s://%s:%d' % ( route.route.protocol.name,\
                                       route.route.addr, route.route.port )
            destination = 'http://%s:%d/%s' % ( \
                output_route.addr, output_route.port, route.path )
            item = [source, destination, media_type]
            table.append(item)

        self.cli_print(table)

    def do_add(self, line):
        """ Adds a new path
        Usage: add <route name> <path> <media type>
        where 
        - route name - network route name that is used as data source
        - path - the assigned path to be used by clients
        - media type - the media data type
        """
        args = line.split(' ')
        if len(args) != 3:
            print 'Usage: add <route name> <path> <media type>'
            return
        #
        # Check args
        #
        possible_names = self.getPossibleInrouteNames()
        if len(possible_names) == 0:
            print 'The route name \'%s\' is invalid: '
            'No network routes assigend' % name
            return

        name = args[0]
        path = args[1]
        media_type_name = args[2]
        media_type = None

        # check route name
        if name not in possible_names:
            print 'The route name \'%s\' is invalid. Possible names:' % name
            print possible_names
            return

        # check path
        if path in self.getAssignedPaths():
            print 'The path \'%s\' has been already assigned' % path
            return
            
        # check media type
        if media_type_name in self._media_types.keys():
            media_type = self._media_types[ media_type_name ]
        if media_type == None:
            print 'The media type \'%s\' is invalid. Possible names:' \
                % media_type_name
            print self._media_types.keys()

        netmodule = self.getNetModule()
        httpstreamer = self.getModule()
        try:
            httpstreamer.addInputRoute( netmodule.routes[name], \
                                            path, \
                                            media_type )
        except klk.common.KLKException, e:
            print 'Error while adding an input route: %s.' % name
            print str(e)
            return
        
        print 'Route \'%s\' has been set as input' % name


    def complete_add(self, text, line, begidx, endidx):
        """ Add command completion        
        """
        args = line.split()

        #print '%d: %s\n' % (len(args), args)
        #print 'Text len: %d\n' % len(text)

        count = len(args)
        if len(text) > 0:
            count = count - 1

        # first arg is the route name
        if count == 1:
            return [i for i in self.getPossibleInrouteNames() \
                        if i.startswith(text) ]  

        route_name = args[1]

        # second arg is path
        if count == 2:
            return [i for i in self.getPossiblePath(route_name) \
                        if i.startswith(text) ]

        # third arg is media type
        if count == 3:
            return [i for i in self._media_types.keys() if i.startswith(text) ]

    def do_delete(self, line):
        """ Deletes the specified route
        Usage: delete <route name>
        """
        args = line.split()
        if len(args) != 1:
            print 'Usage: delete <route name>'
            return

        name = args[0]
        
        #
        # check arguments
        #
        possible_names = self.getAssignedInRoutes()
        if len(possible_names) == 0:
            print 'The path name \'%s\' is invalid.' % name
            print 'No paths assigned'
            return

        if name not in possible_names:
            print 'The path name \'%s\' is invalid. Possible paths:' % name
            print possible_names
            return

        httpstreamer = self.getModule()
        try:
            httpstreamer.delInputRoute(name)
        except klk.common.KLKException, e:
            print 'Error while delete input route: %s.' % name
            print str(e)
            return
        
        print 'Route \'%s\' has been deleted output' % name

    def complete_delete(self, text, line, begidx, endidx):
        """ Delete command completion        
        """
        return [i for i in self.getAssignedInRoutes() \
                    if i.startswith(text) ] 

    def getModule(self):
        """ @return the httpstreamer app module instance
        """
        factory = klk.Factory()
        server = factory.servers[factory.config.server]
        httpstreamer = server.modules[defines.NAME]
        return httpstreamer

    def getNetModule(self):
        """ @return the network module instance
        """
        factory = klk.Factory()
        server = factory.servers[factory.config.server]
        netmodule = server.modules[klk.net.NAME]
        return netmodule

    def getPossibleOutrouteNames(self):
        """ @return a list with possible names for output route
        """
        assigned_route = None
        if self.getModule().output_route != None:
            assigned_route = self.getModule().output_route.name
        routes = []
        for route in self.getNetModule().routes.values():
            if route.protocol.uuid == klk.net.proto.TCPIP and \
                    route.name != assigned_route:
                routes.append(route.name)
        return routes
                
    def getPossibleInrouteNames(self):
        """ @return a list with possible (exist and has not been assigned yet)
        input routes names
        """
        assigned_routes = []
        if self.getModule().input_routes != None:
            assigned_routes = self.getModule().input_routes.keys()
        if self.getModule().output_route != None:
            assigned_routes.append(self.getModule().output_route.name)
        routes = []
        for route in self.getNetModule().routes.values():
            if route.protocol.uuid == klk.net.proto.TCPIP and \
                    route.name not in assigned_routes:
                routes.append(route.name)
        return routes

    def getAssignedInRoutes(self):
        """ @return the list of assigned input routes
        """
        return self.getModule().input_routes.keys()

    def getAssignedPaths(self):
        """ @return the list with already assigned paths
        """
        assigned_paths = []
        for route in self.getModule().input_routes.values():
            assigned_paths.append(route.path)
        return assigned_paths

    def getPossiblePath(self, hint):
        """ @return the possible path based on the hint
        """
        possible_paths = []
        assigned_paths = self.getAssignedPaths()
        test_path = hint

        while test_path in assigned_paths:
            test_path = klk.common.suggestCLIName( test_path )
        possible_paths.append( test_path )
        return possible_paths
        
        
