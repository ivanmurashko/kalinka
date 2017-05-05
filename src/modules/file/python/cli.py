"""@package cli
   @file cli.py
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

import os.path

import klk.common
import klk

import klkfile
import filetype
import defines

class CLI(klk.common.CLI):
    """ CLI for operations with files
    """
    
    def __init__(self, parent):
        """ Initialize the CLI for servers
        @param[in] parent - the parent CLI instance
        """

        klk.common.CLI.__init__(self, parent, 'file', \
                                'Operations with files')
        self._file_types = (filetype.REGULAR, filetype.FOLDER)

    def do_show(self, line):
        """ Prints the list with available files
        """
        files = self.getModule().files
        if len(files) == 0:
            print 'There is no any assigned files yet\n'
            return
        item = ['name', 'path', 'type']
        table = [item, ]
        for file_item in files.values():
            table_item = [file_item.name, file_item.path, file_item.type_name]
            table.append(table_item)

        self.cli_print(table)

    def do_add(self, line):
        """ Adds a new file.
        Usage add <file name> <path> <type>
        where
        <file name> is an internal kalinka file name
        <path> - the real file path
        <type> - the file type
        """
        args = line.split()
        if len(args) != 3:
            print 'Usage: add <file name> <path> <type>\n'
            return

        filemodule = self.getModule()
        files = filemodule.files

        # check file name
        name = args[0]
        if name in files.keys():
            print 'Kalinka file name \'%s\' is allready assigned' % name
            return

        # check path
        # the root folder should exist
        path = args[1]
        if not os.path.exists(os.path.dirname(path)):
            print 'Specified path folder does not exist: %s' % \
                os.path.dirname(path)
            return
        if path in self.getAssignedPaths():
            print 'Path has been already assigned: %s' % path
            return            

        # check type
        if args[2] not in self._file_types:
            print 'Incorrect file type: %s' % (args[2])
            return

        try:
            filemodule.addFile(name, path, args[2])
        except klk.common.KLKException, e:
            print 'Error while add file: %s. %s' % (name, str(e))
            return

        print 'File \'%s\' has been added' % name

    def complete_add(self, text, line, begidx, endidx):
        """ Add command completion        
        """

        args = line.split()

        #print '%d: %s\n' % (len(args), args)
        #print 'Text len: %d\n' % len(text)
        #print self.getPath4Add()

        count = len(args)
        if text:
            count = count - 1

        #print '%d: \n' % count

        # first arg is the name
        if count == 1:
            return [i for i in self.getFileName4Add() if i.startswith(text) ]  
        
        # second arg is path
        if count == 2:
            return [i for i in self.getPath4Add() if i.startswith(text) ]

        # third arg is type
        if count == 3:
            return [i for i in self._file_types if i.startswith(text) ]


    def do_delete(self, line):
        """ Deletes the specified file
        Usage delete <file name> 
        where
        <file name> is an internal kalinka file name
        """
        args = line.split()
        if len(args) != 1:
            print 'Usage: delete <file name>\n'
            return

        filemodule = self.getModule()
        files = filemodule.files

        # check file name
        name = args[0]
        if name not in files.keys():
            print 'Kalinka file name \'%s\' has not been assigned' % (name)
            return

        try:
            filemodule.delFile(name)
        except klk.common.KLKException, e:
            print 'Error while delete file: %s. %s' % (name, str(e))
            return

        print 'File \'%s\' has been deleted' % name

    def complete_delete(self, text, line, begidx, endidx):
        """ Delete command completion        
        """
        return [i for i in self.getModule().files.keys() if i.startswith(text)]

    def getModule(self):
        """ @return the file module instance
        """
        factory = klk.Factory()
        server = factory.servers[factory.config.server]
        filemodule = server.modules[defines.NAME]
        return filemodule

    def getFileName4Add(self):
        """ @return a list with possible files names for add operations
        """
        result = []
        file_names =  self.getModule().files.keys()

        for item in file_names:
            name = klk.common.suggestCLIName(item)
            if name not in file_names:                
                result.append(name)

        if len(result) == 0:
            result.append('file1')

        result.sort()

        return result

    def getAssignedPaths(self):
        """ @return a list with assigned paths
        """
        result = []

        for item in self.getModule().files.values():
            result.append(item.path)

        return result

    def getPath4Add(self):
        """ @return a list with possible files names for add operations
        """
        result = []
        file_paths =  self.getAssignedPaths()

        for item in file_paths:
            name = klk.common.suggestCLIName(item)
            if name not in file_paths and \
                    os.path.exists(os.path.dirname(name)):            
                result.append(name)

        if len(result) == 0:
            result.append('/tmp/file1')

        result.sort()

        return result
