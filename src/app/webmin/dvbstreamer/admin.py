from django.contrib import admin
from models import *

class StationAdmin(admin.ModelAdmin):
    list_display = ('channel', 'route', 'enable', 'priority')

admin.site.register(Station,        StationAdmin)
admin.site.register(FailedStation)