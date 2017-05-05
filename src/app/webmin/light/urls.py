from django.conf.urls.defaults import *

urlpatterns = patterns('',
    (r'^watch/$', 'kalinka.light.views.watch'),
)
