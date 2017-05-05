from django.template import Context, loader
from django.http import Http404, HttpResponse, HttpResponseRedirect
from django.shortcuts import render_to_response
from django.contrib.auth.decorators import login_required

from django.utils import simplejson

from kalinka.settings import *

from kalinka.dvbstreamer.models import *
from kalinka.httpstreamer.models import *
from kalinka.network.models import *
from kalinka.front.models import *

HOSTS = {'192.168.134.2': 'beethoven.lan.spbgasu.ru',
         '192.168.134.3': 'hendel.lan.spbgasu.ru'}

class PlaylistTrack(object):
    def __init__(self, **kwargs):
        self.title = kwargs.get('title')
        self.link  = kwargs.get('link')
        self.pict  = kwargs.get('pict')

class Playlist(object):
    def __init__(self, *args):
        self.tracks = []
        self.title  = 'Playlist'
        #self.mimetype = 'application/xspf+xml'
    
    def add(self, new_track):
        self.tracks.append(new_track)
    
    def to_json(self):
        tracks = []
        for tr in self.tracks:
            tracks.append({ 'title': tr.title, 'link': tr.link })
        return { 'title': self.title, 'tracks': tracks }

def getPlaylist(proto, user=None):
    pl = Playlist()
    if proto == 'udp':
        prefix = 'udp://@'
        pl.title = 'UDP-playlist'
        for station in Station.objects.filter(enable=True).all():
            title = station.channel.name
            host, port  = station.route.host, station.route.port
            
            link  = '%s%s:%d' % (prefix, host, port)
            tr = PlaylistTrack(title=title, link=link)
            pl.add(tr)
    elif proto == 'http':
        prefix = 'http://'
        pl.title = 'HTTP-playlist'
        
        active_stations = Station.objects.filter(enable=True)
        if user:
            for streamer in Streamer.objects.all():
                if user.is_superuser:
                    httproute_set = HttpRoute.objects
                else:
                    httproute_set = user.httproute_set
                
                for httproute in httproute_set.filter(application=streamer):
                    route = httproute.in_route
                    port  = streamer.out_route.port
                    path  = httproute.out_path
                    host = HOSTS.get(streamer.out_route.host, streamer.out_route.host)
                    try:
                        title = active_stations.filter(route=route)[0].channel.name
                    except:
                        title = 'Camera'
                    
                    link  = '%s%s:%d%s' % (prefix, host, port, path)
                    tr = PlaylistTrack(title=title, link=link)
                    pl.add(tr)
        else:
            for streamer in Streamer.objects.all():
                for httproute in HttpRoute.objects.filter(application=streamer):
                    route = httproute.in_route
                    port  = streamer.out_route.port
                    path  = httproute.out_path
                    host = HOSTS.get(streamer.out_route.host, streamer.out_route.host)
                    try:
                        title = active_stations.filter(route=route)[0].channel.name
                    except:
                        title = 'Camera'
                        continue
                    link  = '%s%s:%d%s' % (prefix, host, port, path)
                    tr = PlaylistTrack(title=title, link=link)
                    pl.add(tr)
    else:
        return None
    return pl

def playlist(request, proto):
    c = {'playlist': getPlaylist(proto)}
    return render_to_response('front/playlist.xml', c, mimetype='application/xspf+xml')


def ajax_playlist(request, proto):
    if DEBUG or request.is_ajax():
        json_str = simplejson.dumps(getPlaylist(proto, user=request.user).to_json())
    return HttpResponse(json_str)


def ajax_player(request, player='flash'):
    #raise Exception(request.GET.get('link', 'fuck'))
    c = {'player': player, 'link': request.GET.get('link', 'no link =('), 'title': request.GET.get('title', 'No title =(')}
    return render_to_response('front/player.html', c)
    
@login_required
def watch(request, player='vlc'):
    c = {'playlist': getPlaylist('http', user=request.user), 'user': request.user, 'player': player, 'player_list': ('flash', 'vlc', 'flow', 'jv', 'h264', 'uppod')}
    return render_to_response('front/watch.html', c)


@login_required
def test(request):
    c = {'playlist': getPlaylist('http', user=request.user)}
    return render_to_response('front/test.html', c)


def dashboard(request):
    pass
    #c = {'playlist': getPlaylist('http', None), 'user': request.user, 'vlc': True}
    #return render_to_response('front/watch.html', c)