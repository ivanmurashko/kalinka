from django.db import models as mo
from django.utils.translation import ugettext_lazy as _

from kalinka.core.models import *
from kalinka.dvb.models import *
from kalinka.network.models import *

class Station(mo.Model):
    #station        = mo.CharField(help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid           = UUIDField(primary_key=True, db_column='station')
    channel        = mo.ForeignKey(Channel, verbose_name=_('Channel'), help_text=_('cannel name'), db_column='channel')
    route          = mo.ForeignKey(Route, verbose_name=_('Route'), help_text=_('route'), db_column='route')
    enable         = mo.SmallIntegerField(_('Enabled'), help_text=_('enabled'))
    lock_used_host = mo.CharField(_('Lock host'), max_length=40, blank=True, null=True)
    lock_timestamp = mo.DateTimeField(_('Lock timestamp'), blank=True, null=True)
    priority       = mo.IntegerField(_('Priority'))
    
    def __unicode__(self):
        return _('%s\tto\t%s') % (self.channel, self.route)
    
    class Meta:
        verbose_name = _('Station')
        verbose_name_plural = _('Stations')
        db_table = 'klk_app_dvb_streamer'

class FailedStation(mo.Model):
    station = mo.ForeignKey(Station, verbose_name=_('Station'), help_text=_('station'), primary_key=True, db_column='station_uuid')
    host    = mo.ForeignKey(Host, verbose_name=_('Host'), help_text=_('host'), db_column='host_uuid')
    
    def __unicode__(self):
        return '%s @ %s' % (self.station, self.host)
    
    class Meta:
        verbose_name = _('Failed station')
        verbose_name_plural = _('Failed stations')
        db_table = 'klk_app_dvb_streamer_failed_stations'