from django.conf.urls.defaults import *

urlpatterns = patterns('kalinka.front.views',
    (r'^ajax/playlist/(?P<proto>.+)$', 'ajax_playlist'),
    (r"^ajax/player/$", 'ajax_player'),
    (r'^watch/(?P<player>.+)?$', 'watch'),
    (r'^playlist/(?P<proto>.+)$', 'playlist'),
    (r'^test/$', 'test'),
)
