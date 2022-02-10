function (event) {

    if (event.type == 'start') {
        event.icon.find('.cardinal-fx-knob-page-selector .page-1').click(function() {
            event.icon.find('.cardinal-fx-knob-page-2').css({'display':'none'});
            event.icon.find('.cardinal-fx-knob-page-3').css({'display':'none'});
            event.icon.find('.cardinal-fx-knob-page-1').css({'display':'flex'});
            event.icon.find('.cardinal-fx-knob-page-selector > span').removeClass('selected');
            $(this).addClass('selected');
        });
        event.icon.find('.cardinal-fx-knob-page-selector .page-2').click(function() {
            event.icon.find('.cardinal-fx-knob-page-1').css({'display':'none'});
            event.icon.find('.cardinal-fx-knob-page-3').css({'display':'none'});
            event.icon.find('.cardinal-fx-knob-page-2').css({'display':'flex'});
            event.icon.find('.cardinal-fx-knob-page-selector > span').removeClass('selected');
            $(this).addClass('selected');
        });
        event.icon.find('.cardinal-fx-knob-page-selector .page-3').click(function() {
            event.icon.find('.cardinal-fx-knob-page-1').css({'display':'none'});
            event.icon.find('.cardinal-fx-knob-page-2').css({'display':'none'});
            event.icon.find('.cardinal-fx-knob-page-3').css({'display':'flex'});
            event.icon.find('.cardinal-fx-knob-page-selector > span').removeClass('selected');
            $(this).addClass('selected');
        });
        return;
    }

    if (event.type == 'change') {
        if (event.uri === "https://distrho.kx.studio/plugins/cardinal#fx#screenshot") {
            if (event.value) {
                event.icon.find('.screenshot').css({'background-image':'url(data:image/png;base64,'+event.value+')'});
                event.icon.find('.cardinal-fx.mod-pedal .cardinal-patch-comment').hide();
                event.icon.find('.cardinal-fx.mod-pedal .cardinal-patch-screeenshot').show();
            } else {
                event.icon.find('.cardinal-fx.mod-pedal .cardinal-patch-screeenshot').hide();
                event.icon.find('.cardinal-fx.mod-pedal .cardinal-patch-comment').show();
            }
        }
        return;
    }
}
