from django.contrib import admin
from models import *

class TranscodeSourceAdmin(admin.ModelAdmin):
    list_filter = ('type', )

class TranscodeTaskAdmin(admin.ModelAdmin):
    list_filter = ('media_type', )

admin.site.register(TranscodeSourceType)
admin.site.register(TranscodeSource,    TranscodeSourceAdmin)
admin.site.register(TranscodeTask,      TranscodeTaskAdmin)
#admin.site.register(Transcode)