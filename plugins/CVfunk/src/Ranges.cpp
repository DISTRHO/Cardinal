////////////////////////////////////////////////////////////
//
//   Ranges
//
//   written by Cody Geary
//   Copyright 2024, MIT License
//
//   Divides two input sequences into a range of voltages
//
////////////////////////////////////////////////////////////


#include "plugin.hpp"

struct Ranges : Module {
    enum ParamId {
        TOP_PARAM,
        BOTTOM_PARAM,
        TOP_ATT_PARAM,
        BOTTOM_ATT_PARAM,
        DIVISIONS_PARAM,
        NUM_PARAMS
    };
    enum InputId {
        TOP_INPUT,
        BOTTOM_INPUT,
        DIVISIONS_INPUT,
        NUM_INPUTS
    };
    enum OutputId {
        OUT1_OUTPUT, OUT2_OUTPUT, OUT3_OUTPUT,
        OUT4_OUTPUT, OUT5_OUTPUT, OUT6_OUTPUT,
        OUT7_OUTPUT, OUT8_OUTPUT, OUT9_OUTPUT,
        OUT10_OUTPUT, OUT11_OUTPUT, OUT12_OUTPUT,
		OUT13_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightId {
        OUT1_LIGHT, OUT2_LIGHT, OUT3_LIGHT,
        OUT4_LIGHT, OUT5_LIGHT, OUT6_LIGHT,
        OUT7_LIGHT, OUT8_LIGHT, OUT9_LIGHT,
        OUT10_LIGHT, OUT11_LIGHT, OUT12_LIGHT,
		OUT13_LIGHT,
        NUM_LIGHTS
    };

    Ranges() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(TOP_PARAM, -10.f, 10.f, 0.f, "Top");
        configParam(BOTTOM_PARAM, -10.f, 10.f, 0.f, "Bottom");
        configParam(TOP_ATT_PARAM, -1.f, 1.f, 0.f, "Top Attenuation");
        configParam(BOTTOM_ATT_PARAM, -1.f, 1.f, 0.f, "Botom Attenuation");
        configParam(DIVISIONS_PARAM, 0.f, 11.f, 1.f, "Divisions");


		configInput(TOP_INPUT, "Top IN");
		configInput(BOTTOM_INPUT, "Bottom IN");
		configInput(DIVISIONS_INPUT, "Divisions IN");


        // Initialize lights if needed
        for (int i = 0; i < 13; ++i) {
            configLight(OUT1_LIGHT + i, "Output Active Indicator");
        }
    }

    void process(const ProcessArgs& args) override {
        float start = params[TOP_PARAM].getValue() + params[TOP_ATT_PARAM].getValue() * inputs[TOP_INPUT].getVoltage();
        float end = params[BOTTOM_PARAM].getValue() + params[BOTTOM_ATT_PARAM].getValue() * inputs[BOTTOM_INPUT].getVoltage();

        // Clamp the start and end values
        start = clamp(start, -10.f, 10.f);
        end = clamp(end, -10.f, 10.f);

        // Calculate and clamp divisions
        int divisions = 1+static_cast<int>(floor(params[DIVISIONS_PARAM].getValue() + 2.4 * inputs[DIVISIONS_INPUT].getVoltage()));
        divisions = clamp(divisions, 0, 12);

        // Calculate step size
        float step = divisions > 0 ? (end - start) / (divisions) : 0.f;

        // Set outputs and lights
        for (int i = 0; i < 13; ++i) {
            if (i < divisions+1) {
                float voltage = start + step * i;
                outputs[OUT1_OUTPUT + i].setVoltage(voltage);
                lights[OUT1_LIGHT + i].setBrightness(1.f); // Active
            } else {
                outputs[OUT1_OUTPUT + i].setVoltage(0.f);
                lights[OUT1_LIGHT + i].setBrightness(0.f); // Inactive
            }
        }
     }
};

struct RangesWidget : ModuleWidget {
    RangesWidget(Ranges* module) {
        setModule(module);
       setPanel(createPanel(
			asset::plugin(pluginInstance, "res/Ranges.svg"),
			asset::plugin(pluginInstance, "res/Ranges-dark.svg")
		));

        box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        // Left Section
        addParam(createParam<RoundBlackKnob>(mm2px(Vec(5, 12)), module, Ranges::TOP_PARAM));
        addParam(createParam<Trimpot>(mm2px(Vec(7, 24)), module, Ranges::TOP_ATT_PARAM));
        addInput(createInput<PJ301MPort>(mm2px(Vec(6, 32)), module, Ranges::TOP_INPUT));

        addParam(createParam<RoundBlackKnob>(mm2px(Vec(5, 52)), module, Ranges::BOTTOM_PARAM));
        addParam(createParam<Trimpot>(mm2px(Vec(7, 64)), module, Ranges::BOTTOM_ATT_PARAM));
        addInput(createInput<PJ301MPort>(mm2px(Vec(6, 72)), module, Ranges::BOTTOM_INPUT));

        addParam(createParam<RoundBlackKnob>(mm2px(Vec(5, 97)), module, Ranges::DIVISIONS_PARAM)); 
        addInput(createInput<PJ301MPort>(mm2px(Vec(6, 109)), module, Ranges::DIVISIONS_INPUT)); 

        // Right Section - 13 outputs
        for (int i = 0; i < 13; ++i) {
            float yPos = 13 + i * 8; // Reduced spacing between outputs
            addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(23, yPos+3)), module, Ranges::OUT1_LIGHT + i));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(26, yPos)), module, Ranges::OUT1_OUTPUT + i));
        }
      }
};

Model* modelRanges = createModel<Ranges, RangesWidget>("Ranges");
