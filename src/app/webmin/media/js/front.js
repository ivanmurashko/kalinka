function togglePause(player){
    player.playlist.togglePause();
    return player.playlist.isPlaying;
}

function toggleMute(player){
    player.audio.toggleMute();
    return player.audio.mute;
}

function playNow(player, link){
    player.playlist.clear();
    id = player.playlist.add(link);
    player.playlist.playItem(id);
}

function updatePlaylist(el, link){
    el.empty();
}

function createVlc(){
    alert('!');
}

function prepareAjaxPlaylist(handle, target) {
    
    function addTracks(jsonObj){
        if (jsonObj.title){
            var el = new Element('h2', {'text': jsonObj.title}).inject(target);
        };
        $each(jsonObj.tracks, function(obj){
            var li = new Element('li').inject(target);
            var a  = new Element('a', {'class': 'toggler chanel', 'text': obj.title, 'href': obj.link}).inject(li);
            a.addEvent('click', function(event){
                event.stop();
                playNow(a);
            });
        });
    }
    
    var req = new Request.JSON({
        url: handle,
        onComplete: function(jsonObj){
            target.empty();
            if (jsonObj.tracks.length) {
                addTracks(jsonObj, $(target));
            } else {
                var el = new Element('h2', {'html': "There are no channels"}).inject(target);
            }
        },
        onFailure: function(){
            target.empty();
            var el = new Element('h2', {'html': "Ajax-request failed"}).inject(target);
        }
    });
    
    handle.addEvent('click', function(event){
        event.stop();
        target.empty();
        var el = new Element('span', {'class': 'ajax-loading', 'text': 'loading...'}).inject(target);
        req.send();
    });
}

window.addEvent('domready', function() {
    
    var logger = new Log;
    
    if ($('vlc-container')) {
        //createVlc();
        var vlc = new VLCObject("vlc", "640", "480", "0.8.6");
        logger.log('vlcObject created');
        vlc.write('vlc-container');
        logger.log('object was writed');
        vlc = $('vlc');
        
        $('vlc-playpause').addEvent('click', function(event){
            event.stop();
            if (togglePause(vlc)) {
                event.target.removeClass('play');
                event.target.addClass('pause');
            } else {
                event.target.removeClass('pause');
                event.target.addClass('play');
            };
        });
        $('vlc-stop').addEvent('click', function(event){
            event.stop();
            vlc.playlist.stop();
        });
        $('vlc-mute').addEvent('click', function(event){
            event.stop();
            if (toggleMute(vlc)) {
                event.target.addClass('muted');
            } else {
                event.target.removeClass('muted');
            }
        });
        
        $each($$('a.channel'), function(el){
            el.addEvent('click', function(event){
                event.stop();
                playNow(vlc, el.href);
            });
        });
        
        prepareAjaxPlaylist($('get-http-playlist'), $('playlist'));
        prepareAjaxPlaylist($('get-udp-playlist'), $('playlist'));
    };
    
    //if ($('NAKO')) {
    //    $each($$('a.channel'), function(el){
    //        el.addEvent('click', function(event){
    //            event.stop();
    //            $('NAKO').sendEvent("LOAD", el.href);
    //        });
    //    });
    //}
    
    //var req = new Request.HTML({url: '/iface/front/ajax/player/', update: $('player-container')});
    //$each($$('a.channel'), function(el){
    //    //var req = new Request.HTML({url: '/iface/front/ajax/player/?link=' +el.href +'&title=' +el.text, update: $('player-container')});
    //    el.addEvent('click', function(event){
    //        event.stop();
    //        //req.get({'link': el, 'title': el.text});
    //        
    //        //$('player').sendToUppod('file:' +el, '');
    //        //alert('!');
    //    });
    //});
    
});