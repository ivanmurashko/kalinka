from django.contrib import admin
from models import *

class AddressInline(admin.TabularInline):
    model = InterfaceAddress
    extra = 2

class ResourceAdmin(admin.ModelAdmin):
    inlines = (AddressInline, )

class RouteAdmin(admin.ModelAdmin):
    list_dysplay  = ('name', 'address', 'host', 'port', 'protocol', 'lock_timestamp')
    list_filter = ('protocol', 'address')

admin.site.register(NetworkType)
admin.site.register(Trunk)
admin.site.register(InterfaceResource, ResourceAdmin)
admin.site.register(InterfaceAddress)
admin.site.register(Protocol)
admin.site.register(Route,             RouteAdmin)

