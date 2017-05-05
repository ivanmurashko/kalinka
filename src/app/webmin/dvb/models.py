from django.db import models as mo
from django.utils.translation import ugettext_lazy as _

from kalinka.core.models import *

class Type(mo.Model):
    #dvb_type    = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='dvb_type')
    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return self.description
    
    class Meta:
        verbose_name = _('DVB type')
        verbose_name_plural = _('DVB types')
        db_table = 'klk_dvb_types'

class SignalSourceType(mo.Model):
    #signal_source_type = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=120, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='signal_source_type')
    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return self.description
    
    class Meta:
        verbose_name = _('Signal source type')
        verbose_name_plural = _('Signal source types')
        db_table = 'klk_dvb_signal_source_types'

class SignalSource(mo.Model):
    #signal_source      = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=120, primary_key=True)
    uuid               = UUIDField(primary_key=True, db_column='signal_source')
    signal_source_type = mo.ForeignKey(SignalSourceType, verbose_name=_('Signal source type'), db_column='signal_source_type')
    description        = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return self.description
    
    class Meta:
        verbose_name = _('Signal source')
        verbose_name_plural = _('Signal sources')
        db_table = 'klk_dvb_signal_source'

class DVBResource(Resource):
    #resource      = mo.CharField(help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid          = UUIDField(primary_key=True, db_column='resource')
    dvb_type      = mo.ForeignKey(Type, help_text=_('dvb type'), db_column='dvb_type')
    signal_source = mo.ForeignKey(SignalSource, help_text=_('resource type'), db_column='signal_source')
    #adapter_no    = mo.SmallIntegerField(help_text=_('adapter number 0..16'), default=0)
    #frontend_no   = mo.SmallIntegerField(help_text=_('frontend number 0..16'), default=0)
    
    class Meta:
        verbose_name = _('Resource')
        verbose_name_plural = _('Resources')
        db_table = 'klk_dvb_resources'

class Channel(mo.Model):
    #channel       = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid          = UUIDField(primary_key=True, db_column='channel')
    name          = mo.CharField(_('Name'), help_text=_('channel name'), max_length=50)
    channel_no    = mo.IntegerField(_('Number'), help_text=_('channel number'))
    #frequency     = mo.IntegerField(_('Frequency'), help_text=_('tuned frequency'))
    signal_source = mo.ForeignKey(SignalSource, verbose_name=_('Signal source'), db_column='signal_source')
    bandwidth     = mo.IntegerField(_('Bandwidth'), help_text=_('bandwidth taken'))
    
    def __unicode__(self):
        return '#%d\t%s' % (self.channel_no, self.name)
    
    class Meta:
        #abstract = True
        verbose_name = _('Channel')
        verbose_name_plural = _('Channels')
        ordering = ['channel_no', 'name']
        db_table = 'klk_dvb_channels'
    

class SatelliteChannel(Channel):
    channel      = mo.OneToOneField(Channel, parent_link=True, db_column='channel', primary_key=True)
    frequency    = mo.IntegerField(_('Frequency'), help_text=_('tuned frequency'))
    diseq_source = mo.SmallIntegerField(_('DISEqC'), help_text=_('DISEqC selected source null=disabled 0..7'), null=True)
    polarity     = mo.SmallIntegerField(_('Polarity'), help_text=_('polarity 0=horizontal; 1=vertical'))
    symbol_rate  = mo.IntegerField(_('Symbol rate'), help_text=_('symbol rate 0..2147483647'))
    code_rate_hp = mo.SmallIntegerField(_('Code rate HP'), help_text=_('High priority code rate 0=none, 1=1/2, 2=2/3, 3=3/4, 4=4/5, 5=5/6, 6=6/7, 8=8/9, 9=Auto'))
    
    class Meta:
        verbose_name = _('Satellite channel')
        verbose_name_plural = _('Satellite channels')
        db_table = 'klk_dvb_s_channels'

class DigitalTVChannel(Channel):
    channel       = mo.OneToOneField(Channel, parent_link=True, db_column='channel', primary_key=True)
    frequency     = mo.IntegerField(_('Frequency'), help_text=_('tuned frequency'))
    dvb_bandwidth = mo.SmallIntegerField(_('DVB bandwidth'), help_text=_('Bandwidth 0=8, 1=7, 2=6, 3=Auto'))
    code_rate_hp  = mo.SmallIntegerField(_('Code rate HP'), help_text=_('High priority code rate 0=none, 1=1/2, 2=2/3, 3=3/4, 4=4/5, 5=5/6, 6=6/7, 8=8/9, 9=Auto'))
    code_rate_lp  = mo.SmallIntegerField(_('Code rate LP'), help_text=_('Low priority code rate 0=none, 1=1/2, 2=2/3, 3=3/4, 4=4/5, 5=5/6, 6=6/7, 8=8/9, 9=Auto'))
    modulation    = mo.SmallIntegerField(_('Modulation'), help_text=_('Modulation 0=QPSK, 1=QAM16, 2=QAM32, 3=QAM64, 4=QAM128, 5=QAM256, 6=AUTO'))
    transmode     = mo.SmallIntegerField(_('Transmode'), help_text=_('Transmission mode 0=2k, 1=8k, 2=Auto'))
    hierarchy     = mo.SmallIntegerField(_('Hierarchy'))
    guard         = mo.SmallIntegerField(_('Guard'))

    class Meta:
        verbose_name = _('Digital TV channel')
        verbose_name_plural = _('Digital TV channels')
        db_table = 'klk_dvb_t_channels'

class CChannel(Channel):
    channel      = mo.OneToOneField(Channel, parent_link=True, db_column='channel', primary_key=True)
    frequency    = mo.IntegerField(_('Frequency'), help_text=_('tuned frequency'))
    symbol_rate  = mo.IntegerField(_('Symbol rate'), help_text=_('symbol rate 0..2147483647'))

    class Meta:
        verbose_name = _('C channel')
        verbose_name_plural = _('C channels')
        db_table = 'klk_dvb_c_channels'

#class PidType(mo.Model):
#    #pid_type    = mo.CharField(help_text=_('unique identifier'), max_length=40, primary_key=True)
#    uuid        = UUIDField(primary_key=True, db_column='pid_type')
#    bandwidth   = mo.IntegerField(help_text=_('bandwidth taken'))
#    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
#    
#    class Meta:
#        abstract = True
#        verbose_name = _('Pid type')
#        verbose_name_plural = _('Pid types')
#        db_table = 'klk_dvb_pid_type'
#
#class Pid(mo.Model):
#    #pid         = mo.CharField(help_text=_('unique identifier'), max_length=40, primary_key=True)
#    uuid        = UUIDField(primary_key=True, db_column='pid')
#    pid_type    = mo.ForeignKey(PidType, verbose_name=_('Pid type'), help_text=_('pid type'), db_column='pid_type')
#    pid_no      = mo.IntegerField(_('Pid number'), help_text=_('pid number'))
#    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
#
#    class Meta:
#        abstract = True
#
#class SatellitePid(Pid):
#    channel = mo.ForeignKey(SatelliteChannel, help_text=_('satellite channel'), db_column='channel')
#    
#    class Meta(Pid.Meta):
#        abstract = True
#        verbose_name = _('Satellite PID')
#        verbose_name_plural = _('Satellite PIDs')
#        db_table = 'klk_dvb_s_pids'
#
#class DigitalTVPid(Pid):
#    channel = mo.ForeignKey(DigitalTVChannel, help_text=_('digital tv channel'), db_column='channel')
#    
#    class Meta(Pid.Meta):
#        abstract = True
#        verbose_name = _('Digital TV PID')
#        verbose_name_plural = _('Digital TV PIDs')
#        db_table = 'klk_dvb_t_pids'
#
#class CPid(Pid):
#    channel = mo.ForeignKey(CChannel, help_text=_('c channel'), db_column='channel')
#    
#    class Meta(Pid.Meta):
#        abstract = True
#        verbose_name = _('C PID')
#        verbose_name_plural = _('C PIDs')
#        db_table = 'klk_dvb_c_pids'
#
#class TunedChannel(mo.Model):
#    #tuned_channel = mo.CharField(help_text=_('unique identifier'), max_length=40, primary_key=True)
#    uuid          = UUIDField(primary_key=True, db_column='tuned_channel')
#    channel       = mo.ForeignKey(Channel, db_column='channel')
#    resource      = mo.ForeignKey(Resource, db_column='resource')
#    
#    class Meta:
#        abstract = True
#        verbose_name = _('Tuned channel')
#        verbose_name_plural = _('Tuned channels')
#        db_table = 'klk_dvb_tuned_channels'
#
#class UsedChannel(mo.Model):
#    #used_pid      = mo.CharField(help_text=_('unique identifier'), max_length=40, primary_key=True)
#    uuid          = UUIDField(primary_key=True, db_column='used_pid')
#    tuned_channel = mo.ForeignKey(TunedChannel, db_column='tuned_channel')
#    pid           = mo.ForeignKey(Pid, db_column='pid')
#    
#    class Meta:
#        abstract = True
#        verbose_name = _('Used channel')
#        verbose_name_plural = _('Used channels')
#        db_table = 'klk_dvb_used'
