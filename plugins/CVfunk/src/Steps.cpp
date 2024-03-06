////////////////////////////////////////////////////////////
//
//   Steps
//
//   written by Cody Geary
//   Copyright 2024, MIT License
//
//   A window comparator with step interval sequencer
//
////////////////////////////////////////////////////////////


#include "plugin.hpp"

struct Steps : Module {

	enum ParamId {
		BIAS_PARAM,
		RANGE_PARAM,
		STEP_PARAM,
		TRIGGER_BUTTON_PARAM,
		RESET_BUTTON_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		COMPARATOR_INPUT,
		BIAS_INPUT,
		RANGE_INPUT,
		INVERT_INPUT,
		STEP_INPUT,
		TRIGGER_INPUT,
		RESET_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		COMPARATOR_UP_OUTPUT,
		COMPARATOR_DN_OUTPUT,
		STEPPER_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		UP_LIGHT,
		DOWN_LIGHT,
		SAMPLE_LIGHT,
		OUT1_LIGHT,
		OUT2_LIGHT,
		OUT3_LIGHT,
		OUT4_LIGHT,
		OUT5_LIGHT,
		OUT6_LIGHT,
		OUT7_LIGHT,
		OUT8_LIGHT,
		OUT9_LIGHT,
		OUT10_LIGHT,
		LIGHTS_LEN
	};

    // Step_mixer constant
     float step_mix=0.0;

    // Schmitt trigger constants
    const float TRIGGER_THRESHOLD_HIGH = 1.0;
    const float TRIGGER_THRESHOLD_LOW = 0.1;
    bool triggerState = false; // Keeps track of trigger state
    bool resetState = false; // Keeps track of reset state

    // Declare previous states as member variables
    bool previousTriggerState = false;
    bool previousResetState = false;


	Steps() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BIAS_PARAM, -5.f, 5.f, 1.f, "Bias");
		configParam(RANGE_PARAM, 0.f, 10.f, 3.f, "Range");
		configParam(STEP_PARAM, -1.f, 1.f, 0.41666666f, "Step Size");
		configInput(COMPARATOR_INPUT, "Comparator IN");
		configInput(BIAS_INPUT, "Bias IN");
		configInput(RANGE_INPUT, "Range IN");
		configInput(INVERT_INPUT, "Invert Gate IN");
		configInput(STEP_INPUT, "Step Size IN");
		configInput(TRIGGER_INPUT, "Trigger IN");
		configInput(RESET_INPUT, "Reset IN");
		configOutput(COMPARATOR_UP_OUTPUT, "Comparator Above OUT");
		configOutput(COMPARATOR_DN_OUTPUT, "Comparator Below OUT");
		configOutput(STEPPER_OUTPUT, "Stepper OUT");
        // Initialize step_mix with the bias value
        step_mix = params[BIAS_PARAM].getValue() + inputs[BIAS_INPUT].getVoltage();
        
       // Initialize variables in the constructor
        previousTriggerState = false;
        previousResetState = false;
        
	}

	 void process(const ProcessArgs& args) override {
		// Read parameters
		float bias = params[BIAS_PARAM].getValue() + inputs[BIAS_INPUT].getVoltage();
		float range = params[RANGE_PARAM].getValue() + inputs[RANGE_INPUT].getVoltage();
		float step = params[STEP_PARAM].getValue() + inputs[STEP_INPUT].getVoltage();

		// Read inputs
		float comparatorInput = 0.0; // Initialize to 0
		if (inputs[COMPARATOR_INPUT].isConnected()) {
			// Read from COMPARATOR_INPUT if cable is connected
			comparatorInput = inputs[COMPARATOR_INPUT].getVoltage();
		} else {
			// Otherwise, normalize to StepMix
			comparatorInput = step_mix;
		}
	
		// Clamp the ranges to reasonable values
		bias = clamp(bias, -10.0f, 10.0f); 
		range = abs(range);
		range = clamp(range, 0.0f, 10.0f); 
		step = clamp(step, -5.0f, 5.0f); 
   
	
		float invertInput = inputs[INVERT_INPUT].getVoltage();
		float correction = 0.0;
		bool invert = invertInput > 0;

		// Apply inversion
		if (invert) {
			step = -step;
		}

		float comparatorOutput = 0.0;

		// Disconnect comparator normalization if input is connected
		if (inputs[COMPARATOR_INPUT].isConnected()) {
			// Comparator stage 
			if (comparatorInput > bias + 0.5 * range) { 
				comparatorOutput = -5.0;
			} else if (comparatorInput < bias - 0.5 * range) { //avoid self-oscillatory behavior with < here 
				comparatorOutput = 5.0;
			} else {
				comparatorOutput = 0.0;
			}
			//disconnect the normal to the stepper
			correction = 0.0;
		} else {
			if (step>=0){  //change the behavior of the bouncing walls depending on step direction
				// Comparator stage
				if (comparatorInput >= bias + 0.5 * range) { 
					comparatorOutput = -5.0;
					correction = -range;
				} else if (comparatorInput < bias - 0.5 * range) { //avoid self-oscillatory behavior with < here 
					comparatorOutput = 5.0;
					correction = range;
				} else {
					comparatorOutput = 0.0;
					correction = 0.0;
				}
			} else if (step<0) {
				// Comparator stage
				if (comparatorInput > bias + 0.5 * range) { //avoid self-oscillatory behavior with < here 
					comparatorOutput = -5.0;
					correction = -range;
				} else if (comparatorInput <= bias - 0.5 * range) { 
					comparatorOutput = 5.0;
					correction = range;
				} else {
					comparatorOutput = 0.0;
					correction = 0.0;
				}
			}
		}

		 // Detect rising signal on TRIGGER_INPUT or manual trigger button press
		bool manualTriggerPressed = params[TRIGGER_BUTTON_PARAM].getValue() > 0.0f;
		bool currentTriggerState = (inputs[TRIGGER_INPUT].isConnected() && inputs[TRIGGER_INPUT].getVoltage() > 1.0f) || manualTriggerPressed;

		// Detect rising signal on RESET_INPUT or manual reset button press
		bool manualResetPressed = params[RESET_BUTTON_PARAM].getValue() > 0.0f;
		bool currentResetState = (inputs[RESET_INPUT].isConnected() && inputs[RESET_INPUT].getVoltage() > 1.0f) || manualResetPressed;

		// Trigger logic
		if (currentTriggerState && !previousTriggerState) {
			// Decide to step or to correct based on 'correction' variable
			if (correction == 0) {
				step_mix += step;  // Increment step_mix by step value
			} else {
				step_mix += correction;  // Adjust step_mix by correction value
			}

			// Clamp step_mix to +/-10V
			step_mix = clamp(step_mix, -10.0f, 10.0f);

			outputs[STEPPER_OUTPUT].setVoltage(step_mix);
			lights[SAMPLE_LIGHT].setSmoothBrightness(1.0, args.sampleTime);
		}

		// Reset logic
		if (currentResetState && !previousResetState) {
			// Reset step_mix to either bias or 0.0 depending on COMPARATOR_INPUT connection
			step_mix = inputs[COMPARATOR_INPUT].isConnected() ? 0.0 : bias;

			outputs[STEPPER_OUTPUT].setVoltage(step_mix);
			lights[SAMPLE_LIGHT].setSmoothBrightness(1.0, args.sampleTime);
		} else if (!currentTriggerState) {
			// Maintain previous value if no trigger or reset
			outputs[STEPPER_OUTPUT].setVoltage(step_mix);
			lights[SAMPLE_LIGHT].setSmoothBrightness(0.0, args.sampleTime);
		}

		// Update previous states at the end of the process cycle
		previousTriggerState = currentTriggerState;
		previousResetState = currentResetState;




		// Comparator Output and Lights
		if (comparatorOutput > 0) {
			outputs[COMPARATOR_UP_OUTPUT].setVoltage(0.0);
			outputs[COMPARATOR_DN_OUTPUT].setVoltage(5.0);
			lights[UP_LIGHT].setSmoothBrightness(0.0, args.sampleTime);
			lights[DOWN_LIGHT].setSmoothBrightness(1.0, args.sampleTime);
		} else if (comparatorOutput < 0) {
			outputs[COMPARATOR_UP_OUTPUT].setVoltage(5.0);
			outputs[COMPARATOR_DN_OUTPUT].setVoltage(0.0);
			lights[UP_LIGHT].setSmoothBrightness(1.0, args.sampleTime);
			lights[DOWN_LIGHT].setSmoothBrightness(0.0, args.sampleTime);			
		} else {
			outputs[COMPARATOR_UP_OUTPUT].setVoltage(0.0);
			outputs[COMPARATOR_DN_OUTPUT].setVoltage(0.0);
			lights[UP_LIGHT].setSmoothBrightness(0.0, args.sampleTime);
			lights[DOWN_LIGHT].setSmoothBrightness(0.0, args.sampleTime);
		}

		// Calculate the LED level based on the voltage, bias, and range
		int led_level = floor(((step_mix - (bias - 0.5 * range)) / range) * 10) ;
 
		// Clamp led_level to be within [0, 10]
		led_level = std::max(std::min(led_level, 10), 0);

		// Iterate over the first 10 lights (OUT1_LIGHT to OUT10_LIGHT)
		for (int i = 0; i < 10; ++i) {
			if (i < led_level){
				 lights[OUT1_LIGHT + i].setSmoothBrightness(1.0, args.sampleTime);
			 } else {        
				 float how_bright = lights[OUT1_LIGHT + i].getBrightness();
				 how_bright = (0.9-0.05*i)*how_bright; //dim the brightness, top lights dim faster than bottom lights for effect
				 lights[OUT1_LIGHT + i].setSmoothBrightness(how_bright, args.sampleTime);         
			 }
		
		}	
	 
	}

};

struct StepsWidget : ModuleWidget {

	StepsWidget(Steps* module) {
		setModule(module);
		
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/Steps.svg"),
			asset::plugin(pluginInstance, "res/Steps-dark.svg")
		));
		
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(12.978, 49.183)), module, Steps::BIAS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(37.219, 49.183)), module, Steps::RANGE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(12.978, 78.965)), module, Steps::STEP_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.3, 28.408)), module, Steps::COMPARATOR_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.545, 28.408)), module, Steps::BIAS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.159, 28.408)), module, Steps::RANGE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.3, 94.974)), module, Steps::INVERT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.3, 112.263)), module, Steps::STEP_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.545, 112.263)), module, Steps::TRIGGER_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.159, 112.263)), module, Steps::RESET_INPUT));

		addParam(createParamCentered<TL1105>(mm2px(Vec(19.545, 105.263)), module, Steps::TRIGGER_BUTTON_PARAM));
		addParam(createParamCentered<TL1105>(mm2px(Vec(32.159, 105.263)), module, Steps::RESET_BUTTON_PARAM));


		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.445, 19.632)), module, Steps::COMPARATOR_UP_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.426, 28.485)), module, Steps::COMPARATOR_DN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.426, 112.263)), module, Steps::STEPPER_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(39.075, 21.719)), module, Steps::UP_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(39.19, 31.283)), module, Steps::DOWN_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.726, 78.466)), module, Steps::SAMPLE_LIGHT));
		
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 106.773)), module, Steps::OUT1_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 103.628)), module, Steps::OUT2_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 100.483)), module, Steps::OUT3_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 97.338)), module, Steps::OUT4_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 94.192)), module, Steps::OUT5_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 91.047)), module, Steps::OUT6_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 87.902)), module, Steps::OUT7_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 84.757)), module, Steps::OUT8_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 81.612)), module, Steps::OUT9_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.923, 78.466)), module, Steps::OUT10_LIGHT));
	}
};


Model* modelSteps = createModel<Steps, StepsWidget>("Steps");