from django.template import Context, loader
from django.http import Http404, HttpResponse, HttpResponseRedirect
from django.shortcuts import render_to_response
from django.contrib.auth.decorators import login_required

from kalinka.settings import *

from kalinka.front.models import *
from kalinka.front.views import *



@login_required
def watch(request):
    c = { 'is_popup': True, 'playlist': getPlaylist('http', user=request.user), 'user': request.user }
    if request.GET:
        c['link']  = request.GET.get('link')
        c['title'] = request.GET.get('title')
        return render_to_response('light/player.html', c)
    else:
        return render_to_response('light/watch.html', c)