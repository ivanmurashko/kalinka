from django.contrib import admin
from models import *

class StreamerAdmin(admin.ModelAdmin):
    list_filters = ('application', )

class HttpRouteAdmin(admin.ModelAdmin):
    list_filters = ('application', 'in_route', 'media_type')

admin.site.register(Streamer,   StreamerAdmin)
admin.site.register(HttpRoute,  HttpRouteAdmin)
admin.site.register(HttpMediaType)