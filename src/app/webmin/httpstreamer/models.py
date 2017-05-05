import django.db.models as mo
from django.contrib.auth.models import User
from django.utils.translation import ugettext_lazy as _

from kalinka.core.models import *
from kalinka.network.models import *

class HttpMediaType(mo.Model):
    #uuid       = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid       = UUIDField(primary_key=True, db_column='uuid')
    media_type = mo.ForeignKey(MediaType, verbose_name=_('Media type'), help_text=_('media type'), max_length=40, db_column='media_type')
    
    def __unicode__(self):
        return unicode(self.media_type)
    
    class Meta:
        verbose_name = _('Http-streamer Media type')
        verbose_name_plural = _('Http-streamer Media types')
        db_table = 'klk_app_http_streamer_media_types'

class Streamer(mo.Model):
    application = mo.ForeignKey(Application, verbose_name=_('Application'), related_name='httpstreamers', db_column='application', primary_key=True)
    out_route   = mo.ForeignKey(Route, verbose_name=_('Out route'), db_column='out_route')
    
    def __unicode__(self):
        return "HTTP-streamer to %s" % self.out_route
    
    class Meta:
        verbose_name = _('Http-streamer')
        verbose_name_plural = _('Http-streamers')
        db_table = 'klk_app_http_streamer'

class HttpRoute(mo.Model):
    #uuid        = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='uuid')
    application = mo.ForeignKey(Streamer, verbose_name=_('Streamer'), db_column='application')
    in_route    = mo.ForeignKey(Route, verbose_name=_('Input route'), db_column='in_route')
    out_path    = mo.CharField(_('Output path'), help_text=_('out path'), max_length=40)
    media_type  = mo.ForeignKey(HttpMediaType, verbose_name=_('Http-streamer Media type'), db_column='media_type')
    
    allowed_to  = mo.ManyToManyField(User, verbose_name=_('Allowed to'))
    
    def __unicode__(self):
        return '%s%s' % (self.application.out_route, self.out_path)
    
    class Meta:
        verbose_name = _('Http-streamer Route')
        verbose_name_plural = _('Http-streamer Routes')
        db_table = 'klk_app_http_streamer_route'
