"""@package testtranscode
   @file testtranscode.py
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
import klk.file
import klk.db
import klk.server
import klk.transcode
import klk.mediatypes

import unittest
import os
import sys


import klk.transcode.sourcetype
import klk.transcode.videoquality
import klk.transcode.videosize
import klk.transcode.schedule

import transcodetestdefines

class TranscodeTest(unittest.TestCase):
    """ HTTP streamer application unit test
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
        db.executeScript('testtranscode.sql')

    def testSource(self):
        """ Sources creation test
        """
        server = self._factory.servers['testserver']
        self.assert_(server)        

        transcode = server.modules[klk.transcode.NAME]
        sources = transcode.sources
        self.assert_(len(sources) == 3)
        self.checkMainSources(sources)

        # add a new file
        filemodule = server.modules[klk.file.NAME]
        filemodule.addFile('pyfile1', '/tmp/file1.bin', \
                               klk.file.filetype.REGULAR)
        assignedname = \
            transcode.addSource('pyfile1', klk.transcode.sourcetype.FILE, \
                                    klk.mediatypes.AUTO)
        sources = transcode.sources
        self.assert_(len(sources) == 4)
        self.checkMainSources(sources)
        self.assert_(assignedname in sources.keys())        
        add_source = sources[assignedname]
        self.assert_(add_source.name == assignedname)
        self.assert_(add_source.source_type == klk.transcode.sourcetype.FILE)
        self.assert_(add_source.media_type == klk.mediatypes.AUTO)
        self.assert_(add_source.source_object.name == \
                         filemodule.files['pyfile1'].name)
        self.assert_(add_source.source_object.uuid == \
                         filemodule.files['pyfile1'].uuid)
        self.assert_(add_source.source_object.path == \
                         filemodule.files['pyfile1'].path)

        #delete the file
        transcode.delSource(assignedname)
        sources = transcode.sources
        self.assert_(len(sources) == 3)
        self.checkMainSources(sources)

    def testSourceExceptions(self):
        """ Sources excepton tests
        """
        server = self._factory.servers['testserver']
        self.assert_(server)        
        transcode = server.modules[klk.transcode.NAME]
        self.assertRaises(klk.common.KLKException, transcode.delSource, 'aaaa')


    def checkMainSources(self, sources):
        """ Checks main filling of sources
        """
        self.assert_(transcodetestdefines.INPUT_SOURCE_NAME in sources.keys())
        input_source = sources[transcodetestdefines.INPUT_SOURCE_NAME]
        self.assert_(input_source.name == \
                         transcodetestdefines.INPUT_SOURCE_NAME)
        self.assert_(input_source.source_type == \
                         klk.transcode.sourcetype.FILE)
        self.assert_(input_source.source_object.name == \
                         transcodetestdefines.INPUT_FILE_NAME)
        self.assert_(input_source.source_object.uuid == \
                         transcodetestdefines.INPUT_FILE_UUID)
        self.assert_(input_source.source_object.path == \
                         transcodetestdefines.INPUT_FILE_PATH)

        self.assert_(transcodetestdefines.OUTPUT_SOURCE_NAME in sources.keys())
        output_source = sources[transcodetestdefines.OUTPUT_SOURCE_NAME]
        self.assert_(output_source.source_type == \
                         klk.transcode.sourcetype.FILE)
        self.assert_(output_source.source_object.uuid == \
                         transcodetestdefines.OUTPUT_FILE_UUID)
        self.assert_(output_source.source_object.path == \
                         transcodetestdefines.OUTPUT_FILE_PATH)
        
        self.assert_(transcodetestdefines.NETROUTE_SOURCE_NAME in \
                         sources.keys())        
        netroute_source = sources[transcodetestdefines.NETROUTE_SOURCE_NAME]
        self.assert_(netroute_source.source_type == \
                         klk.transcode.sourcetype.NET)
        self.assert_(netroute_source.source_object.uuid == \
                         transcodetestdefines.NETROUTE_UUID)
        self.assert_(netroute_source.source_object.addr == \
                         transcodetestdefines.NETROUTE_HOST)
        self.assert_(netroute_source.source_object.port == \
                         transcodetestdefines.NETROUTE_PORT)


    def testTask(self):
        """ Tasks creation test
        """
        server = self._factory.servers['testserver']
        self.assert_(server)        

        transcode = server.modules[klk.transcode.NAME]
        tasks = transcode.tasks
        self.assert_(len(tasks) == 2)
        self.checkMainTasks(tasks)

        # add a new task
        transcode.addTask('task3', transcodetestdefines.OUTPUT_SOURCE_NAME, \
                              transcodetestdefines.INPUT_SOURCE_NAME, \
                              klk.transcode.videosize.SIZE320x240, \
                              klk.transcode.videoquality.LOW, \
                              klk.transcode.schedule.ALWAYS, 10)
        
        tasks = transcode.tasks
        self.assert_(len(tasks) == 3)
        self.checkMainTasks(tasks)
        self.assert_('task3' in tasks.keys())
        task3 = tasks['task3']
        self.assert_(task3.name == 'task3')
        self.assert_(task3.input_source == transcodetestdefines.OUTPUT_SOURCE_NAME)
        self.assert_(task3.output_source == \
                         transcodetestdefines.INPUT_SOURCE_NAME)
        self.assert_(task3.video_size == klk.transcode.videosize.SIZE320x240)
        self.assert_(task3.video_quality == klk.transcode.videoquality.LOW)
        self.assert_(task3.schedule_info == klk.transcode.schedule.ALWAYS)
        self.assert_(task3.duration == 10)

        #delete the task
        transcode.delTask('task3')
        tasks = transcode.tasks
        self.assert_(len(tasks) == 2)
        self.checkMainTasks(tasks)        

    def checkMainTasks(self, tasks):
        """ Checks main filling of tasks
        """
        self.assert_(transcodetestdefines.TASK1_NAME in tasks.keys())
        task1 = tasks[transcodetestdefines.TASK1_NAME]
        self.assert_(task1.name == transcodetestdefines.TASK1_NAME)
        self.assert_(task1.input_source == \
                         transcodetestdefines.INPUT_SOURCE_NAME)
        self.assert_(task1.output_source == \
                         transcodetestdefines.OUTPUT_SOURCE_NAME)
        self.assert_(task1.video_size == klk.transcode.videosize.DEFAULT)
        self.assert_(task1.video_quality == klk.transcode.videoquality.DEFAULT)
        self.assert_(task1.schedule_info == klk.transcode.schedule.REBOOT)
        self.assert_(task1.duration == 0)

        self.assert_(transcodetestdefines.TASK2_NAME in tasks.keys())
        task2 = tasks[transcodetestdefines.TASK2_NAME]
        self.assert_(task2.name == transcodetestdefines.TASK2_NAME)
        self.assert_(task2.input_source == \
                         transcodetestdefines.INPUT_SOURCE_NAME)
        self.assert_(task2.output_source == \
                         transcodetestdefines.NETROUTE_SOURCE_NAME)
        self.assert_(task2.video_size == klk.transcode.videosize.DEFAULT)
        self.assert_(task2.video_quality == klk.transcode.videoquality.DEFAULT)
        self.assert_(task2.schedule_info == klk.transcode.schedule.REBOOT)
        self.assert_(task2.duration == 0)

if __name__ == '__main__':
    unittest.main()
