"""@package testhttpstreamer
   @file testhttpstreamer.py
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

import klk.net
import klk.db
import klk.server
import klk.httpstreamer
import klk.mediatypes

import unittest
import os
import sys

import httpstreamertestdefines

class HTTPStreamerTest(unittest.TestCase):
    """ HTTP streamer application unit test
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
        db.executeScript('testhttpstreamer_tcpip.sql')

    def testStreamer(self):
        """ HTTP streamer: Base test
        """
        server = self._factory.servers['testserver']
        self.assert_(server)        

        netmodule = server.modules[klk.net.NAME]
        netroutes = netmodule.routes
        self.assert_(netroutes)        
        
        # 2 input routes + 2 output
        self.assert_(len(netroutes) == 4)
        self.assert_(httpstreamertestdefines.ROUTE1_NAME in netroutes.keys())
        self.assert_(httpstreamertestdefines.ROUTE2_NAME in netroutes.keys())
        self.assert_(httpstreamertestdefines.OUTPUT_NAME_ADD in netroutes.keys())

        httpstreamer = server.modules[klk.httpstreamer.NAME]
        
        output_info = httpstreamer.output_route
        self.assert_(output_info.addr == httpstreamertestdefines.OUTPUT_ADDR)
        self.assert_(output_info.port == httpstreamertestdefines.OUTPUT_PORT)

        # 2 assigned routes
        routes = httpstreamer.input_routes
        self.assert_(len(routes) == 2)
        self.assert_(httpstreamertestdefines.ROUTE1_NAME in routes.keys())        
        self.assert_(httpstreamertestdefines.ROUTE2_NAME in routes.keys())        

        # del one
        httpstreamer.delInputRoute(httpstreamertestdefines.ROUTE1_NAME)
        routes = httpstreamer.input_routes
        self.assert_(len(routes) == 1)
        self.assert_(httpstreamertestdefines.ROUTE1_NAME not in routes.keys())        
        self.assert_(httpstreamertestdefines.ROUTE2_NAME in routes.keys())        
        
        # add one
        netroute1 = netroutes[httpstreamertestdefines.ROUTE1_NAME]
        path1 = '/path1'
        mediatype1 = klk.mediatypes.FLV
        httpstreamer.addInputRoute(netroute1, path1, mediatype1)
        routes = httpstreamer.input_routes
        self.assert_(len(routes) == 2)
        self.assert_(httpstreamertestdefines.ROUTE1_NAME in routes.keys())        
        route = routes[httpstreamertestdefines.ROUTE1_NAME]
        self.assert_(route.name == httpstreamertestdefines.ROUTE1_NAME)      
        self.assert_(route.route.name ==\
                         httpstreamertestdefines.ROUTE1_NAME)      
        self.assert_(route.path == path1)      
        self.assert_(route.media_type.uuid == mediatype1)      

        # update output route
        self.assert_(httpstreamertestdefines.OUTPUT_PORT != \
                         httpstreamertestdefines.OUTPUT_PORT_ADD)

        netroute_add = netroutes[httpstreamertestdefines.OUTPUT_NAME_ADD]
        httpstreamer.output_route = netroute_add
        output_info = httpstreamer.output_route
        self.assert_(output_info.name == httpstreamertestdefines.OUTPUT_NAME_ADD)
        self.assert_(output_info.addr == httpstreamertestdefines.OUTPUT_ADDR_ADD)
        self.assert_(output_info.port == httpstreamertestdefines.OUTPUT_PORT_ADD)
        
if __name__ == '__main__':
    unittest.main()
