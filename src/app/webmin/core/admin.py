from django.contrib import admin
from models import *

# admin-pages metadata
class ApplicationAdmin(admin.ModelAdmin):
    list_filter  = ('host', 'status', 'enabled')

class ModuleAllocationAdmin(admin.ModelAdmin):
    list_filter  = ('module_type', 'host')

class BusAdmin(admin.ModelAdmin):
    list_filter  = ('bus_type', 'host')

class ResourceAdmin(admin.ModelAdmin):
    list_filter  = ('resource_type', 'bus')

class AdapterAdmin(admin.ModelAdmin):
    list_filter  = ('host', )

class FileAdmin(admin.ModelAdmin):
    list_filter = ('host', )


#admin.site.register(Log)
admin.site.register(MediaType)
admin.site.register(Host)
admin.site.register(Application,    ApplicationAdmin)
admin.site.register(ModuleType)
admin.site.register(ModuleAllocation,   ModuleAllocationAdmin)
admin.site.register(BusType)
admin.site.register(Bus,            BusAdmin)
admin.site.register(ResourceType)
admin.site.register(Resource,       ResourceAdmin)
admin.site.register(Adapter,        AdapterAdmin)
admin.site.register(FileType)
admin.site.register(File,           FileAdmin)
