#!/usr/bin/make -f
# Makefile for Cardinal #
# --------------------- #
# Created by falkTX
#

BUILDING_RACK = true
ROOT = ..
include $(ROOT)/Makefile.base.mk

# --------------------------------------------------------------
# List of drwav functions, used in several modules

DRWAV  = drwav
DRWAV += drwav__on_read
DRWAV += drwav__on_seek
DRWAV += drwav__read_and_close_f32
DRWAV += drwav__read_and_close_s16
DRWAV += drwav__read_and_close_s32
DRWAV += drwav_alaw_to_f32
DRWAV += drwav_alaw_to_s16
DRWAV += drwav_alaw_to_s16
DRWAV += drwav_alaw_to_s32
DRWAV += drwav_bytes_to_f32
DRWAV += drwav_bytes_to_s16
DRWAV += drwav_bytes_to_s32
DRWAV += drwav_bytes_to_s64
DRWAV += drwav_bytes_to_u16
DRWAV += drwav_bytes_to_u32
DRWAV += drwav_bytes_to_u64
DRWAV += drwav_close
DRWAV += drwav_close
DRWAV += drwav_container
DRWAV += drwav_data_chunk_size_riff
DRWAV += drwav_data_chunk_size_w64
DRWAV += drwav_data_format
DRWAV += drwav_f32_to_s16
DRWAV += drwav_f32_to_s32
DRWAV += drwav_f64_to_f32
DRWAV += drwav_f64_to_s16
DRWAV += drwav_f64_to_s16
DRWAV += drwav_f64_to_s32
DRWAV += drwav_fmt_get_format
DRWAV += drwav_fopen
DRWAV += drwav_fourcc_equal
DRWAV += drwav_free
DRWAV += drwav_get_cursor_in_pcm_frames
DRWAV += drwav_get_length_in_pcm_frames
DRWAV += drwav_guid_equal
DRWAV += drwav_init
DRWAV += drwav_init_ex
DRWAV += drwav_init_file
DRWAV += drwav_init_file_ex
DRWAV += drwav_init_file_ex_w
DRWAV += drwav_init_file_w
DRWAV += drwav_init_file_with_metadata
DRWAV += drwav_init_file_with_metadata_w
DRWAV += drwav_init_file_write
DRWAV += drwav_init_file_write
DRWAV += drwav_init_file_write__internal
DRWAV += drwav_init_file_write__internal
DRWAV += drwav_init_file_write_sequential
DRWAV += drwav_init_file_write_sequential
DRWAV += drwav_init_file_write_sequential_pcm_frames
DRWAV += drwav_init_file_write_sequential_pcm_frames_w
DRWAV += drwav_init_file_write_sequential_w
DRWAV += drwav_init_file_write_w
DRWAV += drwav_init_memory
DRWAV += drwav_init_memory_ex
DRWAV += drwav_init_memory_with_metadata
DRWAV += drwav_init_memory_write
DRWAV += drwav_init_memory_write
DRWAV += drwav_init_memory_write__internal
DRWAV += drwav_init_memory_write__internal
DRWAV += drwav_init_memory_write_sequential
DRWAV += drwav_init_memory_write_sequential_pcm_frames
DRWAV += drwav_init_with_metadata
DRWAV += drwav_init_write
DRWAV += drwav_init_write
DRWAV += drwav_init_write__internal
DRWAV += drwav_init_write_sequential
DRWAV += drwav_init_write_sequential_pcm_frames
DRWAV += drwav_init_write_with_metadata
DRWAV += drwav_metadata
DRWAV += drwav__metadata_parser
DRWAV += drwav_mulaw_to_f32
DRWAV += drwav_mulaw_to_s16
DRWAV += drwav_mulaw_to_s16
DRWAV += drwav_mulaw_to_s32
DRWAV += drwav_open
DRWAV += drwav_open_and_read_f32
DRWAV += drwav_open_and_read_file_f32
DRWAV += drwav_open_and_read_file_s16
DRWAV += drwav_open_and_read_file_s32
DRWAV += drwav_open_and_read_memory_f32
DRWAV += drwav_open_and_read_memory_s16
DRWAV += drwav_open_and_read_memory_s32
DRWAV += drwav_open_and_read_pcm_frames_f32
DRWAV += drwav_open_and_read_pcm_frames_s16
DRWAV += drwav_open_and_read_pcm_frames_s32
DRWAV += drwav_open_and_read_s16
DRWAV += drwav_open_and_read_s32
DRWAV += drwav_open_ex
DRWAV += drwav_open_file
DRWAV += drwav_open_file_and_read_f32
DRWAV += drwav_open_file_and_read_pcm_frames_f32
DRWAV += drwav_open_file_and_read_pcm_frames_f32_w
DRWAV += drwav_open_file_and_read_pcm_frames_s16
DRWAV += drwav_open_file_and_read_pcm_frames_s16_w
DRWAV += drwav_open_file_and_read_pcm_frames_s32
DRWAV += drwav_open_file_and_read_pcm_frames_s32_w
DRWAV += drwav_open_file_and_read_s16
DRWAV += drwav_open_file_and_read_s32
DRWAV += drwav_open_file_ex
DRWAV += drwav_open_file_write
DRWAV += drwav_open_file_write
DRWAV += drwav_open_file_write__internal
DRWAV += drwav_open_file_write__internal
DRWAV += drwav_open_file_write_sequential
DRWAV += drwav_open_file_write_sequential
DRWAV += drwav_open_memory
DRWAV += drwav_open_memory_and_read_f32
DRWAV += drwav_open_memory_and_read_pcm_frames_f32
DRWAV += drwav_open_memory_and_read_pcm_frames_s16
DRWAV += drwav_open_memory_and_read_pcm_frames_s32
DRWAV += drwav_open_memory_and_read_s16
DRWAV += drwav_open_memory_and_read_s32
DRWAV += drwav_open_memory_ex
DRWAV += drwav_open_memory_write
DRWAV += drwav_open_memory_write
DRWAV += drwav_open_memory_write__internal
DRWAV += drwav_open_memory_write__internal
DRWAV += drwav_open_memory_write_sequential
DRWAV += drwav_open_write
DRWAV += drwav_open_write
DRWAV += drwav_open_write__internal
DRWAV += drwav_open_write_sequential
DRWAV += drwav_read
DRWAV += drwav_read_f32
DRWAV += drwav_read_f32__alaw
DRWAV += drwav_read_f32__alaw
DRWAV += drwav_read_f32__ieee
DRWAV += drwav_read_f32__ieee
DRWAV += drwav_read_f32__ima
DRWAV += drwav_read_f32__ima
DRWAV += drwav_read_f32__msadpcm
DRWAV += drwav_read_f32__msadpcm
DRWAV += drwav_read_f32__mulaw
DRWAV += drwav_read_f32__mulaw
DRWAV += drwav_read_f32__pcm
DRWAV += drwav_read_f32__pcm
DRWAV += drwav_read_pcm_frames
DRWAV += drwav_read_pcm_frames_be
DRWAV += drwav_read_pcm_frames_f32
DRWAV += drwav_read_pcm_frames_f32be
DRWAV += drwav_read_pcm_frames_f32le
DRWAV += drwav_read_pcm_frames_le
DRWAV += drwav_read_pcm_frames_s16
DRWAV += drwav_read_pcm_frames_s16be
DRWAV += drwav_read_pcm_frames_s16le
DRWAV += drwav_read_pcm_frames_s32
DRWAV += drwav_read_pcm_frames_s32be
DRWAV += drwav_read_pcm_frames_s32le
DRWAV += drwav_read_raw
DRWAV += drwav_read_s16
DRWAV += drwav_read_s16__alaw
DRWAV += drwav_read_s16__ieee
DRWAV += drwav_read_s16__ima
DRWAV += drwav_read_s16__msadpcm
DRWAV += drwav_read_s16__mulaw
DRWAV += drwav_read_s16__pcm
DRWAV += drwav_read_s32
DRWAV += drwav_read_s32__alaw
DRWAV += drwav_read_s32__alaw
DRWAV += drwav_read_s32__ieee
DRWAV += drwav_read_s32__ieee
DRWAV += drwav_read_s32__ima
DRWAV += drwav_read_s32__ima
DRWAV += drwav_read_s32__msadpcm
DRWAV += drwav_read_s32__msadpcm
DRWAV += drwav_read_s32__mulaw
DRWAV += drwav_read_s32__mulaw
DRWAV += drwav_read_s32__pcm
DRWAV += drwav_read_s32__pcm
DRWAV += drwav_riff_chunk_size_riff
DRWAV += drwav_riff_chunk_size_w64
DRWAV += drwav_s16_to_f32
DRWAV += drwav_s16_to_s32
DRWAV += drwav_s24_to_f32
DRWAV += drwav_s24_to_s16
DRWAV += drwav_s24_to_s16
DRWAV += drwav_s24_to_s32
DRWAV += drwav_s32_to_f32
DRWAV += drwav_s32_to_s16
DRWAV += drwav_s32_to_s16
DRWAV += drwav_seek_to_pcm_frame
DRWAV += drwav_seek_to_sample
DRWAV += drwav_seek_to_sample
DRWAV += drwav_smpl
DRWAV += drwav_smpl_loop
DRWAV += drwav_take_ownership_of_metadata
DRWAV += drwav_target_write_size_bytes
DRWAV += drwav_u8_to_f32
DRWAV += drwav_u8_to_s16
DRWAV += drwav_u8_to_s16
DRWAV += drwav_u8_to_s32
DRWAV += drwav_uninit
DRWAV += drwav_version
DRWAV += drwav_version_string
DRWAV += drwav_write
DRWAV += drwav_write
DRWAV += drwav_write_pcm_frames
DRWAV += drwav_write_pcm_frames_be
DRWAV += drwav_write_pcm_frames_le
DRWAV += drwav_write_raw

# --------------------------------------------------------------
# Files to build

PLUGIN_FILES = plugins.cpp
MINIPLUGIN_FILES = plugins-mini.cpp

# --------------------------------------------------------------
# Cardinal (built-in)

PLUGIN_FILES += Cardinal/src/AIDA-X.cpp
PLUGIN_FILES += Cardinal/src/Blank.cpp
PLUGIN_FILES += Cardinal/src/ExpanderInputMIDI.cpp
PLUGIN_FILES += Cardinal/src/ExpanderOutputMIDI.cpp
PLUGIN_FILES += Cardinal/src/HostAudio.cpp
PLUGIN_FILES += Cardinal/src/HostCV.cpp
PLUGIN_FILES += Cardinal/src/HostMIDI.cpp
PLUGIN_FILES += Cardinal/src/HostMIDI-CC.cpp
PLUGIN_FILES += Cardinal/src/HostMIDI-Gate.cpp
PLUGIN_FILES += Cardinal/src/HostMIDI-Map.cpp
PLUGIN_FILES += Cardinal/src/HostParameters.cpp
PLUGIN_FILES += Cardinal/src/HostParameters-Map.cpp
PLUGIN_FILES += Cardinal/src/HostTime.cpp
PLUGIN_FILES += Cardinal/src/TextEditor.cpp

MINIPLUGIN_FILES += Cardinal/src/HostAudio.cpp
MINIPLUGIN_FILES += Cardinal/src/HostCV.cpp
MINIPLUGIN_FILES += Cardinal/src/HostMIDI.cpp
MINIPLUGIN_FILES += Cardinal/src/HostMIDI-CC.cpp
MINIPLUGIN_FILES += Cardinal/src/HostMIDI-Gate.cpp
MINIPLUGIN_FILES += Cardinal/src/HostMIDI-Map.cpp
MINIPLUGIN_FILES += Cardinal/src/HostParameters.cpp
MINIPLUGIN_FILES += Cardinal/src/HostParameters-Map.cpp
MINIPLUGIN_FILES += Cardinal/src/HostTime.cpp
MINIPLUGIN_FILES += Cardinal/src/TextEditor.cpp

ifneq ($(USE_GLES2),true)
ifneq ($(USE_GLES3),true)
PLUGIN_FILES += Cardinal/src/glBars.cpp
endif
endif

ifneq ($(STATIC_BUILD),true)
PLUGIN_FILES += Cardinal/src/AudioFile.cpp
ifneq ($(WASM),true)
PLUGIN_FILES += Cardinal/src/Carla.cpp
PLUGIN_FILES += Cardinal/src/Ildaeil.cpp
endif
ifneq ($(HEADLESS),true)
ifeq ($(HAVE_X11),true)
PLUGIN_FILES += Cardinal/src/EmbedWidget.cpp
PLUGIN_FILES += Cardinal/src/MPV.cpp
endif
endif
endif

ifneq ($(HEADLESS),true)
PLUGIN_FILES += Cardinal/src/ImGuiWidget.cpp
PLUGIN_FILES += Cardinal/src/ImGuiTextEditor.cpp
PLUGIN_FILES += Cardinal/src/SassyScope.cpp
PLUGIN_FILES += Cardinal/src/DearImGui.cpp
PLUGIN_FILES += Cardinal/src/DearImGuiColorTextEditor.cpp
MINIPLUGIN_FILES += Cardinal/src/ImGuiWidget.cpp
MINIPLUGIN_FILES += Cardinal/src/ImGuiTextEditor.cpp
MINIPLUGIN_FILES += Cardinal/src/DearImGui.cpp
MINIPLUGIN_FILES += Cardinal/src/DearImGuiColorTextEditor.cpp
endif

ifeq ($(shell $(PKG_CONFIG) --exists fftw3f && echo true),true)
PLUGIN_FILES += Cardinal/src/AudioToCVPitch.cpp
# MINIPLUGIN_FILES += Cardinal/src/AudioToCVPitch.cpp
endif

# --------------------------------------------------------------
# Fundamental (always enabled)

PLUGIN_FILES += $(filter-out Fundamental/src/plugin.cpp,$(wildcard Fundamental/src/*.cpp))
PLUGIN_FILES += Fundamental/src/dr_wav.c

MINIPLUGIN_FILES += Fundamental/src/ADSR.cpp
MINIPLUGIN_FILES += Fundamental/src/LFO.cpp
MINIPLUGIN_FILES += Fundamental/src/Merge.cpp
MINIPLUGIN_FILES += Fundamental/src/MidSide.cpp
MINIPLUGIN_FILES += Fundamental/src/Noise.cpp
MINIPLUGIN_FILES += Fundamental/src/Quantizer.cpp
MINIPLUGIN_FILES += Fundamental/src/Random.cpp
MINIPLUGIN_FILES += Fundamental/src/Scope.cpp
MINIPLUGIN_FILES += Fundamental/src/Split.cpp
MINIPLUGIN_FILES += Fundamental/src/Sum.cpp
MINIPLUGIN_FILES += Fundamental/src/VCA-1.cpp
MINIPLUGIN_FILES += Fundamental/src/VCF.cpp
MINIPLUGIN_FILES += Fundamental/src/VCMixer.cpp
MINIPLUGIN_FILES += Fundamental/src/VCO.cpp

# modules/types which are present in other plugins
FUNDAMENTAL_CUSTOM = $(DRWAV)

# --------------------------------------------------------------
# ZamAudio (always enabled) - TODO

# PLUGIN_FILES += $(wildcard ZamAudio/src/*.cpp)

# --------------------------------------------------------------
# 21kHz

PLUGIN_FILES += $(filter-out 21kHz/src/21kHz.cpp,$(wildcard 21kHz/src/*.cpp))

# --------------------------------------------------------------
# 8Mode

PLUGIN_FILES += $(filter-out 8Mode/src/8mode.cpp,$(wildcard 8Mode/src/*.cpp))

# --------------------------------------------------------------
# alefsbits

PLUGIN_FILES += $(filter-out alefsbits/src/plugin.cpp,$(wildcard alefsbits/src/*.cpp))

# modules/types which are present in other plugins
ALEFSBITS_CUSTOM = Steps Logic

# --------------------------------------------------------------
# AlgoritmArte

PLUGIN_FILES += $(filter-out Algoritmarte/src/plugin.cpp,$(wildcard Algoritmarte/src/*.cpp))

# --------------------------------------------------------------
# Aaron Static

PLUGIN_FILES += $(wildcard AaronStatic/src/*.cpp)

# modules/types which are present in other plugins
AARONSTATIC_CUSTOM = RefreshCounter

# --------------------------------------------------------------
# AmalgamatedHarmonics

PLUGIN_FILES += $(filter-out AmalgamatedHarmonics/src/AH.cpp,$(wildcard AmalgamatedHarmonics/src/*.cpp))

# modules/types which are present in other plugins
AMALGAMATEDHARMONICS_CUSTOM = BasePinkNoiseGenerator EvenVCO LowFrequencyOscillator Pattern PinkNoiseGenerator RedNoiseGenerator
AMALGAMATEDHARMONICS_CUSTOM += bogaudio

# --------------------------------------------------------------
# AnimatedCircuits

PLUGIN_FILES += $(wildcard AnimatedCircuits/src/Folding/*.cpp)
PLUGIN_FILES += $(wildcard AnimatedCircuits/src/LFold/*.cpp)

# --------------------------------------------------------------
# ArableInstruments

PLUGIN_FILES += ArableInstruments/src/Clouds.cpp
PLUGIN_FILES += ArableInstruments/eurorack/clouds/dsp/correlator.cc
PLUGIN_FILES += ArableInstruments/eurorack/clouds/dsp/granular_processor.cc
PLUGIN_FILES += ArableInstruments/eurorack/clouds/dsp/mu_law.cc
PLUGIN_FILES += ArableInstruments/eurorack/clouds/dsp/pvoc/frame_transformation.cc
PLUGIN_FILES += ArableInstruments/eurorack/clouds/dsp/pvoc/phase_vocoder.cc
PLUGIN_FILES += ArableInstruments/eurorack/clouds/dsp/pvoc/stft.cc
PLUGIN_FILES += ArableInstruments/eurorack/clouds/resources.cc
PLUGIN_FILES += ArableInstruments/eurorack/stmlib/utils/random.cc
PLUGIN_FILES += ArableInstruments/eurorack/stmlib/dsp/atan.cc
PLUGIN_FILES += ArableInstruments/eurorack/stmlib/dsp/units.cc

# modules/types which are present in other plugins
ARABLE_CUSTOM = Clouds FreezeLight clouds stmlib

# --------------------------------------------------------------
# Aria

PLUGIN_FILES += AriaModules/src/Blank.cpp
PLUGIN_FILES += AriaModules/src/Darius.cpp
PLUGIN_FILES += AriaModules/src/Pokies.cpp
PLUGIN_FILES += AriaModules/src/Psychopump.cpp
PLUGIN_FILES += AriaModules/src/Qqqq.cpp
PLUGIN_FILES += AriaModules/src/Quale.cpp
PLUGIN_FILES += AriaModules/src/Rotatoes.cpp
PLUGIN_FILES += AriaModules/src/Smerge.cpp
PLUGIN_FILES += AriaModules/src/Solomon.cpp
PLUGIN_FILES += AriaModules/src/Spleet.cpp
PLUGIN_FILES += AriaModules/src/Splirge.cpp
PLUGIN_FILES += AriaModules/src/Splort.cpp
PLUGIN_FILES += AriaModules/src/Swerge.cpp
PLUGIN_FILES += AriaModules/src/Undular.cpp

MINIPLUGIN_FILES += AriaModules/src/Spleet.cpp
MINIPLUGIN_FILES += AriaModules/src/Swerge.cpp

# modules/types which are present in other plugins
ARIA_CUSTOM = Blank

# --------------------------------------------------------------
# AS

PLUGIN_FILES += $(filter-out AS/src/AS.cpp,$(wildcard AS/src/*.cpp))
PLUGIN_FILES += AS/freeverb/revmodel.cpp

# modules/types which are present in other plugins
AS_CUSTOM = ADSR BpmDisplayWidget LabelDisplayWidget LedLight LowFrequencyOscillator SigDisplayWidget VCA WaveShaper YellowRedLight allpass comb revmodel
AS_CUSTOM_PER_FILE = NumberDisplayWidget

# --------------------------------------------------------------
# AudibleInstruments

PLUGIN_FILES += $(filter-out AudibleInstruments/src/plugin.cpp,$(wildcard AudibleInstruments/src/*.cpp))

PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/utils/random.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/atan.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/units.cc

PLUGIN_FILES += AudibleInstruments/eurorack/braids/macro_oscillator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/braids/analog_oscillator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/braids/digital_oscillator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/braids/quantizer.cc
PLUGIN_FILES += AudibleInstruments/eurorack/braids/resources.cc

PLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/*.cc)
PLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/engine/*.cc)
PLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/speech/*.cc)
PLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/physical_modelling/*.cc)
PLUGIN_FILES += AudibleInstruments/eurorack/plaits/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/correlator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/granular_processor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/mu_law.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/pvoc/frame_transformation.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/pvoc/phase_vocoder.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/pvoc/stft.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/exciter.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/ominous_voice.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/resonator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/tube.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/multistage_envelope.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/part.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/string.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/voice.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/fm_voice.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/part.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/string_synth_part.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/string.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/resonator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/tides/generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/tides/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/tides2/poly_slope_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/tides2/ramp_extractor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/tides2/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/warps/dsp/modulator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/warps/dsp/oscillator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/warps/dsp/vocoder.cc
PLUGIN_FILES += AudibleInstruments/eurorack/warps/dsp/filter_bank.cc
PLUGIN_FILES += AudibleInstruments/eurorack/warps/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/frames/keyframer.cc
PLUGIN_FILES += AudibleInstruments/eurorack/frames/resources.cc
PLUGIN_FILES += AudibleInstruments/eurorack/frames/poly_lfo.cc

PLUGIN_FILES += AudibleInstruments/eurorack/peaks/processors.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/resources.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/drums/bass_drum.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/drums/fm_drum.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/drums/high_hat.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/drums/snare_drum.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/modulations/lfo.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/modulations/multistage_envelope.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/pulse_processor/pulse_shaper.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/pulse_processor/pulse_randomizer.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/number_station/number_station.cc

PLUGIN_FILES += AudibleInstruments/eurorack/stages/segment_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stages/ramp_extractor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stages/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/utils/random.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/atan.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/units.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/t_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/x_y_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/output_channel.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/lag_processor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/quantizer.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/ramp/ramp_extractor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/streams/resources.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/processor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/follower.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/lorenz_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/envelope.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/svf.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/vactrol.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/compressor.cc

MINIPLUGIN_FILES += AudibleInstruments/src/Plaits.cpp
# MINIPLUGIN_FILES += AudibleInstruments/eurorack/braids/macro_oscillator.cc
# MINIPLUGIN_FILES += AudibleInstruments/eurorack/braids/analog_oscillator.cc
# MINIPLUGIN_FILES += AudibleInstruments/eurorack/braids/digital_oscillator.cc
# MINIPLUGIN_FILES += AudibleInstruments/eurorack/braids/quantizer.cc
# MINIPLUGIN_FILES += AudibleInstruments/eurorack/braids/resources.cc
MINIPLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/*.cc)
MINIPLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/engine/*.cc)
MINIPLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/speech/*.cc)
MINIPLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/physical_modelling/*.cc)
MINIPLUGIN_FILES += AudibleInstruments/eurorack/plaits/resources.cc
MINIPLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/atan.cc
MINIPLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/units.cc
MINIPLUGIN_FILES += AudibleInstruments/eurorack/stmlib/utils/random.cc

# --------------------------------------------------------------
# Autinn

PLUGIN_FILES += $(wildcard Autinn/src/*.cpp)

AUTINN_CUSTOM = Chord Vibrato

# --------------------------------------------------------------
# Axioma

PLUGIN_FILES += $(filter-out Axioma/src/plugin.cpp,$(wildcard Axioma/src/*.cpp))

# --------------------------------------------------------------
# BaconPlugs

PLUGIN_FILES += $(filter-out BaconPlugs/src/BaconPlugs.cpp,$(wildcard BaconPlugs/src/*.cpp))
PLUGIN_FILES += $(wildcard BaconPlugs/libs/midifile/src/*.cpp)
PLUGIN_FILES += $(wildcard BaconPlugs/libs/open303-code/Source/DSPCode/*.cpp)

MINIPLUGIN_FILES += BaconPlugs/src/Style.cpp

# --------------------------------------------------------------
# Befaco

PLUGIN_FILES += $(filter-out Befaco/src/plugin.cpp,$(wildcard Befaco/src/*.cpp))
PLUGIN_FILES += $(wildcard Befaco/src/noise-plethora/*/*.cpp)

# modules/types which are present in other plugins
BEFACO_CUSTOM = ADSR Mixer chowdsp Burst

# --------------------------------------------------------------
# Bidoo

PLUGIN_FILES += $(filter-out Bidoo/src/plugin.cpp Bidoo/src/ANTN.cpp,$(wildcard Bidoo/src/*.cpp))
PLUGIN_FILES += $(wildcard Bidoo/src/dep/*.cpp)
PLUGIN_FILES += $(wildcard Bidoo/src/dep/filters/*.cpp)
PLUGIN_FILES += $(wildcard Bidoo/src/dep/freeverb/*.cpp)
PLUGIN_FILES += $(filter-out Bidoo/src/dep/lodepng/pngdetail.cpp,$(wildcard Bidoo/src/dep/lodepng/*.cpp))
PLUGIN_FILES += $(filter-out Bidoo/src/dep/resampler/main.cpp,$(wildcard Bidoo/src/dep/resampler/*.cpp))
PLUGIN_FILES += BidooDark/plugin.cpp

# modules/types which are present in other plugins
BIDOO_CUSTOM = ChannelDisplay InstantiateExpanderItem LadderFilter PitchShifter $(DRWAV)
BIDOO_CUSTOM_PER_FILE = channel channel filterType

# --------------------------------------------------------------
# Biset

PLUGIN_FILES += $(filter-out Biset/src/plugin.cpp,$(wildcard Biset/src/*.cpp))
PLUGIN_FILES += $(wildcard Biset/src/*/*.cpp)

# modules/types which are present in other plugins
BISET_CUSTOM = Blank ButtonTrigger Clock Tree TreeDisplay

# --------------------------------------------------------------
# BogaudioModules

PLUGIN_FILES += $(filter-out BogaudioModules/src/bogaudio.cpp,$(wildcard BogaudioModules/src/*.cpp))
PLUGIN_FILES += $(wildcard BogaudioModules/src/dsp/*.cpp)
PLUGIN_FILES += $(wildcard BogaudioModules/src/dsp/filters/*.cpp)

MINIPLUGIN_FILES += BogaudioModules/src/AD.cpp
MINIPLUGIN_FILES += BogaudioModules/src/LFO.cpp
MINIPLUGIN_FILES += BogaudioModules/src/Noise.cpp
MINIPLUGIN_FILES += BogaudioModules/src/Offset.cpp
MINIPLUGIN_FILES += BogaudioModules/src/SampleHold.cpp
MINIPLUGIN_FILES += BogaudioModules/src/Switch.cpp
MINIPLUGIN_FILES += BogaudioModules/src/Switch18.cpp
MINIPLUGIN_FILES += BogaudioModules/src/Unison.cpp
MINIPLUGIN_FILES += BogaudioModules/src/VCA.cpp
MINIPLUGIN_FILES += BogaudioModules/src/VCF.cpp
MINIPLUGIN_FILES += BogaudioModules/src/VCO.cpp
MINIPLUGIN_FILES += BogaudioModules/src/disable_output_limit.cpp
MINIPLUGIN_FILES += BogaudioModules/src/lfo_base.cpp
MINIPLUGIN_FILES += BogaudioModules/src/matrix_base.cpp
MINIPLUGIN_FILES += BogaudioModules/src/menu.cpp
MINIPLUGIN_FILES += BogaudioModules/src/module.cpp
MINIPLUGIN_FILES += BogaudioModules/src/param_quantities.cpp
MINIPLUGIN_FILES += BogaudioModules/src/poly_channels.cpp
MINIPLUGIN_FILES += BogaudioModules/src/rack_overrides.cpp
MINIPLUGIN_FILES += BogaudioModules/src/save_latch_to_patch.cpp
MINIPLUGIN_FILES += BogaudioModules/src/skins.cpp
MINIPLUGIN_FILES += BogaudioModules/src/utils.cpp
MINIPLUGIN_FILES += BogaudioModules/src/vco_base.cpp
MINIPLUGIN_FILES += BogaudioModules/src/widgets.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/analyzer.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/envelope.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/math.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/noise.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/oscillator.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/signal.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/table.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/filters/experiments.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/filters/filter.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/filters/multimode.cpp
MINIPLUGIN_FILES += BogaudioModules/src/dsp/filters/resample.cpp

# modules/types which are present in other plugins
BOGAUDIO_CUSTOM = ADSR BlueNoiseGenerator LFO Noise VCA VCO VCF
BOGAUDIO_CUSTOM_PER_FILE = ARQuantity AttackMenuItem ReleaseMenuItem

# --------------------------------------------------------------
# ChowDSP

PLUGIN_FILES += $(wildcard ChowDSP/src/*/*.cpp)
PLUGIN_FILES += $(wildcard ChowDSP/src/*/*/*.cpp)
PLUGIN_FILES += $(wildcard ChowDSP/lib/r8lib/*.cpp)

# modules/types which are present in other plugins
CHOWDSP_CUSTOM = sign

# --------------------------------------------------------------
# CatroModulo

PLUGIN_FILES += $(filter-out CatroModulo/src/CatroModulo.cpp,$(wildcard CatroModulo/src/*.cpp))

# modules/types which are present in other plugins
CATROMODULO_CUSTOM = LowFrequencyOscillator NumDisplayWidget

# --------------------------------------------------------------
# cf

PLUGIN_FILES += $(filter-out cf/src/plugin.cpp,$(wildcard cf/src/*.cpp))

# modules/types which are present in other plugins
CF_CUSTOM = $(DRWAV)

# --------------------------------------------------------------
# Computerscare

PLUGIN_FILES += $(filter-out Computerscare/src/Computerscare.cpp Computerscare/src/test.cpp,$(wildcard Computerscare/src/*.cpp))

# modules/types which are present in other plugins
COMPUTERSCARE_CUSTOM = Quantizer PanelBorder

# --------------------------------------------------------------
# CVfunk

PLUGIN_FILES += $(filter-out CVfunk/src/plugin.cpp,$(wildcard CVfunk/src/*.cpp))

# modules/types which are present in other plugins
CVFUNK_CUSTOM = Steps

# --------------------------------------------------------------
# dBiz

PLUGIN_FILES += $(filter-out dBiz/src/plugin.cpp,$(wildcard dBiz/src/*.cpp))

# modules/types which are present in other plugins
DBIZ_CUSTOM  = Chord DarkDefaultItem Divider FourSeq FourSeqWidget OrangeLight VCA4 Wavefolder darkPanelID lightPanelID
DBIZ_CUSTOM_PER_FILE = LERP MultiFilter Oscillator calcOutput sineOsc subBank

# --------------------------------------------------------------
# DHE-Modules

PLUGIN_FILES += $(filter-out DHE-Modules/src/plugin.cpp,$(wildcard DHE-Modules/src/modules/*/init.cpp))

# --------------------------------------------------------------
# DrumKit

PLUGIN_FILES += $(wildcard DrumKit/src/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/src/controller/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/src/view/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/src/model/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/deps/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/deps/SynthDevKit/src/*.cpp)

# modules/types which are present in other plugins
DRUMKIT_CUSTOM = ADSR Envelope LowFrequencyOscillator

# --------------------------------------------------------------
# EnigmaCurry

PLUGIN_FILES += $(filter-out EnigmaCurry/src/plugin.cpp,$(wildcard EnigmaCurry/src/*.cpp))
# modules/types which are present in other plugins
ENIGMACURRY_CUSTOM = Pulse

# --------------------------------------------------------------
# ESeries

PLUGIN_FILES += ESeries/src/E340.cpp

# --------------------------------------------------------------
# ExpertSleepers-Encoders

PLUGIN_FILES += $(filter-out ExpertSleepers-Encoders/src/Encoders.cpp,$(wildcard ExpertSleepers-Encoders/src/*.cpp))

# --------------------------------------------------------------
# Extratone

PLUGIN_FILES += $(filter-out Extratone/src/plugin.cpp,$(wildcard Extratone/src/*.cpp))

# --------------------------------------------------------------
# FehlerFabrik

PLUGIN_FILES += $(filter-out FehlerFabrik/src/plugin.cpp,$(wildcard FehlerFabrik/src/*.cpp))

# modules/types which are present in other plugins
FEHLERFABRIK_CUSTOM = Operator Sequencer SlewLimiter SimpleDelay

# --------------------------------------------------------------
# forsitan modulare

PLUGIN_FILES += $(filter-out forsitan-modulare/src/forsitan.cpp,$(wildcard forsitan-modulare/src/*.cpp))

# --------------------------------------------------------------
# GlueTheGiant

PLUGIN_FILES += $(filter-out GlueTheGiant/src/plugin.cpp,$(wildcard GlueTheGiant/src/*.cpp))

# --------------------------------------------------------------
# GoodSheperd

PLUGIN_FILES += $(filter-out GoodSheperd/src/plugin.cpp,$(wildcard GoodSheperd/src/*.cpp))

# --------------------------------------------------------------
# GrandeModular

PLUGIN_FILES += $(filter-out GrandeModular/src/plugin.cpp,$(wildcard GrandeModular/src/*.cpp))

# --------------------------------------------------------------
# H4N4 Modules

PLUGIN_FILES += $(filter-out h4n4-modules/src/plugin.cpp,$(wildcard h4n4-modules/src/*.cpp))

# --------------------------------------------------------------
# Hampton Harmonics

PLUGIN_FILES += $(filter-out HamptonHarmonics/src/plugin.cpp,$(wildcard HamptonHarmonics/src/*.cpp))

# modules/types which are present in other plugins
HAMPTONHARMONICS_CUSTOM = Arp Progress

# --------------------------------------------------------------
# HetrickCV

PLUGIN_FILES += $(filter-out HetrickCV/src/HetrickCV.cpp,$(wildcard HetrickCV/src/*.cpp))
PLUGIN_FILES += $(wildcard HetrickCV/src/DSP/*.cpp)
PLUGIN_FILES += $(wildcard HetrickCV/src/DSP/Phasors/*.cpp)
PLUGIN_FILES += $(wildcard HetrickCV/Gamma/src/arr.cpp)
PLUGIN_FILES += $(wildcard HetrickCV/Gamma/src/Domain.cpp)
PLUGIN_FILES += $(wildcard HetrickCV/Gamma/src/scl.cpp)

# modules/types which are present in other plugins
HETRICKCV_CUSTOM = ASR BlankPanel FlipFlop MidSide MinMax PanelBaseWidget InverterWidget

# --------------------------------------------------------------
# ImpromptuModular

PLUGIN_FILES += $(wildcard ImpromptuModular/src/*.cpp)
PLUGIN_FILES += $(filter-out ImpromptuModular/src/comp/PanelTheme.cpp,$(wildcard ImpromptuModular/src/comp/*.cpp))
PLUGIN_FILES += ImpromptuModularDark/PanelTheme.cpp

# modules/types which are present in other plugins
IMPROMPTUMODULAR_CUSTOM = RefreshCounter
IMPROMPTUMODULAR_CUSTOM_PER_FILE = Clock stepClock

# --------------------------------------------------------------
# ihtsyn

PLUGIN_FILES += $(filter-out ihtsyn/src/plugin.cpp,$(wildcard ihtsyn/src/*.cpp))

# modules/types which are present in other plugins
IHTSYN_CUSTOM_PER_FILE  = mv_allpass
IHTSYN_CUSTOM_PER_FILE += mv_staticallpass4tap
IHTSYN_CUSTOM_PER_FILE += mv_staticdelayline
IHTSYN_CUSTOM_PER_FILE += mv_staticdelayline4tap
IHTSYN_CUSTOM_PER_FILE += mv_staticdelayline8tap
IHTSYN_CUSTOM_PER_FILE += mv_statevariable

# --------------------------------------------------------------
# JW-Modules

PLUGIN_FILES += $(filter-out JW-Modules/src/JWModules.cpp JW-Modules/src/Str1ker.cpp,$(wildcard JW-Modules/src/*.cpp))

ifneq ($(STATIC_BUILD),true)
PLUGIN_FILES += JW-Modules/src/Str1ker.cpp
PLUGIN_FILES += $(wildcard JW-Modules/lib/oscpack/ip/*.cpp)
PLUGIN_FILES += $(wildcard JW-Modules/lib/oscpack/osc/*.cpp)
ifeq ($(WINDOWS),true)
PLUGIN_FILES += $(wildcard JW-Modules/lib/oscpack/ip/win32/*.cpp)
else
PLUGIN_FILES += $(wildcard JW-Modules/lib/oscpack/ip/posix/*.cpp)
endif
endif

# modules/types which are present in other plugins
JW_CUSTOM = PlayHead Quantizer

# --------------------------------------------------------------
# kocmoc

PLUGIN_FILES += $(filter-out kocmoc/src/plugin.cpp,$(wildcard kocmoc/src/*.cpp))

# modules/types which are present in other plugins
KOCMOC_CUSTOM = Phasor __ct_base __ct_comp

# --------------------------------------------------------------
# LifeFormModular

PLUGIN_FILES += $(filter-out LifeFormModular/src/plugin.cpp,$(wildcard LifeFormModular/src/*.cpp))

# modules/types which are present in other plugins
LIFEFORMMODULAR_CUSTOM = IO MS __ct_base __ct_comp

# --------------------------------------------------------------
# Lilac Loop

PLUGIN_FILES += $(filter-out LilacLoop/src/plugin.cpp,$(wildcard LilacLoop/src/*.cpp))

# modules/types which are present in other plugins
LILACLOOP_CUSTOM = AudioFile Mode

# --------------------------------------------------------------
# LittleUtils

PLUGIN_FILES += $(filter-out LittleUtils/src/plugin.cpp,$(wildcard LittleUtils/src/*.cpp))

# modules/types which are present in other plugins
LITTLEUTILS_CUSTOM = MsDisplayWidget MAX_POLY_CHANNELS

# --------------------------------------------------------------
# LomasModules

PLUGIN_FILES += $(filter-out LomasModules/src/plugin.cpp,$(wildcard LomasModules/src/*.cpp))

# modules/types which are present in other plugins
LOMAS_CUSTOM = $(DRWAV)

# --------------------------------------------------------------
# LyraeModules

PLUGIN_FILES += $(filter-out LyraeModules/src/plugin.cpp,$(wildcard LyraeModules/src/*.cpp))

# modules/types which are present in other plugins
LYRAE_CUSTOM = Delta

# --------------------------------------------------------------
# Meander

PLUGIN_FILES += $(filter-out Meander/src/plugin.cpp,$(wildcard Meander/src/*.cpp))

# --------------------------------------------------------------
# MindMeld

PLUGIN_FILES += $(wildcard MindMeldModular/src/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/comp/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/EqMaster/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/MixMaster/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/PatchSet/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/ShapeMaster/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/Utilities/*.cpp)

# modules/types which are present in other plugins
MINDMELD_CUSTOM = Trigger printNote

# --------------------------------------------------------------
# ML_modules

PLUGIN_FILES += $(filter-out ML_modules/src/ML_modules.cpp,$(wildcard ML_modules/src/*.cpp))
PLUGIN_FILES += ML_modules/freeverb/revmodel.cpp

# modules/types which are present in other plugins
ML_CUSTOM = Mode Quant Quantizer QuantizerWidget SH8 allpass comb revmodel

# --------------------------------------------------------------
# MockbaModular

PLUGIN_FILES += $(filter-out MockbaModular/src/plugin.cpp MockbaModular/src/MockbaModular.cpp MockbaModular/src/UDPClockMaster.cpp MockbaModular/src/UDPClockSlave.cpp,$(wildcard MockbaModular/src/*.cpp))

# modules/types which are present in other plugins
MOCKBAMODULAR_CUSTOM = Blank Comparator

MINIPLUGIN_FILES += MockbaModular/src/CZOsc.cpp
MINIPLUGIN_FILES += MockbaModular/src/Filtah.cpp
MINIPLUGIN_FILES += MockbaModular/src/MaugOsc.cpp
MINIPLUGIN_FILES += MockbaModular/src/Mixah.cpp
MINIPLUGIN_FILES += MockbaModular/src/Pannah.cpp
MINIPLUGIN_FILES += MockbaModular/src/ReVoltah.cpp
MINIPLUGIN_FILES += MockbaModular/src/Shapah.cpp

# --------------------------------------------------------------
# Mog

PLUGIN_FILES += Mog/src/Network.cpp
PLUGIN_FILES += Mog/src/Nexus.cpp

# --------------------------------------------------------------
# mscHack

PLUGIN_FILES += $(wildcard mscHack/src/*.cpp)

# modules/types which are present in other plugins
MSCHACK_CUSTOM_PER_FILE = MAIN_SYNC_CLOCK FILTER_STRUCT FILTER_PARAM_STRUCT OSC_PARAM_STRUCT PHRASE_CHANGE_STRUCT

# --------------------------------------------------------------
# MSM

PLUGIN_FILES += $(filter-out MSM/src/MSM.cpp,$(wildcard MSM/src/*.cpp))

# modules/types which are present in other plugins
MSM_CUSTOM = ADSR BlankPanel Delay LFO LowFrequencyOscillator Mult Noise OP VCA VCO sawTable triTable

# --------------------------------------------------------------
# myth-modules

PLUGIN_FILES += $(filter-out myth-modules/src/plugin.cpp,$(wildcard myth-modules/src/*.cpp))

# --------------------------------------------------------------
# Nonlinear Circuits

PLUGIN_FILES += $(filter-out nonlinearcircuits/src/NLC.cpp,$(wildcard nonlinearcircuits/src/*.cpp))

# --------------------------------------------------------------
# Orbits

PLUGIN_FILES += $(filter-out Orbits/src/plugin.cpp,$(wildcard Orbits/src/*.cpp))

# --------------------------------------------------------------
# ParableInstruments

PLUGIN_FILES += ParableInstruments/src/Clouds.cpp
PLUGIN_FILES += ParableInstruments/parasites/clouds/dsp/correlator.cc
PLUGIN_FILES += ParableInstruments/parasites/clouds/dsp/granular_processor.cc
PLUGIN_FILES += ParableInstruments/parasites/clouds/dsp/mu_law.cc
PLUGIN_FILES += ParableInstruments/parasites/clouds/dsp/pvoc/frame_transformation.cc
PLUGIN_FILES += ParableInstruments/parasites/clouds/dsp/pvoc/phase_vocoder.cc
PLUGIN_FILES += ParableInstruments/parasites/clouds/dsp/pvoc/stft.cc
PLUGIN_FILES += ParableInstruments/parasites/clouds/resources.cc
PLUGIN_FILES += ParableInstruments/parasites/stmlib/utils/random.cc
PLUGIN_FILES += ParableInstruments/parasites/stmlib/dsp/atan.cc
PLUGIN_FILES += ParableInstruments/parasites/stmlib/dsp/units.cc

# modules/types which are present in other plugins
PARABLE_CUSTOM = Clouds CustomPanel CloudsWidget FreezeLight clouds stmlib

# --------------------------------------------------------------
# Path Set

PLUGIN_FILES += $(filter-out PathSet/src/plugin.cpp,$(wildcard PathSet/src/*.cpp))

# modules/types which are present in other plugins
PATHSET_CUSTOM = PitchShifter

# --------------------------------------------------------------
# PdArray
PLUGIN_FILES += $(filter-out PdArray/src/plugin.cpp,$(wildcard PdArray/src/*.cpp))

# modules/types which are present in other plugins
PDARRAY_CUSTOM = MAX_POLY_CHANNELS draw

# --------------------------------------------------------------
# PinkTrombone

PLUGIN_FILES += $(filter-out PinkTrombone/src/plugin.cpp,$(wildcard PinkTrombone/src/*.cpp))
PLUGIN_FILES += $(wildcard PinkTrombone/src/PinkTrombone/*.cpp)

# modules/types which are present in other plugins
PINKTROMBONE_CUSTOM = WhiteNoise

# --------------------------------------------------------------
# Prism

PLUGIN_FILES += $(filter-out Prism/src/plugin.cpp,$(wildcard Prism/src/*.cpp))
PLUGIN_FILES += $(wildcard Prism/src/scales/*.cpp)

# modules/types which are present in other plugins
PRISM_CUSTOM = bogaudio Scale

# --------------------------------------------------------------
# rackwindows

PLUGIN_FILES += $(filter-out rackwindows/src/plugin.cpp,$(wildcard rackwindows/src/*.cpp))

# --------------------------------------------------------------
# RebelTech

PLUGIN_FILES += $(filter-out RebelTech/src/plugin.cpp,$(wildcard RebelTech/src/*.cpp))

# modules/types which are present in other plugins
REBELTECH_CUSTOM = BefacoInputPort BefacoOutputPort

# --------------------------------------------------------------
# repelzen

PLUGIN_FILES += $(filter-out repelzen/src/repelzen.cpp,$(wildcard repelzen/src/*.cpp))

# modules/types which are present in other plugins
REPELZEN_CUSTOM = Blank Mixer Werner tanh_pade

# --------------------------------------------------------------
# Sapphire

PLUGIN_FILES += $(filter-out Sapphire/src/plugin.cpp,$(wildcard Sapphire/src/*.cpp))

# --------------------------------------------------------------
# sonusmodular

PLUGIN_FILES += $(filter-out sonusmodular/src/sonusmodular.cpp,$(wildcard sonusmodular/src/*.cpp))

# --------------------------------------------------------------
# Starling Via

PLUGIN_FILES += $(wildcard StarlingVia/src/*.cpp)
PLUGIN_FILES += $(wildcard StarlingVia/Via/io/src/*.cpp)
PLUGIN_FILES += $(wildcard StarlingVia/Via/ui/src/*.cpp)
PLUGIN_FILES += $(wildcard StarlingVia/Via/modules/*/*.cpp)
PLUGIN_BINARIES += StarlingVia/res/original.gateseq
PLUGIN_BINARIES += StarlingVia/res/original.meta
PLUGIN_BINARIES += StarlingVia/res/original.osc3
PLUGIN_BINARIES += StarlingVia/res/original.scanner
PLUGIN_BINARIES += StarlingVia/res/original.sync
PLUGIN_BINARIES += StarlingVia/res/original.sync3

# modules/types which are present in other plugins
STARLINGVIA_CUSTOM = Scanner Scale Wavetable

# --------------------------------------------------------------
# stocaudio

PLUGIN_FILES += $(filter-out stocaudio/src/plugin.cpp,$(wildcard stocaudio/src/*.cpp))

# --------------------------------------------------------------
# stoermelder-packone

STOERMELDER_PACKONE_IGNORED = pluginsettings

# will never work
STOERMELDER_PACKONE_IGNORED += AudioInterface64

# maybe?
STOERMELDER_PACKONE_IGNORED += MidiCat MidiCatCtx MidiCatMem MidiKey MidiMon MidiPlug MidiStep

PLUGIN_FILES += $(filter-out $(STOERMELDER_PACKONE_IGNORED:%=stoermelder-packone/src/%.cpp),$(wildcard stoermelder-packone/src/*.cpp stoermelder-packone/src/mb/*.cpp))

# modules/types which are present in other plugins
STOERMELDER_PACKONE_CUSTOM = LongPressButton
STOERMELDER_PACKONE_CUSTOM_PER_FILE = InputLedDisplay IntermixEnvModule

# --------------------------------------------------------------
# surgext

PLUGIN_FILES += $(filter-out surgext/src/SurgeXT.cpp,$(wildcard surgext/src/*.cpp))
PLUGIN_FILES += surgext-helper/surgext-helper.cpp

# modules/types which are present in other plugins
SURGEXT_CUSTOM = HysteresisProcessing Patch SolverType Tunings Wavetable ghc clouds plaits stmlib

SURGEXT_FLAGS  = $(filter-out -fsingle-precision-constant,$(filter-out -std=gnu++11,$(BUILD_CXX_FLAGS)))
SURGEXT_FLAGS += -std=gnu++17
SURGEXT_FLAGS += -DTIXML_USE_STL=1
SURGEXT_FLAGS += \
	-Isurgext/libs/sst-rackhelpers/include \
	-Isurgext/surge/src/common \
	-Isurgext/surge/src/common/dsp \
	-Isurgext/surge/src/common/dsp/filters \
	-Isurgext/surge/src/common/dsp/vembertech \
	-Isurgext/surge/src/common/dsp/utilities \
	-Isurgext/surge/src/common/dsp/oscillators \
	-Isurgext/surge/src/common/dsp/modulators \
	-Isurgext/surge/src/surge-testrunner \
	-Isurgext/surge/libs/fmt/include \
	-Isurgext/surge/libs/LuaJitLib/LuaJIT/src \
	-Isurgext/surge/libs/sst/sst-basic-blocks/include \
	-Isurgext/surge/libs/sst/sst-cpputils/include \
	-Isurgext/surge/libs/sst/sst-effects/include \
	-Isurgext/surge/libs/sst/sst-filters/include \
	-Isurgext/surge/libs/sst/sst-plugininfra/include \
	-Isurgext/surge/libs/sst/sst-plugininfra/libs/tinyxml/include \
	-Isurgext/surge/libs/sst/sst-plugininfra/libs/filesystem \
	-Isurgext/surge/libs/sst/sst-waveshapers/include \
	-Isurgext/surge/libs/strnatcmp \
	-Isurgext/surge/src/headless \
	-Isurgext/surge/libs/tuning-library/include \
	-I../deps/surge-build/libs/sst/sst-plugininfra/libs/filesystem/include \
	-DSURGE_COMPILE_BLOCK_SIZE=8 \
	-DSURGE_SKIP_ODDSOUND_MTS \
	-DHAS_LUA=0 \
	-DHAS_JUCE=0 \
	-DSKIP_JUCE=1

ifneq ($(DEBUG),true)
SURGEXT_FLAGS += -DRELEASE=1
endif

ifeq ($(MACOS),true)
SURGEXT_FLAGS += -Wno-undefined-bool-conversion -Wno-unused-variable -Wno-reorder -Wno-char-subscripts -Wno-sign-compare -Wno-ignored-qualifiers -Wno-c++17-extensions -Wno-unused-private-field
SURGEXT_FLAGS += -DMAC
else ifeq ($(WINDOWS),true)
SURGEXT_FLAGS += -Wno-suggest-override -Wno-sign-compare -Wno-ignored-qualifiers -Wno-unused-variable -Wno-char-subscripts -Wno-reorder -Wno-int-in-bool-context
SURGEXT_FLAGS += -DWINDOWS
SURGEXT_FLAGS += -DSKIP_MINGW_FORMAT
else
SURGEXT_FLAGS += -Wno-unused-value -Wno-suggest-override -Wno-implicit-fallthrough -Wno-ignored-qualifiers
SURGEXT_FLAGS += -Wno-nonnull-compare -Wno-sign-compare -Wno-char-subscripts -Wno-unused-variable -Wno-unused-but-set-variable -Wno-reorder -Wno-multichar
SURGEXT_FLAGS += -DLINUX
SURGEXT_FLAGS += -Isurge/src/linux
endif

SURGEXT_FLAGS += -DSURGE_RACK_BASE_VERSION=XT1-0-1
SURGEXT_FLAGS += -DSURGE_RACK_PLUG_VERSION=Cardinal
SURGEXT_FLAGS += -DSURGE_RACK_SURGE_VERSION=Cardinal

SURGEXT_FLAGS += -I$(abspath ../carla/source/modules)

MINIPLUGIN_FILES += surgext/src/Delay.cpp
MINIPLUGIN_FILES += surgext/src/DelayLineByFreq.cpp
MINIPLUGIN_FILES += surgext/src/DelayLineByFreqExpanded.cpp
MINIPLUGIN_FILES += surgext/src/EGxVCA.cpp
MINIPLUGIN_FILES += surgext/src/FX.cpp
MINIPLUGIN_FILES += surgext/src/LFO.cpp
MINIPLUGIN_FILES += surgext/src/Mixer.cpp
MINIPLUGIN_FILES += surgext/src/ModMatrix.cpp
MINIPLUGIN_FILES += surgext/src/QuadAD.cpp
MINIPLUGIN_FILES += surgext/src/QuadLFO.cpp
MINIPLUGIN_FILES += surgext/src/VCO.cpp
MINIPLUGIN_FILES += surgext/src/VCF.cpp
MINIPLUGIN_FILES += surgext/src/Waveshaper.cpp
MINIPLUGIN_FILES += surgext/src/XTModule.cpp
MINIPLUGIN_FILES += surgext/src/XTModuleWidget.cpp
MINIPLUGIN_FILES += surgext/src/XTStyle.cpp
MINIPLUGIN_FILES += surgext-helper/surgext-helper.cpp

# --------------------------------------------------------------
# unless_modules

PLUGIN_FILES += $(filter-out unless_modules/src/unless.cpp,$(wildcard unless_modules/src/*.cpp))

# modules/types which are present in other plugins
UNLESS_MODULES_CUSTOM = Selection sign

# --------------------------------------------------------------
# ValleyAudio

PLUGIN_FILES += $(filter-out ValleyAudio/src/Valley.cpp,$(wildcard ValleyAudio/src/*.cpp))
PLUGIN_FILES += $(wildcard ValleyAudio/src/*/*.cpp)
PLUGIN_FILES += $(wildcard ValleyAudio/src/*/*/*.cpp)

PLUGIN_BINARIES += ValleyAudio/src/ADD_BANK1.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_BANK2.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_BANK3.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_BANK4.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_SAW.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_SINE.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_SQR.bin
PLUGIN_BINARIES += ValleyAudio/src/ALTOSAX.bin
PLUGIN_BINARIES += ValleyAudio/src/AM_HARM.bin
PLUGIN_BINARIES += ValleyAudio/src/BASIC.bin
PLUGIN_BINARIES += ValleyAudio/src/BI_PULSE.bin
PLUGIN_BINARIES += ValleyAudio/src/BITCRUSH1.bin
PLUGIN_BINARIES += ValleyAudio/src/BITCRUSH2.bin
PLUGIN_BINARIES += ValleyAudio/src/CELLO_1.bin
PLUGIN_BINARIES += ValleyAudio/src/CELLO_2.bin
PLUGIN_BINARIES += ValleyAudio/src/CHIP_1.bin
PLUGIN_BINARIES += ValleyAudio/src/CHIP_2.bin
PLUGIN_BINARIES += ValleyAudio/src/CHIRP.bin
PLUGIN_BINARIES += ValleyAudio/src/DISTORT.bin
PLUGIN_BINARIES += ValleyAudio/src/EBASS.bin
PLUGIN_BINARIES += ValleyAudio/src/FM1.bin
PLUGIN_BINARIES += ValleyAudio/src/FM2.bin
PLUGIN_BINARIES += ValleyAudio/src/FM3.bin
PLUGIN_BINARIES += ValleyAudio/src/FM4.bin
PLUGIN_BINARIES += ValleyAudio/src/FOLD_SINE.bin
PLUGIN_BINARIES += ValleyAudio/src/GMTRY_1.bin
PLUGIN_BINARIES += ValleyAudio/src/GMTRY_2.bin
PLUGIN_BINARIES += ValleyAudio/src/GMTRY_3.bin
PLUGIN_BINARIES += ValleyAudio/src/GRIT.bin
PLUGIN_BINARIES += ValleyAudio/src/LINEAR.bin
PLUGIN_BINARIES += ValleyAudio/src/OBOE.bin
PLUGIN_BINARIES += ValleyAudio/src/OPAL.bin
PLUGIN_BINARIES += ValleyAudio/src/OVERTONE1.bin
PLUGIN_BINARIES += ValleyAudio/src/OVERTONE2.bin
PLUGIN_BINARIES += ValleyAudio/src/PIANO.bin
PLUGIN_BINARIES += ValleyAudio/src/PLAITS_1.bin
PLUGIN_BINARIES += ValleyAudio/src/PLAITS_2.bin
PLUGIN_BINARIES += ValleyAudio/src/PLAITS_3.bin
PLUGIN_BINARIES += ValleyAudio/src/PLUCK.bin
PLUGIN_BINARIES += ValleyAudio/src/PWM.bin
PLUGIN_BINARIES += ValleyAudio/src/REED.bin
PLUGIN_BINARIES += ValleyAudio/src/RESO_SAW.bin
PLUGIN_BINARIES += ValleyAudio/src/RESO_SQR.bin
PLUGIN_BINARIES += ValleyAudio/src/SAW_GAP1.bin
PLUGIN_BINARIES += ValleyAudio/src/SAW_GAP2.bin
PLUGIN_BINARIES += ValleyAudio/src/SAW_PHASE.bin
PLUGIN_BINARIES += ValleyAudio/src/SINE_HARM.bin
PLUGIN_BINARIES += ValleyAudio/src/SWEEPHARM.bin
PLUGIN_BINARIES += ValleyAudio/src/SYMMETRY.bin
PLUGIN_BINARIES += ValleyAudio/src/TEE_EKS.bin
PLUGIN_BINARIES += ValleyAudio/src/THEREMIN.bin
PLUGIN_BINARIES += ValleyAudio/src/TWO_OP_RAND.bin
PLUGIN_BINARIES += ValleyAudio/src/TWO_OPFM1.bin
PLUGIN_BINARIES += ValleyAudio/src/TWO_OPFM2.bin
PLUGIN_BINARIES += ValleyAudio/src/VIDEOGAME.bin
PLUGIN_BINARIES += ValleyAudio/src/VIOLIN.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_1.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_2.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_3.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_4.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_5.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_6.bin
PLUGIN_BINARIES += ValleyAudio/src/VOX_MACH.bin
PLUGIN_BINARIES += ValleyAudio/src/XFADE.bin

# MINIPLUGIN_FILES += ValleyAudio/src/PlainText.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/SteppedSlider.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/ValleyChoiceMenu.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Common/FreqLUT.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Common/Utilities.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Common/DSP/OnePoleFilters.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Common/DSP/OTAFilter.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Common/SIMD/QuadOsc.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Common/SIMD/VecLoopingADSR.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Common/SIMD/VecOnePoleFilters.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Common/SIMD/VecOTAFilter.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Dexter/Dexter.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Dexter/DexterRoutingMatrix.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Dexter/Osc4Core_SIMD.cpp
# MINIPLUGIN_FILES += ValleyAudio/src/Interzone/Interzone.cpp

# MINIPLUGIN_BINARIES += ValleyAudio/src/ADD_BANK1.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/ADD_SAW.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/ADD_SINE.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/ADD_SQR.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/ALTOSAX.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/AM_HARM.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/BASIC.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/BITCRUSH1.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/BITCRUSH2.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/BI_PULSE.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/CELLO_1.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/CELLO_2.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/CHIP_1.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/CHIP_2.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/OBOE.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/OPAL.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/OVERTONE1.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/OVERTONE2.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/PIANO.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/PLUCK.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/PWM.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/SAW_GAP1.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/SAW_GAP2.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/SINE_HARM.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/SWEEPHARM.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/SYMMETRY.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/TEE_EKS.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/THEREMIN.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/VIDEOGAME.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/VIOLIN.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/VOICE_1.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/VOICE_2.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/VOICE_3.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/VOICE_4.bin
# MINIPLUGIN_BINARIES += ValleyAudio/src/VOICE_5.bin

# modules/types which are present in other plugins
VALLEYAUDIO_CUSTOM = $(DRWAV) Chord DigitalDisplay
VALLEYAUDIO_CUSTOM_PER_FILE = TempoKnob

# --------------------------------------------------------------
# Voxglitch

PLUGIN_FILES += $(filter-out voxglitch/src/plugin.cpp,$(wildcard voxglitch/src/*.cpp))

# modules/types which are present in other plugins
VOXGLITCH_CUSTOM = ADSR AudioFile Looper Readout
VOXGLITCH_CUSTOM_PER_FILE = AudioBuffer GateSequencer Grain SamplePlayer Sequencer SequencerDisplay VoltageSequencer

# --------------------------------------------------------------
# WhatTheRack

PLUGIN_FILES += $(filter-out WhatTheRack/src/WhatTheRack.cpp,$(wildcard WhatTheRack/src/*.cpp))

# --------------------------------------------------------------
# ZetaCarinaeModules

PLUGIN_FILES += $(filter-out ZetaCarinaeModules/src/plugin.cpp,$(wildcard ZetaCarinaeModules/src/*.cpp))

# --------------------------------------------------------------
# ZZC

PLUGIN_FILES += $(filter-out ZZC/src/ZZC.cpp ZZC/src/WavetablePlayer.cpp,$(wildcard ZZC/src/*.cpp))
# PLUGIN_FILES += ZZC/src/dsp/Wavetable.cpp
# PLUGIN_FILES += ZZC/src/filetypes/WavSupport.cpp

# modules/types which are present in other plugins
ZZC_CUSTOM = Clock LowFrequencyOscillator

# --------------------------------------------------------------
# Build setup

ifeq ($(HEADLESS),true)
BUILD_DIR = ../build-headless/plugins
else
BUILD_DIR = ../build/plugins
endif

BASE_FLAGS += -DBUILDING_PLUGIN_MODULES
BASE_FLAGS += -DDONT_SET_USING_DISTRHO_NAMESPACE
BASE_FLAGS += -I$(abspath $(ROOT)/include/osdialog-stub)

ifeq ($(BSD)$(WASM),true)
BASE_FLAGS += -D'aligned_alloc_16(ptr)'='aligned_alloc(16,ptr)'
BASE_FLAGS += -D'aligned_free_16(ptr)'='free(ptr)'
endif

ifeq ($(WASM),true)
BASE_FLAGS += -DEIGEN_DONT_VECTORIZE=1
endif

ifeq ($(shell $(PKG_CONFIG) --exists fftw3f && echo true),true)
BASE_FLAGS += -DHAVE_FFTW3F
endif

ifeq ($(shell $(PKG_CONFIG) --exists sndfile && echo true),true)
BASE_FLAGS += -DHAVE_SNDFILE
endif

ifeq ($(HAVE_X11),true)
BASE_FLAGS += -DHAVE_X11
endif

# --------------------------------------------------------------
# lots of warnings from plugins

BASE_FLAGS += -Wno-deprecated-declarations

ifeq ($(MACOS),true)
BASE_FLAGS += -Wno-unknown-warning-option
endif

# --------------------------------------------------------------
# Carla and friends build flags

CARLA_FLAGS = \
	-DCARLA_BACKEND_NAMESPACE=Cardinal \
	-DREAL_BUILD \
	-DSTATIC_PLUGIN_TARGET \
	-I../deps/aubio/src \
	-I../carla/source/backend \
	-I../carla/source/includes \
	-I../carla/source/modules \
	-I../carla/source/utils

ifeq ($(WASM),true)
CARLA_FLAGS += -DDISTRHO_RUNNER_INDIRECT_WASM_CALLS
endif

# --------------------------------------------------------------
# RTNeural flags, used in AIDA-X

RTNEURAL_FLAGS  = -std=gnu++17
RTNEURAL_FLAGS += -DBiquad=AidaBiquad
RTNEURAL_FLAGS += -DEigen=Aida_Eigen
RTNEURAL_FLAGS += -Dconst_blas_data_mapper=Aida_const_blas_data_mapper
RTNEURAL_FLAGS += -Devaluator=Aida_evaluator
RTNEURAL_FLAGS += -DSKIP_MINGW_FORMAT
RTNEURAL_FLAGS += -DRTNEURAL_DEFAULT_ALIGNMENT=16
RTNEURAL_FLAGS += -DRTNEURAL_USE_EIGEN=1

RTNEURAL_FLAGS += -ICardinal/src/AIDA-X/RTNeural
RTNEURAL_FLAGS += -ICardinal/src/AIDA-X/RTNeural/modules/Eigen

# --------------------------------------------------------------
# Build targets

ifeq ($(HEADLESS),true)
TARGET_SUFFIX = -headless
endif

TARGETS = plugins$(TARGET_SUFFIX).a plugins-mini$(TARGET_SUFFIX).a

all: $(TARGETS)
ifneq ($(HEADLESS),true)
	$(MAKE) HEADLESS=true plugins-mini-headless.a
endif

clean:
	rm -f *.a
	rm -rf $(BUILD_DIR)
	rm -rf surgext/build

# --------------------------------------------------------------

PLUGIN_LIST = $(subst /plugin.json,,$(wildcard */plugin.json))

UNWANTED_FILES  = HetrickCV/res/illustrator - deprecated/MyModule.svg
UNWANTED_FILES += $(wildcard Meander/res/*)
UNWANTED_FILES += $(wildcard Mog/res/*)
UNWANTED_FILES += $(wildcard Mog/res/*/*)
UNWANTED_FILES += $(wildcard nonlinearcircuits/res/*)

RESOURCE_FILES = \
	$(filter-out $(UNWANTED_FILES), \
		$(wildcard */res/*.svg) \
		$(wildcard */res/*/*.svg) \
		$(wildcard */res/*/*/*.svg) \
		$(wildcard */res/*/*/*/*.svg) \
		$(wildcard */res/*/*/*/*/*.svg) \
		$(wildcard */res/*.otf) \
		$(wildcard */res/*/*.otf) \
		$(wildcard */res/*/*/*.otf) \
		$(wildcard */res/*.ttf) \
		$(wildcard */res/*/*.ttf) \
		$(wildcard */res/*/*/*.ttf))

RESOURCE_FILES += $(wildcard */presets)
RESOURCE_FILES += $(wildcard Orbits/res/*.json)
RESOURCE_FILES += ArableInstruments/res/Joni.png
RESOURCE_FILES += BaconPlugs/res/Keypunch029.json
RESOURCE_FILES += BaconPlugs/res/midi/chopin
RESOURCE_FILES += BaconPlugs/res/midi/debussy
RESOURCE_FILES += BaconPlugs/res/midi/goldberg
RESOURCE_FILES += Befaco/res/SpringReverbIR.f32
RESOURCE_FILES += cf/playeroscs
RESOURCE_FILES += DHE-Modules/svg
RESOURCE_FILES += DrumKit/res/samples
RESOURCE_FILES += Meander/res
RESOURCE_FILES += MindMeldModular/res/ShapeMaster/CommunityPresets
RESOURCE_FILES += MindMeldModular/res/ShapeMaster/CommunityShapes
RESOURCE_FILES += MindMeldModular/res/ShapeMaster/MindMeldPresets
RESOURCE_FILES += MindMeldModular/res/ShapeMaster/MindMeldShapes
RESOURCE_FILES += Mog/res
RESOURCE_FILES += nonlinearcircuits/res
RESOURCE_FILES += ParableInstruments/res/Neil.png
RESOURCE_FILES += surgext/build/surge-data/configuration.xml
RESOURCE_FILES += surgext/build/surge-data/fx_presets
RESOURCE_FILES += surgext/build/surge-data/wavetables
RESOURCE_FILES += surgext/build/surge-data/windows.wt
RESOURCE_FILES += $(wildcard surgext/res/xt/fonts/quicksand/*.ttf)
RESOURCE_FILES += $(wildcard unless_modules/art/*.art)
RESOURCE_FILES += $(wildcard unless_modules/art/svg/*/*.svg)
RESOURCE_FILES += $(wildcard unless_modules/font/*.ttf)
# RESOURCE_FILES += $(wildcard unless_modules/manual/*)

JACK_RESOURCES  = $(CURDIR)/surgext/build/surge-data/configuration.xml
JACK_RESOURCES += $(CURDIR)/surgext/build/surge-data/fx_presets
JACK_RESOURCES += $(CURDIR)/surgext/build/surge-data/wavetables
JACK_RESOURCES += $(CURDIR)/surgext/build/surge-data/windows.wt

RESOURCE_FILES += $(wildcard Cardinal/res/*.png)
RESOURCE_FILES += Cardinal/res/Miku/Miku.png

MINIPLUGIN_LIST     = Cardinal
MINIPLUGIN_LIST    += AriaModules
MINIPLUGIN_LIST    += AudibleInstruments
MINIPLUGIN_LIST    += BogaudioModules
MINIPLUGIN_LIST    += Fundamental
MINIPLUGIN_LIST    += MockbaModular
MINIPLUGIN_LIST    += surgext
# MINIPLUGIN_LIST    += ValleyAudio

MINIRESOURCE_FILES  = AriaModules/res/components/jack-transparent.svg
MINIRESOURCE_FILES += AriaModules/res/components/pushbutton-500-off.svg
MINIRESOURCE_FILES += AriaModules/res/components/pushbutton-500-on.svg
MINIRESOURCE_FILES += AriaModules/res/components/pushbutton-500-pink.svg
MINIRESOURCE_FILES += AriaModules/res/components/screw.svg
MINIRESOURCE_FILES += AriaModules/res/faceplates/Spleet.svg
MINIRESOURCE_FILES += AriaModules/res/faceplates/Swerge.svg
MINIRESOURCE_FILES += AriaModules/res/signature/signature.svg

MINIRESOURCE_FILES += AudibleInstruments/res/Plaits.svg
# MINIRESOURCE_FILES += AudibleInstruments/res/hdad-segment14-1.002/Segment14.ttf

MINIRESOURCE_FILES += BogaudioModules/res/AD-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Additator-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/AddrSeq-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/AddrSeqX-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/ADSR-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/AMRM-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Analyzer-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/AnalyzerXL.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Arp-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/ASR-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Assign-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Bool-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/button_18px_0.svg
MINIRESOURCE_FILES += BogaudioModules/res/button_18px_1_green.svg
MINIRESOURCE_FILES += BogaudioModules/res/button_18px_1_orange.svg
MINIRESOURCE_FILES += BogaudioModules/res/button_18px_1.svg
MINIRESOURCE_FILES += BogaudioModules/res/button_9px_0.svg
MINIRESOURCE_FILES += BogaudioModules/res/button_9px_1_green.svg
MINIRESOURCE_FILES += BogaudioModules/res/button_9px_1.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Chirp-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Clpr-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Cmp-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/CmpDist-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/CVD-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/DADSRH-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/DADSRHPlus-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Detune-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/DGate-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Edge-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/EightFO-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/EightOne-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/EQ-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/EQS-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/FFB-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/FlipFlop-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/FMOp-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Follow-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/FourFO-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/FourMan-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Inv-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/knob_16px-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/knob_19px-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/knob_26px-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/knob_29px-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/knob_38px-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/knob_45px-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/knob_68px-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Lag-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/LFO-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Lgsw-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/LLFO-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/LLPG-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Lmtr-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/LPG-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/LVCF-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/LVCO-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Manual-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/MegaGate-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mix1-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mix2-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mix4-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mix4x-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mix8-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mix8x-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mono-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mult-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mumix-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Mute8-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/Noise-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Nsgt-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/Offset-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/OneEight-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Pan-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PEQ14-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PEQ14XF-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PEQ14XR-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PEQ14XV-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PEQ6-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PEQ6XF-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PEQ-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Pgmr-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PgmrX-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PolyCon16-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PolyCon8-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PolyMult-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PolyOff16-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/PolyOff8-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/port-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Pressor-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Pulse-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Ranalyzer.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Reftone-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/RGate-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/SampleHold-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Shaper-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/ShaperPlus-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Sine-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Slew-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/slider_switch_2_14px_0.svg
MINIRESOURCE_FILES += BogaudioModules/res/slider_switch_2_14px_1.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Stack-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Sums-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/Switch-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Switch1616-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/Switch18-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Switch44-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Switch81-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Switch88-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Test-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Test2-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/TestExpanderBase-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/TestExpanderExtension-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/TestGl-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/TestVCF-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/UMix-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/Unison-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/VCA-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/VCAmp-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/VCF-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/VCM-dark.svg
MINIRESOURCE_FILES += BogaudioModules/res/VCO-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Velo-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Vish-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/VU-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Walk2-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/Walk-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/XCO-dark.svg
# MINIRESOURCE_FILES += BogaudioModules/res/XFade-dark.svg

MINIRESOURCE_FILES += Cardinal/res/HostAudio.svg
MINIRESOURCE_FILES += Cardinal/res/HostCV.svg
MINIRESOURCE_FILES += Cardinal/res/HostMIDICC.svg
MINIRESOURCE_FILES += Cardinal/res/HostMIDIGate.svg
MINIRESOURCE_FILES += Cardinal/res/HostMIDIMap.svg
MINIRESOURCE_FILES += Cardinal/res/HostMIDI.svg
MINIRESOURCE_FILES += Cardinal/res/HostParameters.svg
MINIRESOURCE_FILES += Cardinal/res/HostParamsMap.svg
MINIRESOURCE_FILES += Cardinal/res/HostTime.svg

MINIRESOURCE_FILES += Fundamental/presets
MINIRESOURCE_FILES += Fundamental/res/ADSR.svg
MINIRESOURCE_FILES += Fundamental/res/LFO.svg
MINIRESOURCE_FILES += Fundamental/res/Merge.svg
MINIRESOURCE_FILES += Fundamental/res/MidSide.svg
MINIRESOURCE_FILES += Fundamental/res/Noise.svg
MINIRESOURCE_FILES += Fundamental/res/Quantizer.svg
MINIRESOURCE_FILES += Fundamental/res/Random.svg
MINIRESOURCE_FILES += Fundamental/res/Scope.svg
MINIRESOURCE_FILES += Fundamental/res/Split.svg
MINIRESOURCE_FILES += Fundamental/res/Sum.svg
MINIRESOURCE_FILES += Fundamental/res/VCA-1.svg
MINIRESOURCE_FILES += Fundamental/res/VCF.svg
MINIRESOURCE_FILES += Fundamental/res/VCMixer.svg
MINIRESOURCE_FILES += Fundamental/res/VCO.svg
MINIRESOURCE_FILES += Fundamental/res/components/ADSR-bg.svg
MINIRESOURCE_FILES += Fundamental/res/components/button-off.svg
MINIRESOURCE_FILES += Fundamental/res/components/button-on.svg
MINIRESOURCE_FILES += Fundamental/res/components/knob-marker-small.svg
MINIRESOURCE_FILES += Fundamental/res/components/knob-marker.svg
MINIRESOURCE_FILES += Fundamental/res/components/knob-small.svg
MINIRESOURCE_FILES += Fundamental/res/components/knob.svg
MINIRESOURCE_FILES += Fundamental/res/components/port.svg
MINIRESOURCE_FILES += Fundamental/res/components/Quantizer-keyboard.svg
MINIRESOURCE_FILES += Fundamental/res/components/Scope-bg.svg

MINIRESOURCE_FILES += MockbaModular/res/CZOsc.svg
MINIRESOURCE_FILES += MockbaModular/res/Empty_gray.svg
MINIRESOURCE_FILES += MockbaModular/res/Filtah.svg
MINIRESOURCE_FILES += MockbaModular/res/HSW_0.svg
MINIRESOURCE_FILES += MockbaModular/res/HSW_1.svg
MINIRESOURCE_FILES += MockbaModular/res/HSW_2.svg
MINIRESOURCE_FILES += MockbaModular/res/knob.svg
MINIRESOURCE_FILES += MockbaModular/res/MaugOsc.svg
MINIRESOURCE_FILES += MockbaModular/res/Mixah.svg
MINIRESOURCE_FILES += MockbaModular/res/Pannah.svg
MINIRESOURCE_FILES += MockbaModular/res/port.svg
MINIRESOURCE_FILES += MockbaModular/res/ReVoltah.svg
MINIRESOURCE_FILES += MockbaModular/res/screw.svg
MINIRESOURCE_FILES += MockbaModular/res/selector.svg
MINIRESOURCE_FILES += MockbaModular/res/Shapah.svg

MINIRESOURCE_FILES += surgext/build/surge-data/configuration.xml
MINIRESOURCE_FILES += surgext/build/surge-data/windows.wt
MINIRESOURCE_FILES += surgext/res/xt/fonts/quicksand/Quicksand-Regular.ttf
MINIRESOURCE_FILES += surgext/res/xt/fonts/quicksand/Quicksand-Bold.ttf
MINIRESOURCE_FILES += surgext/res/xt/dark/components/fader_bg.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/fader_bg_25.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/fader_handle.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/knob-9.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/knob-12.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/knob-14.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/knob-16.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/knob-pointer-9.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/knob-pointer-12.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/knob-pointer-14.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/knob-pointer-16.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/mod-button.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/components/port.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/panels/other/EGLFO.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/panels/other/Matrix.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/panels/other/Mixer.svg
MINIRESOURCE_FILES += surgext/res/xt/dark/panels/vco/BlankVCO.svg
MINIRESOURCE_FILES += surgext/res/xt/glyphs/lt_0.svg
MINIRESOURCE_FILES += surgext/res/xt/glyphs/lt_1.svg
MINIRESOURCE_FILES += surgext/res/xt/glyphs/lt_2.svg
MINIRESOURCE_FILES += surgext/res/xt/glyphs/lt_3.svg
MINIRESOURCE_FILES += surgext/res/xt/glyphs/lt_4.svg
MINIRESOURCE_FILES += surgext/res/xt/glyphs/lt_5.svg
MINIRESOURCE_FILES += surgext/res/xt/glyphs/lt_6.svg
MINIRESOURCE_FILES += surgext/res/xt/glyphs/lt_7.svg

# MINIRESOURCE_FILES += ValleyAudio/res/DexterPanelDark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/InterzonePanelDark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/LightLEDButton80.svg
# MINIRESOURCE_FILES += ValleyAudio/res/PJ301MDarkSmallOut.svg
# MINIRESOURCE_FILES += ValleyAudio/res/PJ301MDarkSmall.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo0.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo1.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo2.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo3.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo4.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo5.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo6.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo7.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo8.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo9.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo10.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo11.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo12.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo13.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo14.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo15.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo16.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo17.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo18.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo19.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo20.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo21.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo22.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo0Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo1Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo2Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo3Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo4Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo5Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo6Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo7Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo8Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo9Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo10Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo11Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo12Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo13Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo14Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo15Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo16Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo17Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo18Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo19Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo20Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo21Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/algo22Dark.svg
# MINIRESOURCE_FILES += ValleyAudio/res/din1451alt.ttf
# MINIRESOURCE_FILES += ValleyAudio/res/sliderBlue.svg
# MINIRESOURCE_FILES += ValleyAudio/res/sliderGreen.svg
# MINIRESOURCE_FILES += ValleyAudio/res/sliderRed.svg
# MINIRESOURCE_FILES += ValleyAudio/res/sliderOrange.svg
# MINIRESOURCE_FILES += ValleyAudio/res/sliderYellow.svg
# MINIRESOURCE_FILES += ValleyAudio/res/valleySliderBackground.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSBlueMed.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSBlueMed-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSBlueMedSmall.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSBlueMedSmall-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSMed-bg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSMedSmall-bg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSGreenMed.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSGreenMed-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSOrangeMed.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSOrangeMed-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSRedMed.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSRedMed-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSPurpleMed.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Med/Rogan1PSPurpleMed-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSSmall-bg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSBlueSmall.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSBlueSmall-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSOrangeSmall.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSOrangeSmall-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSGreenSmall.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSGreenSmall-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSRedSmall.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSRedSmall-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSMustardSmall.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSMustardSmall-fg.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSPurpleSmall.svg
# MINIRESOURCE_FILES += ValleyAudio/res/v2/Small/Rogan1PSPurpleSmall-fg.svg

# MOD builds only have LV2 main and FX variant
ifeq ($(MOD_BUILD),true)

LV2_RESOURCES  = $(PLUGIN_LIST:%=../bin/Cardinal.lv2/resources/PluginManifests/%.json)
LV2_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.lv2/resources/PluginManifests/%.json)
LV2_RESOURCES += $(MINIPLUGIN_LIST:%=../bin/CardinalMini.lv2/resources/PluginManifests/%.json)
LV2_RESOURCES += $(RESOURCE_FILES:%=../bin/Cardinal.lv2/resources/%)
LV2_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalFX.lv2/resources/%)
LV2_RESOURCES += $(MINIRESOURCE_FILES:%=../bin/CardinalMini.lv2/resources/%)

# MOD builds only have LV2 FX variant for now
else ifeq ($(WASM),true)

LV2_RESOURCES  = $(PLUGIN_LIST:%=../bin/CardinalNative.lv2/resources/PluginManifests/%.json)
LV2_RESOURCES += $(MINIPLUGIN_LIST:%=../bin/CardinalMini.lv2/resources/PluginManifests/%.json)
LV2_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalNative.lv2/resources/%)
LV2_RESOURCES += $(MINIRESOURCE_FILES:%=../bin/CardinalMini.lv2/resources/%)

else

LV2_RESOURCES  = $(PLUGIN_LIST:%=../bin/Cardinal.lv2/resources/PluginManifests/%.json)
LV2_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.lv2/resources/PluginManifests/%.json)
LV2_RESOURCES += $(MINIPLUGIN_LIST:%=../bin/CardinalMini.lv2/resources/PluginManifests/%.json)
LV2_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalSynth.lv2/resources/PluginManifests/%.json)
LV2_RESOURCES += $(RESOURCE_FILES:%=../bin/Cardinal.lv2/resources/%)
LV2_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalFX.lv2/resources/%)
LV2_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalSynth.lv2/resources/%)
LV2_RESOURCES += $(MINIRESOURCE_FILES:%=../bin/CardinalMini.lv2/resources/%)

ifeq ($(MACOS),true)
VST2_RESOURCES  = $(PLUGIN_LIST:%=../bin/CardinalFX.vst/Contents/Resources/PluginManifests/%.json)
VST2_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalSynth.vst/Contents/Resources/PluginManifests/%.json)
VST2_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalFX.vst/Contents/Resources/%)
VST2_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalSynth.vst/Contents/Resources/%)
CLAP_RESOURCES  = $(PLUGIN_LIST:%=../bin/Cardinal.clap/Contents/Resources/PluginManifests/%.json)
CLAP_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.clap/Contents/Resources/PluginManifests/%.json)
CLAP_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalSynth.clap/Contents/Resources/PluginManifests/%.json)
CLAP_RESOURCES += $(RESOURCE_FILES:%=../bin/Cardinal.clap/Contents/Resources/%)
CLAP_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalFX.clap/Contents/Resources/%)
CLAP_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalSynth.clap/Contents/Resources/%)
else
VST2_RESOURCES  = $(PLUGIN_LIST:%=../bin/Cardinal.vst/resources/PluginManifests/%.json)
VST2_RESOURCES += $(RESOURCE_FILES:%=../bin/Cardinal.vst/resources/%)
CLAP_RESOURCES  = $(PLUGIN_LIST:%=../bin/Cardinal.clap/resources/PluginManifests/%.json)
CLAP_RESOURCES += $(RESOURCE_FILES:%=../bin/Cardinal.clap/resources/%)
endif

VST3_RESOURCES  = $(PLUGIN_LIST:%=../bin/Cardinal.vst3/Contents/Resources/PluginManifests/%.json)
VST3_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.vst3/Contents/Resources/PluginManifests/%.json)
VST3_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalSynth.vst3/Contents/Resources/PluginManifests/%.json)
VST3_RESOURCES += $(RESOURCE_FILES:%=../bin/Cardinal.vst3/Contents/Resources/%)
VST3_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalFX.vst3/Contents/Resources/%)
VST3_RESOURCES += $(RESOURCE_FILES:%=../bin/CardinalSynth.vst3/Contents/Resources/%)

endif

resources: $(JACK_RESOURCES) $(LV2_RESOURCES) $(VST2_RESOURCES) $(VST3_RESOURCES) $(CLAP_RESOURCES)

../bin/Cardinal.lv2/resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalFX.lv2/resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalMini.lv2/resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalNative.lv2/resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalSynth.lv2/resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

ifeq ($(MOD_BUILD),true)
../bin/Cardinal.lv2/resources/%.svg: %.svg ../deps/svg2stub.py
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)python3 ../deps/svg2stub.py $< $@

../bin/CardinalFX.lv2/resources/%.svg: %.svg ../deps/svg2stub.py
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)python3 ../deps/svg2stub.py $< $@

../bin/CardinalMini.lv2/resources/%.svg: %.svg ../deps/svg2stub.py
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)python3 ../deps/svg2stub.py $< $@
endif

../bin/Cardinal.lv2/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalFX.lv2/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalMini.lv2/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalNative.lv2/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalSynth.lv2/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/Cardinal.vst3/Contents/Resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalFX.vst3/Contents/Resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalSynth.vst3/Contents/Resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/Cardinal.vst3/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalFX.vst3/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalSynth.vst3/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

ifeq ($(MACOS),true)
../bin/CardinalFX.vst/Contents/Resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalSynth.vst/Contents/Resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalFX.vst/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalSynth.vst/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/Cardinal.clap/Contents/Resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalFX.clap/Contents/Resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalSynth.clap/Contents/Resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/Cardinal.clap/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalFX.clap/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/CardinalSynth.clap/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@
else
../bin/Cardinal.vst/resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/Cardinal.vst/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/Cardinal.clap/resources/%: %
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

../bin/Cardinal.clap/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@
endif

%/surgext/build/surge-data/configuration.xml: surgext/surge/resources/surge-shared/configuration.xml
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

%/surgext/build/surge-data/fx_presets:
	-@mkdir -p "$@"
	cp -R surgext/surge/resources/data/fx_presets/* $@/
	cp -R surgext/res/surge_extra_data/fx_presets/* $@/

%/surgext/build/surge-data/wavetables: surgext/surge/resources/data/wavetables
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

%/surgext/build/surge-data/windows.wt: surgext/surge/resources/surge-shared/windows.wt
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

# --------------------------------------------------------------
# Build commands

PLUGIN_OBJS  = $(PLUGIN_FILES:%=$(BUILD_DIR)/%.o)
PLUGIN_OBJS += $(PLUGIN_BINARIES:%=$(BUILD_DIR)/%.bin.o)

MINIPLUGIN_OBJS  = $(MINIPLUGIN_FILES:%=$(BUILD_DIR)/%.o)
MINIPLUGIN_OBJS += $(MINIPLUGIN_BINARIES:%=$(BUILD_DIR)/%.bin.o)

.PRECIOUS: $(PLUGIN_BINARIES:%=$(BUILD_DIR)/%.bin.c)

# function for custom module names macro
custom_module_names = -D${1}=${2}${1} -Dmodel${1}=model${2}${1} -D${1}Widget=${2}${1}Widget
custom_per_file_names = -D${1}=${2}_${1}

plugins$(TARGET_SUFFIX).a: $(PLUGIN_OBJS)
	@echo "Creating $@"
	$(SILENT)rm -f $@
	$(SILENT)$(AR) crs $@ $^

plugins-mini$(TARGET_SUFFIX).a: $(MINIPLUGIN_OBJS)
	@echo "Creating $@"
	$(SILENT)rm -f $@
	$(SILENT)$(AR) crs $@ $^

$(BUILD_DIR)/%.bin.c: % ../deps/res2c.py
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Generating $*.bin.c"
	$(SILENT)python3 ../deps/res2c.py $< > $@

$(BUILD_DIR)/%.bin.o: $(BUILD_DIR)/%.bin.c
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $*.bin"
	$(SILENT)$(CC) $< $(BUILD_C_FLAGS) -c -o $@

$(BUILD_DIR)/plugins.cpp.o: plugins.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(BUILD_DIR)/plugins-mini.cpp.o: plugins-mini.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(BUILD_DIR)/noplugins.cpp.o: plugins.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(BUILD_DIR)/Cardinal/%.cpp.o: Cardinal/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		-DpluginInstance=pluginInstance__Cardinal \
		-Dstbrp_context=stbrp_context_cardinal \
		-Dstbrp_coord=stbrp_coord_cardinal \
		-Dstbtt_fontinfo=stbtt_fontinfo_cardinal \
		-Dstbrp_node=stbrp_node_cardinal \
		-Dstbrp_rect=stbrp_rect_cardinal \
		$(CARLA_FLAGS)

$(BUILD_DIR)/Cardinal/src/AIDA-X.cpp.o: Cardinal/src/AIDA-X.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		-DpluginInstance=pluginInstance__Cardinal \
		-Dstbrp_context=stbrp_context_cardinal \
		-Dstbrp_coord=stbrp_coord_cardinal \
		-Dstbtt_fontinfo=stbtt_fontinfo_cardinal \
		-Dstbrp_node=stbrp_node_cardinal \
		-Dstbrp_rect=stbrp_rect_cardinal \
		$(RTNEURAL_FLAGS) \
		$(CARLA_FLAGS)

$(BUILD_DIR)/21kHz/%.cpp.o: 21kHz/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(21KHZ_CUSTOM),$(call custom_module_names,$(m),21kHz)) \
		-DpluginInstance=pluginInstance__21kHz

$(BUILD_DIR)/8Mode/%.cpp.o: 8Mode/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(8MODE_CUSTOM),$(call custom_module_names,$(m),8Mode)) \
		-DpluginInstance=pluginInstance__8Mode

$(BUILD_DIR)/AaronStatic/%.cpp.o: AaronStatic/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(AARONSTATIC_CUSTOM),$(call custom_module_names,$(m),AaronStatic)) \
		-DpluginInstance=pluginInstance__AaronStatic \
		-Dinit=init__AaronStatic

$(BUILD_DIR)/alefsbits/%.cpp.o: alefsbits/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ALEFSBITS_CUSTOM),$(call custom_module_names,$(m),alefsbits)) \
		-DpluginInstance=pluginInstance__alefsbits

$(BUILD_DIR)/Algoritmarte/%.cpp.o: Algoritmarte/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ALGORITMARTE_CUSTOM),$(call custom_module_names,$(m),Algoritmarte)) \
		-DpluginInstance=pluginInstance__Algoritmarte

$(BUILD_DIR)/AmalgamatedHarmonics/%.cpp.o: AmalgamatedHarmonics/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(AMALGAMATEDHARMONICS_CUSTOM),$(call custom_module_names,$(m),AmalgamatedHarmonics)) \
		-DpluginInstance=pluginInstance__AmalgamatedHarmonics

$(BUILD_DIR)/AnimatedCircuits/%.cpp.o: AnimatedCircuits/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ANIMATEDCIRCUITS_CUSTOM),$(call custom_module_names,$(m),AnimatedCircuits)) \
		-DpluginInstance=pluginInstance__AnimatedCircuits

$(BUILD_DIR)/ArableInstruments/%.o: ArableInstruments/%
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ARABLE_CUSTOM),$(call custom_module_names,$(m),Arable)) \
		-DpluginInstance=pluginInstance__ArableInstruments \
		-DTEST \
		-IArableInstruments/eurorack \
		-Wno-class-memaccess \
		-Wno-unused-local-typedefs

$(BUILD_DIR)/AriaModules/%.cpp.o: AriaModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ARIA_CUSTOM),$(call custom_module_names,$(m),Aria)) \
		-DpluginInstance=pluginInstance__Aria \
		-Wno-cast-function-type

$(BUILD_DIR)/AS/%.cpp.o: AS/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(AS_CUSTOM),$(call custom_module_names,$(m),AS)) \
		$(foreach m,$(AS_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),AS_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__AS \
		-Wno-unused-private-field

$(BUILD_DIR)/AudibleInstruments/%.o: AudibleInstruments/%
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(AUDIBLEINSTRUMENTS_CUSTOM),$(call custom_module_names,$(m),AudibleInstruments)) \
		-DpluginInstance=pluginInstance__AudibleInstruments \
		-DTEST \
		-IAudibleInstruments/eurorack \
		-Wno-class-memaccess \
		-Wno-unused-local-typedefs

$(BUILD_DIR)/Autinn/%.cpp.o: Autinn/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(AUTINN_CUSTOM),$(call custom_module_names,$(m),Autinn)) \
		-DpluginInstance=pluginInstance__Autinn \
		-Dinit=init__Autinn

$(BUILD_DIR)/Axioma/%.cpp.o: Axioma/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(AXIOMA_CUSTOM),$(call custom_module_names,$(m),Axioma)) \
		-DpluginInstance=pluginInstance__Axioma

$(BUILD_DIR)/BaconPlugs/%.cpp.o: BaconPlugs/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -std=gnu++17 -c -o $@ \
		$(foreach m,$(BACON_CUSTOM),$(call custom_module_names,$(m),BaconPlugs)) \
		-DpluginInstance=pluginInstance__Bacon \
		-DDARK_BACON \
		-IBaconPlugs/libs/midifile/include \
		-IBaconPlugs/libs/open303-code/Source/DSPCode \
		-Wno-array-bounds \
		-Wno-strict-aliasing

$(BUILD_DIR)/Befaco/%.cpp.o: Befaco/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BEFACO_CUSTOM),$(call custom_module_names,$(m),Befaco)) \
		-DpluginInstance=pluginInstance__Befaco

$(BUILD_DIR)/Bidoo%.cpp.o: Bidoo%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BIDOO_CUSTOM),$(call custom_module_names,$(m),Bidoo)) \
		$(foreach m,$(BIDOO_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),Bidoo_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__Bidoo \
		-DSKIP_MINGW_FORMAT \
		-IBidoo/src/dep/gverb/include \
		-Wno-ignored-qualifiers \
		-Wno-sign-compare \
		-Wno-unused-function

$(BUILD_DIR)/Biset/src/%.cpp.o: Biset/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BISET_CUSTOM),$(call custom_module_names,$(m),Biset)) \
		-DpluginInstance=pluginInstance__Biset

$(BUILD_DIR)/BogaudioModules/src/follower_base.cpp.o: BogaudioModules/src/follower_base.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BOGAUDIO_CUSTOM),$(call custom_module_names,$(m),Bogaudio)) \
		-DpluginInstance=pluginInstance__BogaudioModules \
		-DefGainMaxDecibels=efGainMaxDecibelsDebug \
		-DRACK_SIMD=1 \
		-DSKIP_MINGW_FORMAT \
		-IBogaudioModules/lib \
		-IBogaudioModules/src/dsp

$(BUILD_DIR)/BogaudioModules/%.cpp.o: BogaudioModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BOGAUDIO_CUSTOM),$(call custom_module_names,$(m),Bogaudio)) \
		$(foreach m,$(BOGAUDIO_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),Bogaudio_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__BogaudioModules \
		-DRACK_SIMD=1 \
		-DSKIP_MINGW_FORMAT \
		-IBogaudioModules/lib \
		-IBogaudioModules/src/dsp

$(BUILD_DIR)/CatroModulo/src/%.cpp.o: CatroModulo/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(CATROMODULO_CUSTOM),$(call custom_module_names,$(m),CatroModulo)) \
		-DpluginInstance=pluginInstance__CatroModulo

$(BUILD_DIR)/cf/src/%.cpp.o: cf/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(CF_CUSTOM),$(call custom_module_names,$(m),cf)) \
		-DpluginInstance=pluginInstance__cf \
		-Wno-misleading-indentation

$(BUILD_DIR)/Computerscare/src/%.cpp.o: Computerscare/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(COMPUTERSCARE_CUSTOM),$(call custom_module_names,$(m),Computerscare)) \
		-DpluginInstance=pluginInstance__Computerscare

$(BUILD_DIR)/CVfunk/src/%.cpp.o: CVfunk/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(CVFUNK_CUSTOM),$(call custom_module_names,$(m),CVfunk)) \
		-DpluginInstance=pluginInstance__CVfunk

$(BUILD_DIR)/ChowDSP/%.cpp.o: ChowDSP/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(CHOWDSP_CUSTOM),$(call custom_module_names,$(m),ChowDSP)) \
		-DpluginInstance=pluginInstance__ChowDSP \
		-DUSE_EIGEN \
		-DSKIP_MINGW_FORMAT \
		-IChowDSP/lib \
		-IChowDSP/lib/chowdsp_utils/modules/chowdsp_dsp/WDF \
		-Wno-deprecated-copy

$(BUILD_DIR)/dBiz/src/%.cpp.o: dBiz/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(DBIZ_CUSTOM),$(call custom_module_names,$(m),dBiz)) \
		$(foreach m,$(DBIZ_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),dBiz_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__dBiz

$(BUILD_DIR)/DHE-Modules/src/%.cpp.o: DHE-Modules/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(DHE_CUSTOM),$(call custom_module_names,$(m),DHE)) \
		-DpluginInstance=pluginInstance__DHE \
		-IDHE-Modules/src

$(BUILD_DIR)/DrumKit/%.cpp.o: DrumKit/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(DRUMKIT_CUSTOM),$(call custom_module_names,$(m),DrumKit)) \
		-DpluginInstance=pluginInstance__DrumKit \
		-Dinit=init__DrumKit \
		-Wno-sign-compare

$(BUILD_DIR)/EnigmaCurry/src/%.cpp.o: EnigmaCurry/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ENIGMACURRY_CUSTOM),$(call custom_module_names,$(m),EnigmaCurry)) \
		-DpluginInstance=pluginInstance__EnigmaCurry \
		-Wno-misleading-indentation

$(BUILD_DIR)/ESeries/%.cpp.o: ESeries/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ESERIES_CUSTOM),$(call custom_module_names,$(m),ESeries)) \
		-DpluginInstance=pluginInstance__ESeries

$(BUILD_DIR)/ExpertSleepers-Encoders/src/%.cpp.o: ExpertSleepers-Encoders/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(EXPERTSLEEPERS_ENCODERS_CUSTOM),$(call custom_module_names,$(m),ExpertSleepersEncoders)) \
		-DpluginInstance=pluginInstance__ExpertSleepersEncoders

$(BUILD_DIR)/Extratone/src/%.cpp.o: Extratone/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(EXTRATONE_CUSTOM),$(call custom_module_names,$(m),Extratone)) \
		-DpluginInstance=pluginInstance__Extratone

$(BUILD_DIR)/FehlerFabrik/%.cpp.o: FehlerFabrik/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(FEHLERFABRIK_CUSTOM),$(call custom_module_names,$(m),FehlerFabrik)) \
		-DpluginInstance=pluginInstance__FehlerFabrik

$(BUILD_DIR)/forsitan-modulare/%.cpp.o: forsitan-modulare/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(FORSITAN_CUSTOM),$(call custom_module_names,$(m),forsitan)) \
		-DpluginInstance=pluginInstance__forsitan

$(BUILD_DIR)/Fundamental/%.c.o: Fundamental/%.c
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CC) $< $(BUILD_C_FLAGS) -c -o $@ \
		$(foreach m,$(FUNDAMENTAL_CUSTOM),$(call custom_module_names,$(m),Fundamental)) \
		-DpluginInstance=pluginInstance__Fundamental

$(BUILD_DIR)/Fundamental/%.cpp.o: Fundamental/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(FUNDAMENTAL_CUSTOM),$(call custom_module_names,$(m),Fundamental)) \
		-DpluginInstance=pluginInstance__Fundamental

$(BUILD_DIR)/Fundamental/src/Scope.cpp.o: Fundamental/src/Scope.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(FUNDAMENTAL_CUSTOM),$(call custom_module_names,$(m),Fundamental)) \
		-DpluginInstance=pluginInstance__Fundamental

$(BUILD_DIR)/GlueTheGiant/%.cpp.o: GlueTheGiant/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(GLUETHEGIANT_CUSTOM),$(call custom_module_names,$(m),GlueTheGiant)) \
		-DpluginInstance=pluginInstance__GlueTheGiant

$(BUILD_DIR)/GlueTheGiant/src/gtgComponents.cpp.o: GlueTheGiant/src/gtgComponents.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(GLUETHEGIANT_CUSTOM),$(call custom_module_names,$(m),GlueTheGiant)) \
		-DpluginInstance=pluginInstance__GlueTheGiant \
		-DloadGtgPluginDefault=ignoredGlueTheGiant1 \
		-DsaveGtgPluginDefault=ignoredGlueTheGiant2

$(BUILD_DIR)/GoodSheperd/%.cpp.o: GoodSheperd/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(GOODSHEPERD_CUSTOM),$(call custom_module_names,$(m),GoodSheperd)) \
		-DpluginInstance=pluginInstance__GoodSheperd

$(BUILD_DIR)/GrandeModular/%.cpp.o: GrandeModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(GRANDEMODULAR_CUSTOM),$(call custom_module_names,$(m),GrandeModular)) \
		-DpluginInstance=pluginInstance__GrandeModular \
		-Wno-missing-braces \
		-Wno-narrowing \
		-Wno-self-assign

$(BUILD_DIR)/h4n4-modules/%.cpp.o: h4n4-modules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(H4N4_CUSTOM),$(call custom_module_names,$(m),H4N4)) \
		-DpluginInstance=pluginInstance__H4N4

$(BUILD_DIR)/HamptonHarmonics/%.cpp.o: HamptonHarmonics/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(HAMPTONHARMONICS_CUSTOM),$(call custom_module_names,$(m),HamptonHarmonics)) \
		-DpluginInstance=pluginInstance__HamptonHarmonics

$(BUILD_DIR)/HetrickCV/%.cpp.o: HetrickCV/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(HETRICKCV_CUSTOM),$(call custom_module_names,$(m),HetrickCV)) \
		-DpluginInstance=pluginInstance__HetrickCV \
		-DSTDIO_OVERRIDE=HetrickCV \
		-IHetrickCV/Gamma \
		-Wno-unused-but-set-variable

$(BUILD_DIR)/ImpromptuModular/src/Foundr%.cpp.o: ImpromptuModular/src/Foundr%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM),$(call custom_module_names,$(m),ImpromptuModular)) \
		-DpluginInstance=pluginInstance__ImpromptuModular \
		-DStepAttributes=StepAttributesKernel \

$(BUILD_DIR)/ImpromptuModular/src/ImpromptuModular.cpp.o: ImpromptuModular/src/ImpromptuModular.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM),$(call custom_module_names,$(m),ImpromptuModular)) \
		-DpluginInstance=pluginInstance__ImpromptuModular \
		-Dinit=init__ImpromptuModular

$(BUILD_DIR)/ImpromptuModular%.cpp.o: ImpromptuModular%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM),$(call custom_module_names,$(m),ImpromptuModular)) \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),ImpromptuModular_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__ImpromptuModular \
		-Wno-format-truncation

$(BUILD_DIR)/ihtsyn/%.cpp.o: ihtsyn/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IHTSYN_CUSTOM),$(call custom_module_names,$(m),ihtsyn)) \
		$(foreach m,$(IHTSYN_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),ihtsyn_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__ihtsyn

$(BUILD_DIR)/JW-Modules/src/WavHead.cpp.o: JW-Modules/src/WavHead.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(JW_CUSTOM),$(call custom_module_names,$(m),JW)) \
		-DpluginInstance=pluginInstance__JW \
		-D'nvgRGB(r,g,b)=nvgRGBblank(r,g,b)' \
		-IJW-Modules/src \
		-IJW-Modules/lib/oscpack \
		-Wno-misleading-indentation \
		-Wno-unused-but-set-variable \
		-Wno-unused-result

$(BUILD_DIR)/JW-Modules/%.cpp.o: JW-Modules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(JW_CUSTOM),$(call custom_module_names,$(m),JW)) \
		-DpluginInstance=pluginInstance__JW \
		-IJW-Modules/src \
		-IJW-Modules/lib/oscpack \
		-Wno-misleading-indentation \
		-Wno-unused-but-set-variable \
		-Wno-unused-result

$(BUILD_DIR)/kocmoc/%.cpp.o: kocmoc/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(KOCMOC_CUSTOM),$(call custom_module_names,$(m),kocmoc)) \
		-DpluginInstance=pluginInstance__kocmoc

$(BUILD_DIR)/LifeFormModular/%.cpp.o: LifeFormModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(LIFEFORMMODULAR_CUSTOM),$(call custom_module_names,$(m),LifeFormModular)) \
		-DpluginInstance=pluginInstance__LifeFormModular

$(BUILD_DIR)/LilacLoop/%.cpp.o: LilacLoop/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(LILACLOOP_CUSTOM),$(call custom_module_names,$(m),LilacLoop)) \
		-DpluginInstance=pluginInstance__LilacLoop \
		-DSKIP_MINGW_FORMAT

$(BUILD_DIR)/LittleUtils/%.cpp.o: LittleUtils/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(LITTLEUTILS_CUSTOM),$(call custom_module_names,$(m),LittleUtils)) \
		-DpluginInstance=pluginInstance__LittleUtils

$(BUILD_DIR)/LomasModules/%.cpp.o: LomasModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(LOMAS_CUSTOM),$(call custom_module_names,$(m),Lomas)) \
		-DpluginInstance=pluginInstance__Lomas

$(BUILD_DIR)/LyraeModules/%.cpp.o: LyraeModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(LYRAE_CUSTOM),$(call custom_module_names,$(m),Lyrae)) \
		-DpluginInstance=pluginInstance__Lyrae

$(BUILD_DIR)/Meander/src/Meander.cpp.o: Meander/src/Meander.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MEANDER_CUSTOM),$(call custom_module_names,$(m),Meander)) \
		-DpluginInstance=pluginInstance__Meander \
		-Wno-format-security \
		-Wno-format-zero-length

$(BUILD_DIR)/MindMeldModular/src/MindMeldModular.cpp.o: MindMeldModular/src/MindMeldModular.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MINDMELD_CUSTOM),$(call custom_module_names,$(m),MindMeld)) \
		-DpluginInstance=pluginInstance__MindMeld \
		-Dinit=init__MindMeld

$(BUILD_DIR)/MindMeldModular/%.cpp.o: MindMeldModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MINDMELD_CUSTOM),$(call custom_module_names,$(m),MindMeld)) \
		-DpluginInstance=pluginInstance__MindMeld

$(BUILD_DIR)/ML_modules/%.cpp.o: ML_modules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ML_CUSTOM),$(call custom_module_names,$(m),ML)) \
		-DpluginInstance=pluginInstance__ML

$(BUILD_DIR)/MockbaModular/%.cpp.o: MockbaModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MOCKBAMODULAR_CUSTOM),$(call custom_module_names,$(m),MockbaModular)) \
		-DpluginInstance=pluginInstance__MockbaModular

$(BUILD_DIR)/Mog/%.cpp.o: Mog/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MOG_CUSTOM),$(call custom_module_names,$(m),Mog)) \
		-DpluginInstance=pluginInstance__Mog

$(BUILD_DIR)/mscHack/%.cpp.o: mscHack/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MSCHACK_CUSTOM),$(call custom_module_names,$(m),mscHack)) \
		$(foreach m,$(MSCHACK_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),mscHack_$(shell basename $*))) \
		-DthePlugin=pluginInstance__mscHack \
		-Dinit=init__mscHack \
		-Wno-class-memaccess \
		-Wno-format-overflow \
		-Wno-non-c-typedef-for-linkage \
		-Wno-unused-but-set-variable

$(BUILD_DIR)/MSM/%.cpp.o: MSM/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MSM_CUSTOM),$(call custom_module_names,$(m),MSM)) \
		-DpluginInstance=pluginInstance__MSM

$(BUILD_DIR)/myth-modules/%.cpp.o: myth-modules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MYTH_MODULES_CUSTOM),$(call custom_module_names,$(m),myth_modules)) \
		-DpluginInstance=pluginInstance__myth_modules

$(BUILD_DIR)/nonlinearcircuits/%.cpp.o: nonlinearcircuits/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(NONLINEARCIRCUITS_CUSTOM),$(call custom_module_names,$(m),nonlinearcircuits)) \
		-DpluginInstance=pluginInstance__nonlinearcircuits

$(BUILD_DIR)/Orbits/%.cpp.o: Orbits/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ORBITS_CUSTOM),$(call custom_module_names,$(m),Orbits)) \
		-DpluginInstance=pluginInstance__Orbits

$(BUILD_DIR)/ParableInstruments/%.o: ParableInstruments/%
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(PARABLE_CUSTOM),$(call custom_module_names,$(m),Parable)) \
		-DpluginInstance=pluginInstance__ParableInstruments \
		-DPARASITES \
		-DTEST \
		-IArableInstruments/parasites \
		-Wno-class-memaccess \
		-Wno-unused-local-typedefs

$(BUILD_DIR)/PathSet/%.cpp.o: PathSet/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(PATHSET_CUSTOM),$(call custom_module_names,$(m),PathSet)) \
		-DpluginInstance=pluginInstance__PathSet

$(BUILD_DIR)/PdArray/%.cpp.o: PdArray/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(PDARRAY_CUSTOM),$(call custom_module_names,$(m),PdArray)) \
		-DpluginInstance=pluginInstance__PdArray

$(BUILD_DIR)/PinkTrombone/%.cpp.o: PinkTrombone/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(PINKTROMBONE_CUSTOM),$(call custom_module_names,$(m),PinkTrombone)) \
		-DpluginInstance=pluginInstance__PinkTrombone

$(BUILD_DIR)/Prism/%.cpp.o: Prism/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(PRISM_CUSTOM),$(call custom_module_names,$(m),Prism)) \
		$(foreach m,$(PRISM_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),Prism_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__Prism \

$(BUILD_DIR)/rackwindows/%.cpp.o: rackwindows/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(RACKWINDOWS_CUSTOM),$(call custom_module_names,$(m),rackwindows)) \
		-DpluginInstance=pluginInstance__rackwindows \
		-Wno-implicit-fallthrough \
		-Wno-sign-compare

$(BUILD_DIR)/RebelTech/%.cpp.o: RebelTech/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(REBELTECH_CUSTOM),$(call custom_module_names,$(m),RebelTech)) \
		-DpluginInstance=pluginInstance__RebelTech

$(BUILD_DIR)/repelzen/%.cpp.o: repelzen/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(REPELZEN_CUSTOM),$(call custom_module_names,$(m),repelzen)) \
		-DpluginInstance=pluginInstance__repelzen

$(BUILD_DIR)/Sapphire/%.cpp.o: Sapphire/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(SAPPHIRE_CUSTOM),$(call custom_module_names,$(m),Sapphire)) \
		-DpluginInstance=pluginInstance__sapphire

$(BUILD_DIR)/sonusmodular/%.cpp.o: sonusmodular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(SONUSMODULAR_CUSTOM),$(call custom_module_names,$(m),sonusmodular)) \
		-DpluginInstance=pluginInstance__sonusmodular

$(BUILD_DIR)/StarlingVia/%.cpp.o: StarlingVia/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(STARLINGVIA_CUSTOM),$(call custom_module_names,$(m),StarlingVia)) \
		-DpluginInstance=pluginInstance__StarlingVia \
		-DBUILD_VIRTUAL \
		-IStarlingVia/dep/starling-dsp \
		-IStarlingVia/Via/modules/inc \
		-IStarlingVia/Via/synthesis/inc \
		-IStarlingVia/Via/io/inc \
		-IStarlingVia/Via/ui/inc \
		-IStarlingVia/src/dep/starling-dsp \
		-IStarlingVia/src/dep/starling-rack-ui

$(BUILD_DIR)/stoermelder-packone/%.cpp.o: stoermelder-packone/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(STOERMELDER_PACKONE_CUSTOM),$(call custom_module_names,$(m),stoermelder_p1)) \
		$(foreach m,$(STOERMELDER_PACKONE_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),stoermelder_p1_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__stoermelder_p1 \
		-Dinit=init__stoermelder_p1

$(BUILD_DIR)/stocaudio/%.cpp.o: stocaudio/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(STOCAUDIO_CUSTOM),$(call custom_module_names,$(m),stocaudio)) \
		-DpluginInstance=pluginInstance__stocaudio

$(BUILD_DIR)/surgext%.cpp.o: surgext%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(SURGEXT_FLAGS) -c -o $@ \
		$(foreach m,$(SURGEXT_CUSTOM),$(call custom_module_names,$(m),surgext)) \
		-DpluginInstance=pluginInstance__surgext

$(BUILD_DIR)/unless_modules/%.cpp.o: unless_modules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(UNLESS_MODULES_CUSTOM),$(call custom_module_names,$(m),unless_modules)) \
		-DpluginInstance=pluginInstance__unless_modules

$(BUILD_DIR)/ValleyAudio/%.cpp.o: ValleyAudio/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(VALLEYAUDIO_CUSTOM),$(call custom_module_names,$(m),ValleyAudio)) \
		$(foreach m,$(VALLEYAUDIO_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),ValleyAudio_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__ValleyAudio \
		-DSTDIO_OVERRIDE=ValleyAudio \
		-IValleyAudio/src \
		-Wno-sign-compare \
		-Wno-unused-but-set-variable

$(BUILD_DIR)/voxglitch/%.cpp.o: voxglitch/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(VOXGLITCH_CUSTOM),$(call custom_module_names,$(m),Voxglitch)) \
		$(foreach m,$(VOXGLITCH_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),Voxglitch_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__Voxglitch \
		-DSKIP_MINGW_FORMAT

$(BUILD_DIR)/WhatTheRack/%.cpp.o: WhatTheRack/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(WHATTHERACK_CUSTOM),$(call custom_module_names,$(m),WhatTheRack)) \
		-DpluginInstance=pluginInstance__WhatTheRack

$(BUILD_DIR)/ZamAudio/%.cpp.o: ZamAudio/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ZAMAUDIO_CUSTOM),$(call custom_module_names,$(m),ZamAudio)) \
		-DpluginInstance=pluginInstance__ZamAudio

$(BUILD_DIR)/ZetaCarinaeModules/%.cpp.o: ZetaCarinaeModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ZETACARINAE_CUSTOM),$(call custom_module_names,$(m),ZetaCarinae)) \
		-DpluginInstance=pluginInstance__ZetaCarinaeModules

$(BUILD_DIR)/ZZC/%.cpp.o: ZZC/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ZZC_CUSTOM),$(call custom_module_names,$(m),ZZC)) \
		-DpluginInstance=pluginInstance__ZZC

# --------------------------------------------------------------

-include $(PLUGIN_OBJS:%.o=%.d)

# --------------------------------------------------------------
