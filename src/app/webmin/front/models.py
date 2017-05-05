from django.db import models as mo
from django.contrib.auth.models import User
from django.utils.translation import ugettext_lazy as _

from kalinka.httpstreamer.models import *

#class OutStream(HttpRoute):
#    allowed_to = mo.ManyToManyField(User)
#    
#    class Meta:
#        verbose_name = _('OutStream')
#        verbose_name_plural = _('OutStreams')
        #permissions = (
        #    ("can_drive", "Can drive"),
        #    ("can_vote", "Can vote in elections"),
        #    ("can_drink", "Can drink alcohol"),
        #)

#class Bookmark(mo.Model):
#    name = mo.CharField(_('Name'), max_length=32)
#    desc = mo.TextField(_('Description'), blank=True)
#    source = mo.ForeignKey(Streamer)
#
#    def __unicode__(self):
#        return self.name
#
#    class Meta:
#        verbose_name = _('Bookmark')
#        verbose_name_plural = _('Bookmarks')
#        ordering = ['name']
