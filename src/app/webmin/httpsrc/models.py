from django.db import models as mo
from django.utils.translation import ugettext_lazy as _

from kalinka.core.models import *

class HttpSource(mo.Model):
    #httpsrc     = mo.CharField(_('UUID'), help_text=_('unique identifier'), max_length=40, primary_key=True)
    uuid        = UUIDField(primary_key=True, db_column='httpsrc')
    title       = mo.CharField(_('Title'), max_length=64)
    address     = mo.CharField(_('Source address'), max_length=255)
    #path        = mo.CharField(_('Path to stream'), max_length=128)
    login       = mo.CharField(_('Login'), help_text=_('login for http-auth'), max_length=32)
    passwd      = mo.CharField(_('Password'), help_text=_('password for http-auth(will be stored as plain text)'), max_length=32)
    description = mo.CharField(_('Description'), help_text=_('remark'), max_length=255)
    
    def __unicode__(self):
        return '%s (%s/%s)' % (self.title, self.address, self.path)
    
    class Meta:
        verbose_name = _('HttpSource')
        verbose_name_plural = _('HttpSources')
        ordering = ['title']
        db_table = 'klk_httpsrc'
