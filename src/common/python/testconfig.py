"""@package testconfig
   @file testconfig.py
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
   - 2010/12/03 created by ipp (Ivan Murashko)
"""

import klk.config
import unittest

class ConfigTest(unittest.TestCase):
    """ Unit test for retriving DB config info from config file
    """
    def setUp(self):
        """ Prepare test data
        """
        pass
 
    def testInfo(self):
        """ Test config info retrieval
        
        The test file keeps the following strings

        HostName  confunittest_servername
        Database  klkdb_user:klkdb_pwd@klkdbhost:1234/klkdbname/
        """

        config = klk.config.Config()
        config.load(klk.paths.TESTDATA_PATH + 'conf.test')
        self.assert_(config.server == 'confunittest_servername')
        self.assert_(config.db.host_name == 'klkdbhost')
        self.assert_(config.db.port == 1234)
        self.assert_(config.db.name == 'klkdbname')
        self.assert_(config.db.user_name == 'klkdb_user')
        self.assert_(config.db.user_password == 'klkdb_pwd')


if __name__ == '__main__':
    unittest.main()
