"""@package testcommon
   @file testcommon.py
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
   - 2011/01/06 created by ipp (Ivan Murashko)
"""

import klk.common
import unittest

class CommonTest(unittest.TestCase):
    """ Unit test for several common routines
    """
    def setUp(self):
        """ Prepare test data
        """
        pass
 
    def testModuleList(self):
        """ Tests the module retrieval test
        """
        modlist = klk.common.getModules()
        self.assert_('klk' in modlist)
        self.assert_('klk.adapter' in modlist)
        self.assert_('klk.adapter.ipc' in modlist)

    def testCLISuggest(self):
        """ Test klk.common.suggestName
        """        
        self.assert_(klk.common.suggestCLIName('prefix') == 'prefix1')
        self.assert_(klk.common.suggestCLIName('prefix1') == 'prefix2')
        self.assert_(klk.common.suggestCLIName('prefix24') == 'prefix25')
        self.assert_(klk.common.suggestCLIName('prefix123') == 'prefix124')

        self.assert_(klk.common.suggestCLIName('12prefix1') == '12prefix2')
        self.assert_(klk.common.suggestCLIName('pr12efix1') == 'pr12efix2')

        self.assert_(klk.common.suggestCLIName('/tmp/p1') == '/tmp/p2')

if __name__ == '__main__':
    unittest.main()
