from django.contrib import admin
from models import *

class SignalSourceAdmin(admin.ModelAdmin):
    list_filter = ('signal_source_type', )

class ChannelAdmin(admin.ModelAdmin):
    list_display = ('channel_no', 'name', 'signal_source')
    list_filter = ('signal_source', )
    search_fields = ('name', )


admin.site.register(Type)
admin.site.register(SignalSourceType)
admin.site.register(SignalSource,       SignalSourceAdmin)
admin.site.register(Channel,            ChannelAdmin)
admin.site.register(SatelliteChannel,   ChannelAdmin)
admin.site.register(DigitalTVChannel,   ChannelAdmin)
admin.site.register(CChannel,           ChannelAdmin)
#admin.site.register(PidType)
#admin.site.register(Pid)
#admin.site.register(SatellitePid)
#admin.site.register(DigitalTVPid)
#admin.site.register(CPid)
#admin.site.register(TunedChannel)
#admin.site.register(UsedChannel)