"""@package transcode
   @file transcode.py
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

from defines import *

import klk.common as common
import klk.adapter
import klk.factory

import klk.net
import klk.file

import sourcetype

import MySQLdb

class Source(common.Object):
    """ Transcode source info holder
    """

    def __init__(self, uuid, name, source_type, media_type, server):
        """ Constructs the source object
        @param[in] uuid - the source uuid
        @param[in] name - the source name
        @param[in] source_type - the source type (file, net etc.)
        @param[in] media_type - the media type name
        @param[in] server - the choosen server
        """
        common.Object.__init__(self, uuid, name)
        self._source_type = source_type
        self._media_type = media_type
        self._server = server

    @property
    def source_type(self):
        """ @return the object source type
        """
        return self._source_type

    @property
    def media_type(self):
        """ @return the source media type
        """
        return self._media_type

    @property
    def source_object(self):
        """ @return the source object itself
        """
        if self._source_type == sourcetype.FILE:
            module = self._server.modules[klk.file.NAME]
        elif self._source_type == sourcetype.NET:
            module = self._server.modules[klk.net.NAME]
        else:
            raise common.KLKException,\
                "Unknown source type for transcode application: %s" % \
                self._source_type

        return module.getItemByUUID(self.uuid)


class Task(common.Object):
    """Transcode task info
    """
    
    def __init__(self, uuid, name, input_source, output_source, \
                     vsize, vquality, schedule_info, duration):
        """ Constructs the task object
        @param[in] name - the task name
        @param[in] uuid - the task uuid
        @param[in] input_source - the input source
        @param[in] output_source - the output source
        @param[in] vsize - the video size info
        @param[in] vquality - the video quality
        @param[in] schedule_info - the scheduler info
        @param[in] duration - the task duration
        """
        common.Object.__init__(self, uuid, name)
        self._input_source = input_source
        self._output_source = output_source
        self._video_size = vsize
        self._video_quality = vquality
        self._schedule_info = schedule_info
        self._duration = duration

    @property
    def input_source(self):
        """ @return the input source name
        """
        return self._input_source

    @property
    def output_source(self):
        """ @return the output source name
        """
        return self._output_source

    @property
    def video_size(self):
        """ @return video size
        """
        return self._video_size

    @property
    def video_quality(self):
        """ @return video quality
        """
        return self._video_quality

    @property
    def schedule_info(self):
        """ @return schedule info for the task
        """
        return self._schedule_info

    @property
    def duration(self):
        """ @return the task duration
        """
        return self._duration
    

class Transcode(common.Application):
    """ Transcode application module
    """

    def __init__(self, server):
        """ Constructor
        @param[in] server - the parent object (the mediaserver 
        that keeps the module)
        """        
        common.Application.__init__(self, UUID, NAME, server)            

    @property
    def sources(self):
        """@return the dict of available sources
        key is the source name
        """
        sources = dict()
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            cursor.callproc('klk_app_transcode_source_show_cli')
            rows = cursor.fetchall()
            cursor.close()
            for row in rows:
                # klk_app_transcode_source.source_name AS source_name,
                # klk_app_transcode_source_type.type_name AS type_name,
		# klk_media_types.name AS media_type_name,
                # klk_app_transcode_source.source_uuid AS source_uuid
                # klk_media_types.uuid AS media_type_uuid
                source_name = row[0]
                type_name = row[1]
                media_type_uuid = row[4]
                source_uuid = row[3]
                sources[source_name] = Source(source_uuid, source_name, \
                         type_name, media_type_uuid, self._server)
            cursor.close()
                
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])
        
        return sources
    
    @property
    def tasks(self):
        """@return the dict of available tasks
        key is the task name
        """
        tasks = dict()
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor()
            cursor.callproc('klk_app_transcode_get')
            rows = cursor.fetchall()
            cursor.close()
            for row in rows:
                # klk_app_transcode_task.task AS task_uuid,
                #  klk_app_transcode_task.task_name AS task_name,
                #  input_source.source_uuid AS input_uuid,
                #  input_source.source_name AS input_name,
                #  input_source.source_type AS input_source_type,
                #  input_source.media_type AS input_media_type,
                #  output_source.source_uuid AS output_uuid,
                #  output_source.source_name AS output_name,
                #  output_source.source_type AS output_source_type,
                #  output_source.media_type AS output_media_type,
                #  klk_app_transcode_video_size.size_name AS vsize_name,
                #  klk_app_transcode_video_size.size_uuid AS vsize_uuid,
                #  klk_app_transcode_video_quality.quality_name AS vquality_name,
               #  klk_app_transcode_video_quality.quality_uuid AS vquality_uuid,
                # klk_app_transcode_task.schedule_start AS schedule_start,
                # klk_app_transcode_task.schedule_duration AS schedule_duration                
                uuid = row[0]
                name = row[1]
                input_source = row[3]
                output_source = row[7]
                vsize = row[10]
                vquality = row[12]
                schedule_info = row[14]
                duration = row[15]
                tasks[name] = Task(uuid, name, \
                                            input_source, output_source, vsize, vquality, \
                                            schedule_info, duration)
            cursor.close()
                
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])
        
        return tasks
    
    def addSource(self, source_object_name, source_type, media_type):
        """ Adds a new source
        @param[in] source_object_name - the name of source object
        @param[in] source_type - the source type
        @param[in] media_type - the media type

        @return the assigned source name
        """
        assigned_name = None
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor() 
            media_type_name = klk.Factory().getMediaTypeName(media_type)
            args = (source_type, source_object_name, \
                        media_type_name, assigned_name, )
            cursor.callproc('klk_app_transcode_source_add_cli',  args )
            cursor.execute('SELECT @_klk_app_transcode_source_add_cli_3')
            rows = cursor.fetchone()
            assigned_name = rows[0]
            if assigned_name == None:
                raise common.KLKException,\
                    "MySQL error: cannot add a transcode source (%s, %s, %s)" \
                    % (source_type, source_object_name, media_type_name)
                    

            conn.commit()
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)
        return assigned_name

    
    def delSource(self, name):
        """ Deletes a source with the specified name
        @param[in] name - the source name
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor() 
            return_value = -1
            args = (name, return_value, )
            cursor.callproc('klk_app_transcode_source_del_cli',  args )
            cursor.execute('SELECT @_klk_app_transcode_source_del_cli_1')
            conn.commit()
            rows = cursor.fetchone()
            if rows[0] != 0:
                raise common.KLKException,\
                    "MySQL error: cannot del a transcode source (%s)" \
                    % (name)                    
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)
    
    def addTask(self, name, input_source, output_source, \
                     vsize, vquality, schedule_info, duration):
        """ Adds a new source
        @param[in] name - the task name
        @param[in] input_source - the input source
        @param[in] output_source - the output source
        @param[in] vsize - the video size info
        @param[in] vquality - the video quality
        @param[in] schedule_info - the scheduler info
        @param[in] duration - the task duration        
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor() 
            return_value = -1
            args = ( name, input_source, output_source, \
                     vsize, vquality, schedule_info, duration, return_value)
            cursor.callproc('klk_app_transcode_task_add_cli',  args )
            cursor.execute('SELECT @_klk_app_transcode_task_add_cli_7')
            rows = cursor.fetchone()
            if rows[0] != 0:
                raise common.KLKException,\
                    "MySQL error: cannot add a transcode task (%s, %s, %s)" \
                    % (name, input_source, output_source)
                    
            conn.commit()
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)
            
    def delTask(self, name):
        """ Deletes a task with specified name
        @param[in] name - the task name to be deleted
        """
        try:
            db = self.getDB()
            conn = db.connect()
            cursor = conn.cursor() 
            return_value = -1
            args = (name, return_value, )
            cursor.callproc('klk_app_transcode_task_del_cli',  args )
            cursor.execute('SELECT @_klk_app_transcode_task_del_cli_1')
            conn.commit()
            rows = cursor.fetchone()
            if rows[0] != 0:
                raise common.KLKException,\
                    "MySQL error: cannot del a transcode task (%s)" \
                    % (name)                    
            cursor.close()
        except MySQLdb.Error, e:
            raise common.KLKException,\
                "MySQL error %d: %s" % (e.args[0], e.args[1])

        # notify the module about the changes
        adapter = self._server.modules[klk.adapter.NAME]
        adapter.sendSyncMessage(UUID, DBUPDATE_MESSAGE)
