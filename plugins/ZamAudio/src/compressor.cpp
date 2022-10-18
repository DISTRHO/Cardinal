/*
 * ZamComp mono compressor for Cardinal
 * Copyright (C) 2014-2019 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2022 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#include "plugin.hpp"
#include "widgets.hpp"

// --------------------------------------------------------------------------------------------------------------------

struct ZamAudioCompModule : Module {
    enum ParamIds {
        PARAM_ATTACK,
        PARAM_RELEASE,
        PARAM_THRESHOLD,
        PARAM_RATIO,
        PARAM_KNEE,
        PARAM_SLEW,
        PARAM_MAKEUP,
        NUM_PARAMS
    };
    enum InputIds {
        AUDIO_INPUT,
        SIDECHAIN_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        AUDIO_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        OUTLEVEL,
        GAINREDUCTION,
        NUM_LIGHTS
    };

    float gainred,outlevel; //lights
    float oldL_yl, oldL_y1, oldL_yg; //temp

	static inline float
	sanitize_denormal(float v) {
	        if(!std::isnormal(v))
	                return 0.f;
	        return v;
	}

	static inline float
	from_dB(float gdb) {
	        return (expf(0.05f*gdb*logf(10.f)));
	}

	static inline float
	to_dB(float g) {
	        return (20.f*log10f(g));
	}

    ZamAudioCompModule()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(PARAM_ATTACK, 0.1f, 100.f, 10.f, "Attack", " ms");
        configParam(PARAM_RELEASE, 1.f, 500.f, 80.f, "Release", " ms");
        configParam(PARAM_THRESHOLD, -80.f, 0.f, 0.f, "Threshold", " db");
        configParam(PARAM_RATIO, 1.f, 20.f, 4.f, "Ratio");
        configParam(PARAM_KNEE, 0.f, 8.f, 0.f, "Knee", " dB");
        configParam(PARAM_SLEW, 1.f, 150.f, 1.f, "Slew");
        configParam(PARAM_MAKEUP, 0.f, 30.f, 0.f, "Makeup", " dB");
        configInput(AUDIO_INPUT, "Audio");
        configInput(SIDECHAIN_INPUT, "Sidechain");
        configOutput(AUDIO_OUTPUT, "Audio");
        configLight(OUTLEVEL, "Output Level");
        configLight(GAINREDUCTION, "Gain Reduction");

		gainred = 0.0f;
		outlevel = -45.0f;
        oldL_yl = oldL_y1 = oldL_yg = 0.f;
    }

    void process(const ProcessArgs& args) override
    {
        const float attack     = params[PARAM_ATTACK].getValue();
        const float release    = params[PARAM_RELEASE].getValue();
        const float thresdb    = params[PARAM_THRESHOLD].getValue();
        const float ratio      = params[PARAM_RATIO].getValue();
        const float knee       = params[PARAM_KNEE].getValue();
        const float slewfactor = params[PARAM_SLEW].getValue();
        const float makeup     = params[PARAM_MAKEUP].getValue();

	    const float srate = args.sampleRate;
	    const float width = (6.f * knee) + 0.01;
	    const float slewwidth = 1.8f;
        const float release_coeff = exp(-1000.f/(release * srate));

        // const float gain = std::pow(params[0].getValue(), 2.f);

        const float in0 = inputs[AUDIO_INPUT].getVoltageSum() * 0.1f;
		const float in = inputs[SIDECHAIN_INPUT].isConnected()
                       ? inputs[SIDECHAIN_INPUT].getVoltageSum() * 0.1f
                       : in0;

		const float Lxg = sanitize_denormal(in == 0.f ? -160.f : to_dB(fabsf(in)));

        const float checkwidth = 2.f*fabsf(Lxg-thresdb);

        bool attslew = false;
        float Lyg;
        if (2.f*(Lxg-thresdb) < -width) {
            Lyg = Lxg;
        } else if (checkwidth <= width) {
			Lyg = thresdb + (Lxg-thresdb)/ratio;
			Lyg = sanitize_denormal(Lyg);
			if (checkwidth <= slewwidth) {
				if (Lyg >= oldL_yg) {
					attslew = true;
				}
			}
        } else if (2.f*(Lxg-thresdb) > width) {
            Lyg = thresdb + (Lxg-thresdb)/ratio;
            Lyg = sanitize_denormal(Lyg);
        } else {
            Lyg = Lxg + (1.f/ratio-1.f)*(Lxg-thresdb+width/2.f)*(Lxg-thresdb+width/2.f)/(2.f*width);
        }

        const float attack_coeff = attslew
                                 ? exp(-1000.f/((attack + 2.0*(slewfactor - 1)) * srate))
                                 : exp(-1000.f/(attack * srate));
        // Don't slew on release

        const float Lxl = Lxg - Lyg;

        const float Lyl = sanitize_denormal(Lxl < oldL_yl ? release_coeff * oldL_yl + (1.f-release_coeff)*Lxl
                                                             : Lxl > oldL_yl ? attack_coeff * oldL_yl+(1.f-attack_coeff)*Lxl : Lxl);

        const float Lgain = from_dB(-Lyl);

		const float out = in0 * Lgain * from_dB(makeup);
        outputs[AUDIO_OUTPUT].setVoltage(out * 10.0f);

        oldL_yl = Lyl;
        oldL_yg = Lyg;

        gainred = Lyl;
	    // const float max = (fabsf(out) > max) ? fabsf(outputs[0][i]) : sanitize_denormal(max);
	    // outlevel = (max == 0.f) ? -45.f : to_dB(max); // relative to - thresdb;
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct ZamAudioCompModuleWidget : ZamAudioModuleWidget {
	typedef FundamentalBlackKnob<36> BigKnob;

    ZamAudioCompModule* const module;

    ZamAudioCompModuleWidget(ZamAudioCompModule* const m)
        : ZamAudioModuleWidget(),
          module(m)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ZamComp.svg")));

        addInput(createInput<PJ301MPort>(Vec(32, 280), m, ZamAudioCompModule::AUDIO_INPUT));
        addInput(createInput<PJ301MPort>(Vec(32, 310), m, ZamAudioCompModule::SIDECHAIN_INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(100, 310), m, ZamAudioCompModule::AUDIO_OUTPUT));

        const float scale = 0.8f;
		addParam(createParamCentered<BigKnob>(Vec(45.75f * scale, 121.25f * scale), m, ZamAudioCompModule::PARAM_ATTACK));
		addParam(createParamCentered<BigKnob>(Vec(127.75f * scale, 121.25f * scale), m, ZamAudioCompModule::PARAM_RELEASE));

		addParam(createParamCentered<BigKnob>(Vec(48.25f * scale, 208.f * scale), m, ZamAudioCompModule::PARAM_THRESHOLD));
		addParam(createParamCentered<BigKnob>(Vec(130.75f * scale, 208.f * scale), m, ZamAudioCompModule::PARAM_RATIO));

		addParam(createParamCentered<BigKnob>(Vec(48.25f * scale, 305.f * scale), m, ZamAudioCompModule::PARAM_KNEE));
		addParam(createParamCentered<BigKnob>(Vec(130.75f * scale, 305.f * scale), m, ZamAudioCompModule::PARAM_SLEW));
    }
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelZamComp = createModel<ZamAudioCompModule, ZamAudioCompModuleWidget>("ZamComp");

// --------------------------------------------------------------------------------------------------------------------
