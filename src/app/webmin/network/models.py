from django.db import models as mo
from django.utils.translation import ugettext_lazy as _

from kalinka.core.models import *

class NetworkType(mo.Model):
    #network_type = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True, db_column='media_type')
    uuid         = UUIDField(primary_key=True, db_column='media_type')
    bandwidth    = mo.IntegerField(help_text=_('max allowed bandwidth'))
    description  = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return self.description
    
    class Meta:
        verbose_name = _('Network type')
        verbose_name_plural = _('Network types')
        db_table = 'klk_network_media_types'

class Trunk(mo.Model):
    #trunk        = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid         = UUIDField(primary_key=True, db_column='trunk')
    network_type = mo.ForeignKey(NetworkType, help_text=_('network media type'), db_column='media_type')
    description  = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return self.description
    
    class Meta:
        verbose_name = _('Trunk')
        verbose_name_plural = _('Trunks')
        db_table = 'klk_network_trunks'

class InterfaceResource(mo.Model):
    #resource  = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid      = UUIDField(primary_key=True, db_column='resource')
    trunk     = mo.ForeignKey(Trunk, verbose_name=_('Trunk'), help_text=_('trunk'), db_column='trunk')
    node_name = mo.CharField(_('Node name'), help_text=_('device name, e.g. eth0'), max_length=10)
    
    def __unicode__(self):
        return '%s/%s' % (self.trunk, self.node_name)
    
    class Meta:
        verbose_name = _('Interface resource')
        verbose_name_plural = _('Interfaces resources')
        db_table = 'klk_network_interface_resources'

class InterfaceAddress(mo.Model):
    #address    = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid       = UUIDField(primary_key=True, db_column='address')
    resource   = mo.ForeignKey(InterfaceResource, help_text=_('resource'), db_column='resource')
    ip_address = mo.CharField(help_text=_('IPv4 or IPv6 network address'), max_length=16)
    ip_mask    = mo.CharField(help_text=_('network mask'), max_length=16)
    #ip_network = mo.CharField(max_length=48, blank=True)
    
    def __unicode__(self):
        return '%s/%s' % (self.ip_address, self.ip_mask)
    
    class Meta:
        verbose_name = _('Interface address')
        verbose_name_plural = _('Interface addresses')
        db_table = 'klk_network_interface_addresses'

class Protocol(mo.Model):
    #protocol    = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=120, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='protocol')
    name        = mo.CharField(_('Protocol name'), unique=True, max_length=120, db_column='proto_name')
    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=120)
    
    def __unicode__(self):
        return self.name
    
    class Meta:
        verbose_name = _('Protocol')
        verbose_name_plural = _('Protocols')
        db_table = 'klk_network_protocols'

class Route(mo.Model):
    #route    = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid     = UUIDField(primary_key=True, db_column='route')
    name     = mo.CharField(_('Name'), unique=True, max_length=120)
    address  = mo.ForeignKey(InterfaceAddress, verbose_name=_('Address'), help_text=_('interface address'), db_column='address')
    host     = mo.CharField(_('Host'), help_text=_('multicast group, hostname or network address'),
                            max_length=50, default='localhost')
    port     = mo.IntegerField(_('Port'), help_text=_('network port to send to 0 - 65535'), default=4951)
    protocol = mo.ForeignKey(Protocol, verbose_name=_('Protocol'), db_column='protocol')
    lock_timestamp = mo.DateTimeField()
    lock_resource = mo.CharField(max_length=120, blank=True)
    
    def __unicode__(self):
        return '%s:%s' % (self.host, self.port)
    
    class Meta:
        verbose_name = _('Route')
        verbose_name_plural = _('Routes')
        db_table = 'klk_network_routes'
