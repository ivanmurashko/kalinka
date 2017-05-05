from django.conf.urls.defaults import *

urlpatterns = patterns('django.contrib.auth.views',

    (r'^login/$',  'login',  {'template_name': 'auth/login.html'}),
    (r'^logout/$', 'logout', {'template_name': 'auth/logout.html'}),
    (r'^password_change/$', 'password_change', {'template_name': 'auth/pass_change.html'})
    #(r'^password_change_done/$', 'password_change_done', {'template_name': 'auth/pass_change_done.html'})
)


