function (event) {

    if (event.type == 'start') {
        event.data['visibility-audio'] = true;
        event.data['visibility-cv'] = true;
        event.icon.find('.visibility-audio').click(function() {
            var visible = event.data['visibility-audio'];
            if (visible) {
                event.icon.find('.mod-pedal-input .mod-audio-input').addClass('cardinal-expanded');
                event.icon.find('.mod-pedal-output .mod-audio-output').addClass('cardinal-expanded');
                $(this).text('Hide Audio');
            } else {
                event.icon.find('.mod-pedal-input .mod-audio-input').removeClass('cardinal-expanded');
                event.icon.find('.mod-pedal-output .mod-audio-output').removeClass('cardinal-expanded');
                $(this).text('Show Audio');
            }
            event.data['visibility-audio'] = !visible;
        });
        event.icon.find('.visibility-cv').click(function() {
            var visible = event.data['visibility-cv'];
            if (visible) {
                event.icon.find('.mod-pedal-input .mod-cv-input').addClass('cardinal-expanded');
                event.icon.find('.mod-pedal-output .mod-cv-output').addClass('cardinal-expanded');
                $(this).text('Hide CV');
            } else {
                event.icon.find('.mod-pedal-input .mod-cv-input').removeClass('cardinal-expanded');
                event.icon.find('.mod-pedal-output .mod-cv-output').removeClass('cardinal-expanded');
                $(this).text('Show CV');
            }
            event.data['visibility-cv'] = !visible;
        });
        return;
    }

    if (event.type == 'change') {
        console.log("change", event)
        if (event.uri === "https://distrho.kx.studio/plugins/cardinal#screenshot") {
            if (event.value) {
                event.icon.find('.screenshot').css({'background-image':'url(data:image/png;base64,'+event.value+')'});
                event.icon.find('.cardinal-main.mod-pedal .cardinal-patch-comment').hide();
                event.icon.find('.cardinal-main.mod-pedal .cardinal-patch-screeenshot').show();
            } else {
                event.icon.find('.cardinal-main.mod-pedal .cardinal-patch-screeenshot').hide();
                event.icon.find('.cardinal-main.mod-pedal .cardinal-patch-comment').show();
            }
        }
        return;
    }
}
