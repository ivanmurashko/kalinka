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

import klk.file
import klk.file.filetype
import klk.server

import unittest
import os
import sys


class FileTest(unittest.TestCase):
    """ Filework module unit test
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
        db.executeScript('testfile.sql')

    def testFiles(self):
        """ File module: files info test
        """

        # initially we don't have any files
        server = self._factory.servers[self._factory.config.server]
        self.assert_(server)        
        filemodule = server.modules[klk.file.NAME]
        files = filemodule.files
        self.assert_(files)        
        self.assert_(len(files) == 3)

        # add a file
        filemodule.addFile("pyfile1", "/tmp/file1.bin", klk.file.filetype.REGULAR)
        filemodule.addFile("pyfile2", "/tmp/file2.bin", klk.file.filetype.REGULAR)
        filemodule.addFile("pyfolder1", "/tmp/folder", klk.file.filetype.FOLDER)

        # test the result
        files = filemodule.files
        self.assert_(files)        
        self.assert_(len(files) == 6)
        self.assert_("pyfile1" in files.keys())
        self.assert_("pyfile2" in files.keys())
        self.assert_("pyfolder1" in files.keys())
        item = files["pyfile1"]
        self.assert_(item.name == "pyfile1")
        self.assert_(item.path == "/tmp/file1.bin")
        self.assert_(item.type_name == klk.file.filetype.REGULAR)
        item = files["pyfile2"]
        self.assert_(item.name == "pyfile2")
        self.assert_(item.path == "/tmp/file2.bin")
        self.assert_(item.type_name == klk.file.filetype.REGULAR)
        item = files["pyfolder1"]
        self.assert_(item.name == "pyfolder1")
        self.assert_(item.path == "/tmp/folder")
        self.assert_(item.type_name == klk.file.filetype.FOLDER)

        # check getItemByUUID
        uuid = item.uuid
        checkitem = filemodule.getItemByUUID(uuid)
        self.assert_(checkitem)
        self.assert_(checkitem.name == "pyfolder1")
        self.assert_(checkitem.path == "/tmp/folder")
        self.assert_(checkitem.type_name == klk.file.filetype.FOLDER)        

        # check the del
        filemodule.delFile("pyfile2")
        files = filemodule.files
        self.assert_(files)        
        self.assert_(len(files) == 5)
        self.assert_("pyfile1" in files.keys())
        self.assert_("pyfolder1" in files.keys())
        filemodule.delFile("pyfile1")
        files = filemodule.files
        self.assert_(files)        
        self.assert_(len(files) == 4)
        self.assert_("pyfolder1" in files.keys())
        filemodule.delFile("pyfolder1")
        files = filemodule.files
        self.assert_(files)        
        self.assert_(len(files) == 3)
        
