#!/usr/bin/env python

import sys

import klk.config
import klk.net
import klk.file
import klk.server
import klk.httpstreamer
import klk.transcode
import klk.mediatypes

class Demo():
    """ The Demo class that prints current HTTP streamer state
    It also adds a route and delete the added route
    """
    
    def __init__(self, config_file):
        """
        @param[in] config_file -  the path to config file
        """

        # the name is used as the route name
        self._ext_route_name = 'test_route'
        self._int_route_name = 'internal_route'
        self._file_name = 'testfile'

        self._in_source_name = None
        self._out_source_name = None
        self._file_source_name = None
        self._net_task_name = 'nettask'
        self._file_task_name = 'filetask'

        config = klk.config.Config(config_file)
        self._factory = klk.Factory()
        self._factory.config = config

        # initial cleanup
        self.delRoute()

    
    def printState(self):
        """
        Prints the current state
        """

        # the kalinka mediaserver
        server = self._factory.servers['testserver']

        #
        # Transcoder
        #

        print 'Transcoder:', '\n'
        transcode = server.modules[klk.transcode.NAME]
        for task in transcode.tasks.values():
            print 'Task %s: %s => %s \n' % \
                ( task.name, task.input_source, task.output_source ) 
        

        #
        # http streamer application
        #
        httpstreamer = server.modules[klk.httpstreamer.NAME]

        # output route aka bind info
        output_route = httpstreamer.output_route 
        
        # input routes
        routes = httpstreamer.input_routes
        
        print 'HTTP streamer:', '\n'
        for route in routes.values():
            print 'Route: ', route.route.addr, ':', \
                route.route.port, ' => ', \
                output_route.addr, ':', output_route.port, \
                '/', route.path, '\n'


    def addRoute(self):
        """
        Adds a route
        """

        # the kalinka mediaserver
        server = self._factory.servers['testserver']

        #
        # Assign network interface
        #

        # network module
        netmodule = server.modules[klk.net.NAME]

        # get available interfaces
        ifaces = netmodule.interfaces
        # ifaces have the names in the following format
        # dev_name@host => eth0@testserver
        iface = ifaces['eth0@testserver']

        # add external network route
        name = self._ext_route_name
        addr = iface.addr
        port = 54003
        proto = klk.net.proto.UDP
        netmodule.addRoute(name, iface, addr, port, proto)

        # add internal network route
        name = self._int_route_name
        addr = iface.addr
        port = 54004
        proto = klk.net.proto.TCPIP
        netmodule.addRoute(name, iface, addr, port, proto)

        # 
        # File module
        #
        filemodule = server.modules[klk.file.NAME]
        name = self._file_name
        path = '/tmp/folder'
        filemodule.addFile(name, path, klk.file.filetype.FOLDER)

        #
        # Transcode tasks
        #
        transcode = server.modules[klk.transcode.NAME]

        # add sources
        self._in_source_name = \
            transcode.addSource(self._ext_route_name, klk.transcode.sourcetype.NET, \
                                    klk.mediatypes.RTP_MPEGTS)
        self._out_source_name = \
            transcode.addSource(self._int_route_name, klk.transcode.sourcetype.NET, \
                                    klk.mediatypes.AUTO)

        self._file_source_name = \
            transcode.addSource(self._file_name, klk.transcode.sourcetype.FILE, \
                                    klk.mediatypes.AUTO)
        
        # link the sources into a task
        transcode.addTask(self._net_task_name, self._in_source_name, \
                              self._out_source_name, \
                              klk.transcode.videosize.DEFAULT, \
                              klk.transcode.videoquality.DEFAULT, \
                              klk.transcode.schedule.ALWAYS, 0)

        transcode.addTask(self._file_task_name, self._in_source_name, \
                              self._file_source_name, \
                              klk.transcode.videosize.DEFAULT, \
                              klk.transcode.videoquality.DEFAULT, \
                              klk.transcode.schedule.ALWAYS, 120)

        #
        # Assign path on http-streamer
        #

        # http streamer application
        httpstreamer = server.modules[klk.httpstreamer.NAME]

        net_route = netmodule.routes[self._int_route_name]
        path = '/path3'
        mediatype = klk.mediatypes.MPEGTS
        httpstreamer.addInputRoute(net_route, path, mediatype)

    def delRoute(self):
        """
        Deletes a route
        """
        # the kalinka mediaserver
        server = self._factory.servers['testserver']

        # delete route on http streamer
        httpstreamer = server.modules[klk.httpstreamer.NAME]
        httpstreamer.delInputRoute(self._int_route_name)

        # delete on transcode
        transcode = server.modules[klk.transcode.NAME]        

        # tasks
        if self._net_task_name in transcode.tasks.keys():
            transcode.delTask(self._net_task_name)
        if self._file_task_name in transcode.tasks.keys():            
            transcode.delTask(self._file_task_name)

        # sources
        if self._in_source_name in transcode.sources.keys():            
            transcode.delSource(self._in_source_name)
        if self._out_source_name in transcode.sources.keys():            
            transcode.delSource(self._out_source_name)
        if self._file_source_name in transcode.sources.keys():            
            transcode.delSource(self._file_source_name)
        
        # delete route on network module
        netmodule = server.modules[klk.net.NAME]
        netmodule.delRoute(self._int_route_name)
        netmodule.delRoute(self._ext_route_name)
       
        # delete on file
        filemodule = server.modules[klk.file.NAME]
        filemodule.delFile(self._file_name)

if __name__ == '__main__':
    config_name = sys.argv[1]
    
    demo = Demo(config_name)

    print '\nInitial state: \n'
    demo.printState()

    print '\nAdd route ... \n'
    demo.addRoute()
    
    print '\nState: \n'
    demo.printState()

    print '\nDel route ... \n'
    demo.delRoute()

    print '\nFinal state: \n'
    demo.printState()
