from django.db import models as mo
from django.utils.translation import ugettext_lazy as _

from kalinka.core.models import *

class TranscodeSourceType(mo.Model):
    #uuid = mo.CharField(max_length=120, primary_key=True, db_column='type_uuid')
    uuid = UUIDField(primary_key=True, db_column='type_uuid')
    name = mo.CharField(unique=True, max_length=120, db_column='type_name')
    
    def __unicode__(self):
        return self.name
    
    class Meta:
        verbose_name = _('Source type')
        verbose_name_plural = _('Source types')
        db_table = u'klk_app_transcode_source_type'

class TranscodeSource(mo.Model):
    #uuid   = mo.CharField(_('UUID'), max_length=120, primary_key=True, db_column='source_uuid')
    uuid   = UUIDField(primary_key=True, db_column='source_uuid')
    name   = mo.CharField(unique=True, max_length=120, db_column='source_name')
    type   = mo.ForeignKey(TranscodeSourceType, verbose_name=_('Source type'), db_column='source_type')
    #media_type    = mo.ForeignKey(MediaType, verbose_name=_('Media type'), db_column='media_type')
    #external_uuid = mo.CharField(max_length=120)
    
    def __unicode__(self):
        return "%s" % self.name
    
    class Meta:
        verbose_name = _('Transcode source')
        verbose_name_plural = _('Transcode sources')
        db_table = u'klk_app_transcode_source'

class TranscodeTask(mo.Model):
    #uuid = mo.CharField(_('UUID'), max_length=120, primary_key=True, db_column='task')
    uuid            = UUIDField(primary_key=True, db_column='task')
    name            = mo.CharField(unique=True, max_length=384, db_column='task_name')
    input_source    = mo.ForeignKey(TranscodeSource, related_name='tasks_as_input', db_column='input_source')
    output_source   = mo.ForeignKey(TranscodeSource, related_name='task_as_output', db_column='output_source')
    media_type      = mo.ForeignKey(MediaType, db_column='media_type')
    schedule_start  = mo.CharField(max_length=384)
    schedule_duration = mo.IntegerField()
    
    def __unicode__(self):
        return "%s (%s) | %s -> %s" % (self.name, self.media_type, self.input_source, self.output_source)
    
    class Meta:
        verbose_name = _('Task')
        verbose_name_plural = _('Tasks')
        db_table = u'klk_app_transcode_task'

#class Transcode(mo.Model):
#    uuid = mo.CharField(_('UUID'), max_length=120, primary_key=True)
#    application = mo.ForeignKey(Application, verbose_name=_('Application'), db_column='application')
#    task = mo.ForeignKey(TranscodeTask, verbose_name=_('Task'), db_column='task')
#    
#    def __unicode__(self):
#        return "%s with %s" % (self.application, self.task)
#    
#    class Meta:
#        db_table = u'klk_app_transcode'
