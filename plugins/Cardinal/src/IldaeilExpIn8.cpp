//Expander module for Ildaeil
//
//Based on code from GateSeq64 and GateSeq64Expander by Marc Boulé
//Adapted for Ildaeil by Simon-L

#include "plugin.hpp"

static const unsigned int ildaeil_expanderRefreshStepSkips = 4;

struct IldaeilExpIn8 : Module {
	enum InputIds {
		PARAM1_INPUT,
		PARAM2_INPUT,
		PARAM3_INPUT,
		PARAM4_INPUT,
		PARAM5_INPUT,
		PARAM6_INPUT,
		PARAM7_INPUT,
		PARAM8_INPUT,
		NUM_INPUTS
	};

	// Expander
	float rightMessages[2][2] = {};// messages from Ildaeil

	unsigned int expanderRefreshCounter = 0;	

	IldaeilExpIn8() {
		config(0, NUM_INPUTS, 0, 0);
		
		rightExpander.producerMessage = rightMessages[0];
		rightExpander.consumerMessage = rightMessages[1];
		
		configInput(PARAM1_INPUT, "Parameter 1");
		configInput(PARAM2_INPUT, "Parameter 2");
		configInput(PARAM3_INPUT, "Parameter 3");
		configInput(PARAM4_INPUT, "Parameter 4");
		configInput(PARAM5_INPUT, "Parameter 5");
		configInput(PARAM6_INPUT, "Parameter 6");
		configInput(PARAM7_INPUT, "Parameter 7");
		configInput(PARAM8_INPUT, "Parameter 8");

	}


	void process(const ProcessArgs &args) override {		
		bool ildaeilPresent = (rightExpander.module && rightExpander.module->model == modelIldaeil);
		if (ildaeilPresent) {
			float *messagesToIldaeil = (float*)rightExpander.module->leftExpander.producerMessage;
			for (int i = 0; i < NUM_INPUTS; i++) {
				messagesToIldaeil[i] = (inputs[i].isConnected() ? inputs[i].getVoltage() : 0.0);
			}
			rightExpander.module->leftExpander.messageFlipRequested = true;
		}		
	}// process()
};


struct IldaeilExpIn8Widget : ModuleWidget {
	IldaeilExpIn8Widget(IldaeilExpIn8 *module) {
		setModule(module);
	
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/IldaeilExpIn8.svg")));
        box.size = Vec(RACK_GRID_WIDTH * 2, RACK_GRID_HEIGHT);

        // Screws
        addChild(createWidget<ScrewBlack>(Vec(0, 0)));
        addChild(createWidget<ScrewBlack>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // Inputs
		addInput(createInput<PJ301MPort>(Vec(3, 54 + 75), module, IldaeilExpIn8::PARAM1_INPUT));
		addInput(createInput<PJ301MPort>(Vec(3, 54 + 105), module, IldaeilExpIn8::PARAM2_INPUT));
		addInput(createInput<PJ301MPort>(Vec(3, 54 + 135), module, IldaeilExpIn8::PARAM3_INPUT));
        addInput(createInput<PJ301MPort>(Vec(3, 54 + 165), module, IldaeilExpIn8::PARAM4_INPUT));
        addInput(createInput<PJ301MPort>(Vec(3, 54 + 195), module, IldaeilExpIn8::PARAM5_INPUT));
        addInput(createInput<PJ301MPort>(Vec(3, 54 + 225), module, IldaeilExpIn8::PARAM6_INPUT));
        addInput(createInput<PJ301MPort>(Vec(3, 54 + 255), module, IldaeilExpIn8::PARAM7_INPUT));
        addInput(createInput<PJ301MPort>(Vec(3, 54 + 285), module, IldaeilExpIn8::PARAM8_INPUT));
	}

};

Model *modelIldaeilExpIn8 = createModel<IldaeilExpIn8, IldaeilExpIn8Widget>("IldaeilExpIn8");
