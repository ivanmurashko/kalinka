from django.db import models as mo
from django.utils.translation import ugettext_lazy as _

import datetime as dt
import uuid, os


class UUIDField(mo.CharField) :
    
    def __init__(self, *args, **kwargs):
        kwargs['max_length'] = kwargs.get('max_length', 64)
        #kwargs['blank']      = kwargs.get('blank', False)
        kwargs['editable']   = kwargs.get('editable', False)
        kwargs['help_text']  = kwargs.get('help_text', _('unique identifier'))
        mo.CharField.__init__(self, *args, **kwargs)
    
    def pre_save(self, model_instance, add):
        if add :
            value = str(uuid.uuid4())
            setattr(model_instance, self.attname, value)
            return value
        else:
            return super(mo.CharField, self).pre_save(model_instance, add)

#class KlkModel(mo.Model):
#    uuid = UUIDField(primary_key=True)
#    
#    class Meta:
#        abstract = True
    

class Host(mo.Model):
    #host          = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid          = UUIDField(primary_key=True, db_column='host')
    name          = mo.CharField(_('Host name'), help_text=_('server host name'), max_length=50, unique=True, db_column='host_name')
    cpu_index_typ = mo.IntegerField(_('Typical CPU-index'), help_text=_('typical cpu requirement index'))
    cpu_index_max = mo.IntegerField(_('Max CPU-index'), help_text=_('peak cpu requirement index'))
    
    def __unicode__(self):
        return '%s' % self.name
    
    class Meta:
        verbose_name = _('Host')
        verbose_name_plural = _('Hosts')
        db_table = 'klk_hosts'

class MediaType(mo.Model):
    #uuid = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=120, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='uuid')
    name        = mo.CharField(_('Name'), unique=True, max_length=120)
    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return self.name
    
    class Meta:
        verbose_name = _('Media type')
        verbose_name_plural = _('Media types')
        db_table = u'klk_media_types'

class Application(mo.Model):
    #application = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='application')
    name        = mo.CharField(_('Appliction name'), help_text=_('application name'), max_length=50, unique=True)
    #BooleanField() ??
    enabled     = mo.SmallIntegerField(_('Enabled'), help_text=_('is the application activated by the user'), )
    status      = mo.SmallIntegerField(_('Status'), help_text=_('is the kalinka cluster currently executing this application, 0 = no, 1 = yes'), )
    
    priority    = mo.IntegerField(_('Priority'), help_text=_('application priority, 0 = most'))
    host        = mo.ForeignKey(Host, verbose_name=_('Host'), help_text=_('prefered host for this application'), db_column='host')
    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return '%s @ %s' % (self.name, self.host)
    
    class Meta:
        verbose_name = _('Application')
        verbose_name_plural = _('Applications')
        db_table = 'klk_applications'

class ModuleType(mo.Model):
    #module_type   = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid          = UUIDField(primary_key=True, db_column='module_type')
    cpu_index_typ = mo.IntegerField(_('Typical CPU-index'), help_text=_('typical cpu requirement index'))
    cpu_index_max = mo.IntegerField(_('Max CPU-index'), help_text=_('peak cpu requirement index'))
    description   = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return '%s' % self.description
    
    class Meta:
        verbose_name = _('Module type')
        verbose_name_plural = _('Module types')
        db_table = 'klk_module_types'

class ModuleAllocation(mo.Model):
    #module      = mo.CharField(_('UUID'), help_text=_('module'), max_length=40, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='module')
    application = mo.ForeignKey(Application, verbose_name=_('Application'), help_text=_('references the application which requires this module to load'), db_column='application')
    module_type = mo.ForeignKey(ModuleType, verbose_name=_('Module type'), help_text=_('module type'), db_column='module_type')
    host        = mo.ForeignKey(Host, verbose_name=_('Host'), help_text=_('host'), db_column='host')
    
    def __unicode__(self):
        return '%s for %s @ %s' % (self.module_type, self.application, self.host)
    
    class Meta:
        verbose_name = _('Module allocation')
        verbose_name_plural = _('Module allocations')
        db_table = 'klk_modules_allocated'


#class Log(mo.Model):
#    id        = mo.IntegerField(help_text=_('unique identifier'), primary_key=True)
#    module    = mo.ForeignKey(ModuleAllocation, db_column='module_uuid')
#    timestamp = mo.DateTimeField(help_text=_('timestamp of event'), default=dt.datetime.now)
#    
#    def __unicode__(self):
#        return "Log entry at %s" % self.timestamp
#    
#    class Meta:
#        verbose_name = _('Log')
#        verbose_name_plural = _('Logs')
#        db_table = 'klk_log'


class BusType(mo.Model):
    #bus_type    = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='bus_type')
    bandwidth   = mo.IntegerField(_('Bandwidth'), help_text=_('bandwidth in khz'))
    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return '%s' % self.description
    
    class Meta:
        verbose_name = _('Bus type')
        verbose_name_plural = _('Bus types')
        db_table = 'klk_bus_types'

class Bus(mo.Model):
    #bus         = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='bus')
    host        = mo.ForeignKey(Host, verbose_name=_('Host'), help_text=_('host'), db_column='host')
    bus_type    = mo.ForeignKey(BusType, verbose_name=_('Bus type'), help_text=_('bus type'), db_column='bus_type')
    bandwidth   = mo.IntegerField(_('Bandwidth'), help_text=_('reserved take bandwidth in khz'))
    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return _('%s (%d kHz) @ %s') % (self.bus_type, self.bandwidth, self.host)
    
    class Meta:
        verbose_name = _('Bus')
        verbose_name_plural = _('Buses')
        db_table = 'klk_busses'

class ResourceType(mo.Model):
    #resource_type = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid          = UUIDField(primary_key=True, db_column='resource_type')
    bandwidth     = mo.IntegerField(_('Bandwidth'), help_text=_('max allowed bandwidth in MHz'))
    description   = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return _('%s (%d MHz)') % (self.description, self.bandwidth)
    
    class Meta:
        verbose_name = _('Resource type')
        verbose_name_plural = _('Resource types')
        db_table = 'klk_resource_types'

class Resource(mo.Model):
    #resource      = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid          = UUIDField(primary_key=True, db_column='resource')
    bus           = mo.ForeignKey(Bus, verbose_name=_('Bus'), help_text=_('bus'), db_column='bus')
    resource_type = mo.ForeignKey(ResourceType, verbose_name=_('Resource type'), help_text=_('resource type'), db_column='resource_type')
    bandwidth     = mo.IntegerField(_('Bandwidth'), help_text=_('taken bandwdith in khz'))
    enabled       = mo.SmallIntegerField(_('Enabled'), help_text=_('resource available for use'), default=0)
    
    def __unicode__(self):
        return _('%s on %s @ %s') % (self.resource_type.description, self.bus.description, self.bus.host)
    
    class Meta:
        verbose_name = _('Resource')
        verbose_name_plural = _('Resources')
        db_table = 'klk_resources'

class Adapter(mo.Model):
    #adapter = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=120, primary_key=True)
    uuid    = UUIDField(primary_key=True, db_column='adapter')
    host    = mo.ForeignKey(Host, verbose_name=_('Host'), help_text=_('host'), db_column='host_uuid')
    addr    = mo.CharField(_('Address'), max_length=120)
    port    = mo.IntegerField(_('Port'))
    inuse   = mo.IntegerField(_('In use'))
    
    def __unicode__(self):
        return '%s:%d @ %s' % (self.addr, self.port, self.host)
    
    class Meta:
        verbose_name = _('Adapter')
        verbose_name_plural = _('Adapters')
        db_table = 'klk_adapter'

class FileType(mo.Model):
    uuid    = UUIDField(primary_key=True, db_column='type_uuid')
    name    = mo.CharField(_('Name'), max_length=128, db_column='type_name')
    
    def __unicode__(self):
        return self.name
    
    class Meta:
        verbose_name = _('File type')
        verbose_name_plural = _('File types')
        db_table = 'klk_file_type'

class File(mo.Model):
    #file      = mo.CharField(_('UUID'), max_length=120, primary_key=True)
    uuid      = UUIDField(primary_key=True, db_column='file')
    #type      = mo.ForeignKey(FileType, db_column='type_uuid')
    name      = mo.CharField(_('File name'), unique=True, max_length=128)
    path      = mo.CharField(_('File path'), unique=True, max_length=255, db_column='file_path')
    host      = mo.ForeignKey(Host, db_column='host')
    #parent    = mo.ForeignKey('self', )
    
    def __unicode__(self):
        return '%s @ %s' % (os.path.join(self.path, self.name), self.host)
    
    class Meta:
        verbose_name = _('File')
        verbose_name_plural = _('Files')
        db_table = 'klk_files'

