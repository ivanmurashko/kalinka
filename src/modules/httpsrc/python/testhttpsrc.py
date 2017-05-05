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

import klk.httpsrc
import klk.server

import unittest
import os
import sys


class HTTPSourceTest(unittest.TestCase):
    """ HTTPSourcework module unit test
    """

    def setUp(self):
        """ Creates test data
        """

        # Factory object
        config_file = os.path.join(klk.paths.INSTALL_PATH, \
                                       'etc/klk/klktest.conf')
        config = klk.config.Config(config_file)
        self._factory = klk.Factory()
        self._factory.config = config

        db = klk.db.DB(config)
        db.recreateDB()
        # fill DB with test data
        db.executeScript('testhttpsrc.sql')

    def testHTTPSources(self):
        """ HTTPSource module: HTTP sources info test
        """

        # initially we have 2 sources
        server = self._factory.servers[self._factory.config.server]
        self.assert_(server)        
        module = server.modules[klk.httpsrc.NAME]
        sources = module.sources
        self.assert_(sources)        
        self.assert_(len(sources) == 2)

        # add several sources
        module.addSource('source1', 'http://webcam1/', 'login', '12345' )
        module.addSource('source2', 'http://webcam2/', '', '' )

        # test the result
        sources = module.sources
        self.assert_(sources)        
        self.assert_(len(sources) == 4)
        self.assert_('source1' in sources.keys())
        self.assert_('source2' in sources.keys())

        item = sources['source1']
        self.assert_(item.name == 'source1')
        self.assert_(item.address == 'http://webcam1/')
        self.assert_(item.login == 'login')
        self.assert_(item.passwd == '12345')

        item = sources['source2']
        self.assert_(item.name == 'source2')
        self.assert_(item.address == 'http://webcam2/')
        self.assert_(item.login == '')
        self.assert_(item.passwd == '')

        # check the del
        module.delSource('source1')
        sources = module.sources
        self.assert_(sources)        
        self.assert_(len(sources) == 3)
        self.assert_('source1' not in sources.keys())
        self.assert_('source2' in sources.keys())

        module.delSource('source2')
        sources = module.sources
        self.assert_(sources)        
        self.assert_(len(sources) == 2)
        self.assert_('source1' not in sources.keys())
        self.assert_('source2' not in sources.keys())
        
