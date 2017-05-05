
    AJAX_FORM_URL = 'test.html';
    //var st = new Element('a', { 'id': 'starter', 'text': 'go!' });
    //var el = new Element('div', { 'id': 'klk' });
    document.write("<a href='#' id='starter'>go</a><br/><div id='klk'></div>");
    //document.adopt(st);
    //document.adopt(el);
    //alert(AJAX_FORM_URL);
    var target = $('klk');
    
    
//    window.addEvent('domready', function() {
//    });
    
    var req = new Request.HTML( { url: AJAX_FORM_URL, update: target,
            onSuccess: function() {
                alert('ok');
            },
            onFailure: function(xhr) {
                alert('fail');
                target.empty();
                var el = new Element('h2', { 'text': 'Request failed!' }).inject(target);
            }
        });
    $('starter').addEvent('click', function() { req.send() });
    
    
    
