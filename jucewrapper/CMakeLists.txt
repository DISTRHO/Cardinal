cmake_minimum_required(VERSION 3.15)
project(Cardinal VERSION 0.0.0)

add_subdirectory(JUCE)

# Config

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Define static libs

add_library(dgl STATIC IMPORTED)
set_property(TARGET dgl PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../dpf/build/libdgl-opengl.a")

add_library(carla_host_plugin STATIC IMPORTED)
set_property(TARGET carla_host_plugin PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/plugin/Release/carla-host-plugin.cpp.o")

add_library(carla_engine_plugin STATIC IMPORTED)
set_property(TARGET carla_engine_plugin PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/carla_engine_plugin.a")

add_library(carla_plugin STATIC IMPORTED)
set_property(TARGET carla_plugin PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/carla_plugin.a")

add_library(native_plugins STATIC IMPORTED)
set_property(TARGET native_plugins PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/native-plugins.a")

add_library(audio_decoder STATIC IMPORTED)
set_property(TARGET audio_decoder PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/audio_decoder.a")

add_library(jackbridge STATIC IMPORTED)
set_property(TARGET jackbridge PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/jackbridge.min.a")

add_library(lilv STATIC IMPORTED)
set_property(TARGET lilv PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/lilv.a")

add_library(rtmempool STATIC IMPORTED)
set_property(TARGET rtmempool PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/rtmempool.a")

add_library(sfzero STATIC IMPORTED)
set_property(TARGET sfzero PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/sfzero.a")

add_library(water STATIC IMPORTED)
set_property(TARGET water PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/water.a")

add_library(zita_resampler STATIC IMPORTED)
set_property(TARGET zita_resampler PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../carla/build/modules/Release/zita-resampler.a")

add_library(sCardinalFX STATIC IMPORTED)
set_property(TARGET sCardinalFX PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../bin/CardinalFX.a")

add_library(sPlugins STATIC IMPORTED)
set_property(TARGET sPlugins PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../plugins/plugins.a")

add_library(sRack STATIC IMPORTED)
set_property(TARGET sRack PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../src/rack.a")

#ifeq ($(WINDOWS),true)
#RACK_EXTRA_LIBS += $(DEP_LIB_PATH)/libarchive_static.a
add_library(libarchive STATIC IMPORTED)
set_property(TARGET libarchive PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../src/Rack/dep/lib/libarchive.a")

add_library(libjansson STATIC IMPORTED)
set_property(TARGET libjansson PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../src/Rack/dep/lib/libjansson.a")

add_library(libquickjs STATIC IMPORTED)
set_property(TARGET libquickjs PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../src/Rack/dep/lib/libquickjs.a")

add_library(libsamplerate STATIC IMPORTED)
set_property(TARGET libsamplerate PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../src/Rack/dep/lib/libsamplerate.a")

add_library(libspeexdsp STATIC IMPORTED)
set_property(TARGET libspeexdsp PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../src/Rack/dep/lib/libspeexdsp.a")

add_library(libzstd STATIC IMPORTED)
set_property(TARGET libzstd PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../src/Rack/dep/lib/libzstd.a")

#find_package(Dbus)
find_package(OpenGL)
find_package(X11)

# FX variant

juce_add_plugin(CardinalFX
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    PLUGIN_MANUFACTURER_CODE Dstr
    PLUGIN_CODE dCnF
    FORMATS Standalone AU
    PRODUCT_NAME "CardinalFX")

target_sources(CardinalFX
    PRIVATE
        CardinalWrapper.cpp)

target_include_directories(CardinalFX
    PRIVATE
        .
        ../dpf/distrho)

target_compile_definitions(CardinalFX
    PUBLIC
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_CURL=0
        JUCE_WEB_BROWSER=0)

target_link_directories(CardinalFX
    PUBLIC
        /opt/kxstudio/lib)

target_link_libraries(CardinalFX
    PRIVATE
        juce::juce_audio_utils
        -Wl,--whole-archive
        sCardinalFX
        sPlugins
        sRack
        carla_host_plugin
        carla_engine_plugin
        carla_plugin
        native_plugins
        audio_decoder
        jackbridge
        lilv
        rtmempool
        sfzero
        water
        zita_resampler
        dgl
        libarchive
        libjansson
        libquickjs
        libsamplerate
        libspeexdsp
        libzstd
        -Wl,--no-whole-archive
        GLX
        OpenGL
        X11
        Xcursor
        Xext
        Xrandr
    PUBLIC
        -ldbus-1
        -llo
        -lmagic
        -lsndfile -lFLAC -lvorbisenc -lvorbis -logg
        -lrt
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Synth variant
