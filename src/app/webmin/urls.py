from django.conf.urls.defaults import *
from settings import *

from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    (r'^media/(?P<path>.*)$', 'django.views.static.serve', {'document_root': MEDIA_ROOT}),
    (r'^$', 'django.views.generic.simple.redirect_to', {'url': '/iface/front/watch/'}),
    
    (r'^auth/',  include('kalinka.auth.urls')),
    (r'^front/', include('kalinka.front.urls')),
    (r'^light/', include('kalinka.light.urls')),
    
    (r'^admin/(.*)', admin.site.root),
)
