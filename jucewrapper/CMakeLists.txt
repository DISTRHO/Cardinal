cmake_minimum_required(VERSION 3.15)
project(Cardinal VERSION 0.0.0)

add_subdirectory(JUCE)

# FX variant

juce_add_plugin(CardinalFX
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    PLUGIN_MANUFACTURER_CODE Dstr
    PLUGIN_CODE dCnF
    FORMATS AU
    PRODUCT_NAME "CardinalFX")

target_sources(CardinalFX
    PRIVATE
        Source/CardinalWrapper.cpp)

target_compile_definitions(CardinalFX
    PUBLIC
        JUCE_WEB_BROWSER=0)

target_link_libraries(CardinalFX
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Synth variant
