"""@package testadapter
   @file testadapter.py
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
   - 2010/12/25 created by ipp (Ivan Murashko)
"""

import klk.db
import klk.server
import klk.adapter
import klk.net
import klk.httpstreamer

import unittest
import os
import sys

import adaptertestdefines

class AdapterTest(unittest.TestCase):
    """ Adapter module unit test
    """

    def setUp(self):
        """ Creates test data for the unit test
        """
        # Factory object
        config_file = os.path.join(klk.paths.INSTALL_PATH, 'etc/klk/klktest.conf')
        config = klk.config.Config(config_file)
        self._factory = klk.Factory()
        self._factory.config = config

        db = klk.db.DB(config)
        db.recreateDB()
        # fill DB with test data
        db.executeScript('testadapter.sql')

    def testEndpoint(self):
        """ The unit test for endpoints retrieval
        """
        server = self._factory.servers[self._factory.config.server]
        self.assert_(server)        
        adapter = server.modules[klk.adapter.NAME]

        # Test for the module endpoint retrieval
        net_endpoint = adapter.getEndpoint(klk.net.UUID)
        self.assert_(net_endpoint)        
        self.assert_(net_endpoint == adaptertestdefines.MOD_ENDPOINT)        

        adapter_endpoint = adapter.getEndpoint(klk.adapter.UUID)
        self.assert_(adapter_endpoint)        
        self.assert_(adapter_endpoint == adaptertestdefines.MOD_ENDPOINT)        

        # should be same endpoint
        self.assert_(adapter_endpoint == net_endpoint)

        # application endpoint retrieval
        app_endpoint  =  adapter.getEndpoint(klk.httpstreamer.UUID)
        self.assert_(app_endpoint)        
        self.assert_(app_endpoint == adaptertestdefines.APP_ENDPOINT)        
        self.assert_(app_endpoint != net_endpoint)


