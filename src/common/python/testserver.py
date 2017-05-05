"""@package testserver
   @file testserver.py
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

import klk
import klk.common
import klk.config
import klk.server

import unittest
import os

class ServerTest(unittest.TestCase):
    """ Network module unit test
    """

    def setUp(self):
        """ Creates test data
        """
        config = klk.config.Config(os.path.join(klk.paths.INSTALL_PATH,  'etc/klk/klktest.conf'))
        self._factory = klk.Factory()
        self._factory.config = config

        db = klk.db.DB(config)
        db.recreateDB()
        # fill DB with test data
        db.executeScript('testnetwork.sql')

    def test(self):
        """ Server: main test that retrieves a list of 
        available servers from the DB
        """
        servers = self._factory.servers
        # there are should be 2 servers
        self.assert_(len(servers) == 3)
        self.assert_('testserver' in servers.keys())
        self.assert_('testserver_add' in servers.keys())
        self.assert_('testserver_ext' in servers.keys())
        # just the container structure test
        for key, srv in servers.iteritems():
            self.assert_(key == srv.name)
            self.assert_(len(srv.uuid) > 10)

