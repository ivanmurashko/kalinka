# This is an auto-generated Django model module.
# You'll have to do the following manually to clean this up:
#     * Rearrange models' order
#     * Make sure each model has one field with primary_key=True
# Feel free to rename the models, but don't rename db_table values or field names.
#
# Also note: You'll have to insert the output of 'django-admin.py sqlcustom [appname]'
# into your database.

from django.db import models

class AuthGroup(models.Model):
    id = models.IntegerField(primary_key=True)
    name = models.CharField(unique=True, max_length=240)
    class Meta:
        db_table = u'auth_group'

class AuthGroupPermissions(models.Model):
    id = models.IntegerField(primary_key=True)
    group_id = models.IntegerField(unique=True)
    permission_id = models.IntegerField()
    class Meta:
        db_table = u'auth_group_permissions'

class AuthMessage(models.Model):
    id = models.IntegerField(primary_key=True)
    user_id = models.IntegerField()
    message = models.TextField()
    class Meta:
        db_table = u'auth_message'

class AuthPermission(models.Model):
    id = models.IntegerField(primary_key=True)
    name = models.CharField(max_length=150)
    content_type_id = models.IntegerField()
    codename = models.CharField(unique=True, max_length=300)
    class Meta:
        db_table = u'auth_permission'

class AuthUser(models.Model):
    id = models.IntegerField(primary_key=True)
    username = models.CharField(unique=True, max_length=90)
    first_name = models.CharField(max_length=90)
    last_name = models.CharField(max_length=90)
    email = models.CharField(max_length=225)
    password = models.CharField(max_length=384)
    is_staff = models.IntegerField()
    is_active = models.IntegerField()
    is_superuser = models.IntegerField()
    last_login = models.DateTimeField()
    date_joined = models.DateTimeField()
    class Meta:
        db_table = u'auth_user'

class AuthUserGroups(models.Model):
    id = models.IntegerField(primary_key=True)
    user_id = models.IntegerField(unique=True)
    group_id = models.IntegerField()
    class Meta:
        db_table = u'auth_user_groups'

class AuthUserUserPermissions(models.Model):
    id = models.IntegerField(primary_key=True)
    user_id = models.IntegerField(unique=True)
    permission_id = models.IntegerField()
    class Meta:
        db_table = u'auth_user_user_permissions'

class DjangoAdminLog(models.Model):
    id = models.IntegerField(primary_key=True)
    action_time = models.DateTimeField()
    user_id = models.IntegerField()
    content_type_id = models.IntegerField(null=True, blank=True)
    object_id = models.TextField(blank=True)
    object_repr = models.CharField(max_length=600)
    action_flag = models.IntegerField()
    change_message = models.TextField()
    class Meta:
        db_table = u'django_admin_log'

class DjangoContentType(models.Model):
    id = models.IntegerField(primary_key=True)
    name = models.CharField(max_length=300)
    app_label = models.CharField(unique=True, max_length=300)
    model = models.CharField(unique=True, max_length=300)
    class Meta:
        db_table = u'django_content_type'

class DjangoSession(models.Model):
    session_key = models.CharField(max_length=120, primary_key=True)
    session_data = models.TextField()
    expire_date = models.DateTimeField()
    class Meta:
        db_table = u'django_session'

class DjangoSite(models.Model):
    id = models.IntegerField(primary_key=True)
    domain = models.CharField(max_length=300)
    name = models.CharField(max_length=150)
    class Meta:
        db_table = u'django_site'

class KlkAdapter(models.Model):
    adapter = models.CharField(max_length=120, primary_key=True)
    host_uuid = models.ForeignKey(KlkHosts, db_column='host_uuid')
    addr = models.CharField(max_length=120)
    port = models.IntegerField()
    inuse = models.IntegerField()
    class Meta:
        db_table = u'klk_adapter'

class KlkAppDvbStreamer(models.Model):
    station = models.CharField(max_length=120, primary_key=True)
    channel = models.ForeignKey(KlkDvbChannels, db_column='channel')
    route = models.ForeignKey(KlkNetworkRoutes, db_column='route')
    enable = models.IntegerField()
    lock_used_host = models.CharField(max_length=120, blank=True)
    lock_timestamp = models.DateTimeField()
    priority = models.IntegerField()
    class Meta:
        db_table = u'klk_app_dvb_streamer'

class KlkAppDvbStreamerChannelView(models.Model):
    station_name = models.CharField(max_length=303, blank=True)
    channel = models.CharField(max_length=120)
    class Meta:
        db_table = u'klk_app_dvb_streamer_channel_view'

class KlkAppDvbStreamerFailedStations(models.Model):
    station_uuid = models.ForeignKey(KlkAppDvbStreamer, db_column='station_uuid')
    host_uuid = models.ForeignKey(KlkHosts, db_column='host_uuid')
    class Meta:
        db_table = u'klk_app_dvb_streamer_failed_stations'

class KlkAppDvbStreamerStationView(models.Model):
    station_name = models.CharField(max_length=303, blank=True)
    route_name = models.CharField(max_length=120)
    enable = models.IntegerField()
    priority = models.IntegerField()
    station = models.CharField(max_length=120)
    channel = models.CharField(max_length=120)
    route = models.CharField(max_length=120)
    lock_used_host = models.CharField(max_length=120, blank=True)
    lock_timestamp = models.DateTimeField()
    source_uuid = models.CharField(max_length=120, blank=True)
    class Meta:
        db_table = u'klk_app_dvb_streamer_station_view'

class KlkAppHttpStreamer(models.Model):
    application = models.ForeignKey(KlkApplications, db_column='application')
    out_route = models.ForeignKey(KlkNetworkRoutes, db_column='out_route')
    class Meta:
        db_table = u'klk_app_http_streamer'

class KlkAppHttpStreamerMediaTypes(models.Model):
    uuid = models.CharField(max_length=120, primary_key=True)
    media_type = models.ForeignKey(KlkMediaTypes, db_column='media_type')
    class Meta:
        db_table = u'klk_app_http_streamer_media_types'

class KlkAppHttpStreamerRoute(models.Model):
    uuid = models.CharField(max_length=120, primary_key=True)
    application = models.ForeignKey(KlkAppHttpStreamer, db_column='application')
    in_route = models.ForeignKey(KlkNetworkRoutes, db_column='in_route')
    out_path = models.CharField(unique=True, max_length=120)
    media_type = models.ForeignKey(KlkAppHttpStreamerMediaTypes, db_column='media_type')
    class Meta:
        db_table = u'klk_app_http_streamer_route'

class KlkAppHttpStreamerRouteAllowedTo(models.Model):
    id = models.IntegerField(primary_key=True)
    httproute_id = models.CharField(unique=True, max_length=120)
    user_id = models.IntegerField()
    class Meta:
        db_table = u'klk_app_http_streamer_route_allowed_to'

class KlkAppTranscodeSource(models.Model):
    source_uuid = models.CharField(max_length=120, primary_key=True)
    source_name = models.CharField(unique=True, max_length=765)
    source_type = models.ForeignKey(KlkAppTranscodeSourceType, db_column='source_type')
    class Meta:
        db_table = u'klk_app_transcode_source'

class KlkAppTranscodeSourceType(models.Model):
    type_uuid = models.CharField(max_length=120, primary_key=True)
    type_name = models.CharField(unique=True, max_length=120)
    class Meta:
        db_table = u'klk_app_transcode_source_type'

class KlkAppTranscodeTask(models.Model):
    task = models.CharField(max_length=120, primary_key=True)
    task_name = models.CharField(unique=True, max_length=384)
    input_source = models.ForeignKey(KlkAppTranscodeSource, db_column='input_source')
    output_source = models.ForeignKey(KlkAppTranscodeSource, db_column='output_source')
    media_type = models.ForeignKey(KlkMediaTypes, db_column='media_type')
    schedule_start = models.CharField(max_length=384)
    schedule_duration = models.IntegerField()
    class Meta:
        db_table = u'klk_app_transcode_task'

class KlkApplications(models.Model):
    application = models.CharField(unique=True, max_length=120)
    module = models.CharField(max_length=120)
    name = models.CharField(unique=True, max_length=150)
    enabled = models.IntegerField()
    priority = models.IntegerField()
    status = models.IntegerField()
    host = models.CharField(max_length=120, blank=True)
    description = models.CharField(max_length=765, blank=True)
    class Meta:
        db_table = u'klk_applications'

class KlkBusTypes(models.Model):
    bus_type = models.CharField(unique=True, max_length=120)
    description = models.CharField(unique=True, max_length=765)
    bandwidth = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = u'klk_bus_types'

class KlkBusses(models.Model):
    bus = models.CharField(unique=True, max_length=120)
    id = models.IntegerField(null=True, blank=True)
    host = models.ForeignKey(KlkHosts, db_column='host')
    description = models.CharField(unique=True, max_length=765, blank=True)
    bus_type = models.ForeignKey(KlkBusTypes, db_column='bus_type')
    bandwidth = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = u'klk_busses'

class KlkDvbCChannels(models.Model):
    channel = models.ForeignKey(KlkDvbChannels, db_column='channel')
    frequency = models.IntegerField()
    symbol_rate = models.IntegerField()
    class Meta:
        db_table = u'klk_dvb_c_channels'

class KlkDvbCPids(models.Model):
    id = models.IntegerField(primary_key=True)
    class Meta:
        db_table = u'klk_dvb_c_pids'

class KlkDvbChannels(models.Model):
    channel = models.CharField(max_length=120, primary_key=True)
    name = models.CharField(unique=True, max_length=150)
    provider = models.CharField(unique=True, max_length=150, blank=True)
    channel_no = models.IntegerField()
    signal_source = models.ForeignKey(KlkDvbSignalSources, null=True, db_column='signal_source', blank=True)
    bandwidth = models.IntegerField()
    dvb_type = models.ForeignKey(KlkDvbTypes, db_column='dvb_type')
    class Meta:
        db_table = u'klk_dvb_channels'

class KlkDvbPidType(models.Model):
    id = models.IntegerField(primary_key=True)
    class Meta:
        db_table = u'klk_dvb_pid_type'

class KlkDvbPidTypes(models.Model):
    pid_type = models.CharField(unique=True, max_length=120)
    description = models.CharField(max_length=765)
    bandwidth = models.IntegerField()
    class Meta:
        db_table = u'klk_dvb_pid_types'

class KlkDvbPids(models.Model):
    pid = models.CharField(unique=True, max_length=120)
    pid_type = models.CharField(max_length=120)
    channel = models.ForeignKey(KlkDvbChannels, db_column='channel')
    pid_no = models.IntegerField(null=True, blank=True)
    description = models.CharField(max_length=765)
    class Meta:
        db_table = u'klk_dvb_pids'

class KlkDvbResources(models.Model):
    resource = models.ForeignKey(KlkResources, db_column='resource')
    dvb_type = models.ForeignKey(KlkDvbTypes, db_column='dvb_type')
    signal_source = models.ForeignKey(KlkDvbSignalSources, null=True, db_column='signal_source', blank=True)
    adapter_no = models.IntegerField(null=True, blank=True)
    frontend_no = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = u'klk_dvb_resources'

class KlkDvbSChannels(models.Model):
    channel = models.ForeignKey(KlkDvbChannels, db_column='channel')
    diseq_source = models.IntegerField(null=True, blank=True)
    frequency = models.IntegerField()
    polarity = models.CharField(max_length=3)
    symbol_rate = models.IntegerField()
    code_rate_hp = models.IntegerField()
    class Meta:
        db_table = u'klk_dvb_s_channels'

class KlkDvbSPids(models.Model):
    id = models.IntegerField(primary_key=True)
    class Meta:
        db_table = u'klk_dvb_s_pids'

class KlkDvbSignalSource(models.Model):
    signal_source = models.CharField(max_length=120, primary_key=True)
    signal_source_type = models.CharField(max_length=120)
    description = models.CharField(max_length=765)
    class Meta:
        db_table = u'klk_dvb_signal_source'

class KlkDvbSignalSourceTypes(models.Model):
    signal_source_type = models.CharField(max_length=120, primary_key=True)
    description = models.CharField(max_length=765)
    class Meta:
        db_table = u'klk_dvb_signal_source_types'

class KlkDvbSignalSources(models.Model):
    signal_source = models.CharField(max_length=120, primary_key=True)
    dvb_type = models.ForeignKey(KlkDvbTypes, db_column='dvb_type')
    name = models.CharField(unique=True, max_length=300)
    description = models.CharField(max_length=765)
    class Meta:
        db_table = u'klk_dvb_signal_sources'

class KlkDvbTChannels(models.Model):
    channel = models.ForeignKey(KlkDvbChannels, db_column='channel')
    frequency = models.IntegerField()
    dvb_bandwidth = models.IntegerField()
    code_rate_hp = models.IntegerField()
    code_rate_lp = models.IntegerField()
    modulation = models.IntegerField()
    transmode = models.IntegerField()
    guard = models.IntegerField()
    hierarchy = models.IntegerField()
    class Meta:
        db_table = u'klk_dvb_t_channels'

class KlkDvbTPids(models.Model):
    id = models.IntegerField(primary_key=True)
    class Meta:
        db_table = u'klk_dvb_t_pids'

class KlkDvbTunedChannels(models.Model):
    id = models.IntegerField(primary_key=True)
    class Meta:
        db_table = u'klk_dvb_tuned_channels'

class KlkDvbTypes(models.Model):
    dvb_type = models.CharField(unique=True, max_length=120)
    name = models.CharField(unique=True, max_length=120)
    description = models.CharField(max_length=765)
    class Meta:
        db_table = u'klk_dvb_types'

class KlkDvbUsed(models.Model):
    id = models.IntegerField(primary_key=True)
    class Meta:
        db_table = u'klk_dvb_used'

class KlkFiles(models.Model):
    file = models.CharField(max_length=120, primary_key=True)
    name = models.CharField(unique=True, max_length=120)
    file_path = models.CharField(unique=True, max_length=765)
    host = models.ForeignKey(KlkHosts, db_column='host')
    class Meta:
        db_table = u'klk_files'

class KlkHosts(models.Model):
    host = models.CharField(unique=True, max_length=120)
    host_name = models.CharField(unique=True, max_length=150)
    cpu_index_typ = models.IntegerField()
    cpu_index_max = models.IntegerField()
    class Meta:
        db_table = u'klk_hosts'

class KlkHttpsrc(models.Model):
    httpsrc = models.CharField(max_length=120, primary_key=True)
    title = models.CharField(unique=True, max_length=192)
    address = models.CharField(unique=True, max_length=765)
    login = models.CharField(max_length=96)
    passwd = models.CharField(max_length=96)
    description = models.CharField(max_length=765)
    class Meta:
        db_table = u'klk_httpsrc'

class KlkIeee1394(models.Model):
    uuid = models.CharField(max_length=120, primary_key=True)
    name = models.CharField(unique=True, max_length=120)
    description = models.CharField(max_length=765)
    class Meta:
        db_table = u'klk_ieee1394'

class KlkLog(models.Model):
    id = models.IntegerField(primary_key=True)
    module_uuid = models.CharField(max_length=120)
    timestamp = models.DateTimeField()
    class Meta:
        db_table = u'klk_log'

class KlkMediaTypes(models.Model):
    uuid = models.CharField(max_length=120, primary_key=True)
    name = models.CharField(unique=True, max_length=120)
    description = models.CharField(max_length=120)
    class Meta:
        db_table = u'klk_media_types'

class KlkModuleTypes(models.Model):
    module_type = models.CharField(max_length=120, primary_key=True)
    cpu_index_typ = models.IntegerField()
    cpu_index_max = models.IntegerField()
    description = models.CharField(max_length=765)
    class Meta:
        db_table = u'klk_module_types'

class KlkModulesAllocated(models.Model):
    module = models.CharField(max_length=120, primary_key=True)
    application = models.CharField(max_length=120)
    module_type = models.CharField(max_length=120)
    host = models.CharField(max_length=120)
    class Meta:
        db_table = u'klk_modules_allocated'

class KlkNetworkInterfaceAddresses(models.Model):
    address = models.CharField(unique=True, max_length=120)
    resource = models.ForeignKey(KlkResources, db_column='resource')
    ip_address = models.CharField(max_length=48)
    ip_mask = models.CharField(max_length=48, blank=True)
    ip_network = models.CharField(max_length=48, blank=True)
    class Meta:
        db_table = u'klk_network_interface_addresses'

class KlkNetworkInterfaceResources(models.Model):
    resource = models.ForeignKey(KlkResources, db_column='resource')
    trunk = models.CharField(max_length=120)
    node_name = models.CharField(max_length=30)
    class Meta:
        db_table = u'klk_network_interface_resources'

class KlkNetworkMediaTypes(models.Model):
    media_type = models.CharField(unique=True, max_length=120)
    bandwidth = models.IntegerField()
    description = models.CharField(unique=True, max_length=765)
    class Meta:
        db_table = u'klk_network_media_types'

class KlkNetworkProtocols(models.Model):
    protocol = models.CharField(max_length=120, primary_key=True)
    proto_name = models.CharField(unique=True, max_length=120)
    description = models.CharField(max_length=120)
    class Meta:
        db_table = u'klk_network_protocols'

class KlkNetworkRoutes(models.Model):
    route = models.CharField(unique=True, max_length=120)
    name = models.CharField(unique=True, max_length=120)
    address = models.ForeignKey(KlkNetworkInterfaceAddresses, db_column='address')
    host = models.CharField(unique=True, max_length=192)
    port = models.IntegerField(unique=True)
    protocol = models.ForeignKey(KlkNetworkProtocols, db_column='protocol')
    lock_timestamp = models.DateTimeField()
    lock_resource = models.CharField(max_length=120, blank=True)
    class Meta:
        db_table = u'klk_network_routes'

class KlkNetworkTrunks(models.Model):
    trunk = models.CharField(unique=True, max_length=120)
    media_type = models.ForeignKey(KlkNetworkMediaTypes, db_column='media_type')
    description = models.CharField(max_length=765)
    class Meta:
        db_table = u'klk_network_trunks'

class KlkResourceTypes(models.Model):
    resource_type = models.CharField(unique=True, max_length=120)
    description = models.CharField(unique=True, max_length=765)
    bandwidth = models.IntegerField()
    class Meta:
        db_table = u'klk_resource_types'

class KlkResources(models.Model):
    resource = models.CharField(unique=True, max_length=120)
    id = models.IntegerField(null=True, blank=True)
    bus = models.ForeignKey(KlkBusses, db_column='bus')
    resource_type = models.ForeignKey(KlkResourceTypes, db_column='resource_type')
    bandwidth = models.IntegerField()
    enabled = models.IntegerField()
    resource_name = models.CharField(unique=True, max_length=150)
    class Meta:
        db_table = u'klk_resources'

