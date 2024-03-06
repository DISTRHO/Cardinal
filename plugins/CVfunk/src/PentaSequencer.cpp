////////////////////////////////////////////////////////////
//
//   Penta Sequencer
//
//   written by Cody Geary
//   Copyright 2024, MIT License
//
//   A five stage sequencer with five outputs and slew.
//
////////////////////////////////////////////////////////////


#include "plugin.hpp"

struct PentaSequencer : Module {

    // Initialize variables for trigger detection
    dsp::SchmittTrigger resetTrigger;

    // Initialize timer dsps
    dsp::Timer triggerIntervalTimer;

	// Sequencer operation modes
	enum Mode {
		CW_CIRC,
		CCW_CIRC,
		CW_STAR,
		CCW_STAR
	};

    // Mode toggles
    bool circMode = true, starMode = false, cwMode = true, ccwMode = false; // Default modes

	enum ParamId {
		SLEW_PARAM,
		KNOB1_PARAM, KNOB2_PARAM, KNOB3_PARAM, KNOB4_PARAM, KNOB5_PARAM,
		MANUAL_TRIGGER_PARAM,	
		PARAMS_LEN
	};
	enum InputId {
		TRIG_INPUT, SHAPE_INPUT, SHIFT_INPUT, DIR_INPUT, RESET_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		A_OUTPUT, B_OUTPUT, C_OUTPUT, D_OUTPUT, E_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		STEP1_LIGHT, STEP2_LIGHT, STEP3_LIGHT, STEP4_LIGHT, STEP5_LIGHT,

		AA_LIGHT, AB_LIGHT, AC_LIGHT, AD_LIGHT, AE_LIGHT,		
		BA_LIGHT, BB_LIGHT, BC_LIGHT, BD_LIGHT, BE_LIGHT,
		CA_LIGHT, CB_LIGHT, CC_LIGHT, CD_LIGHT, CE_LIGHT,
		DA_LIGHT, DB_LIGHT, DC_LIGHT, DD_LIGHT, DE_LIGHT,
		EA_LIGHT, EB_LIGHT, EC_LIGHT, ED_LIGHT, EE_LIGHT,

		INNERA_LIGHT, INNERB_LIGHT, INNERC_LIGHT, INNERD_LIGHT, INNERE_LIGHT,
		OUTERA_LIGHT, OUTERB_LIGHT, OUTERC_LIGHT, OUTERD_LIGHT, OUTERE_LIGHT,
		LIGHTS_LEN
	};

    // Variables for internal logic
    int step = 0; // Current step in the sequence
    int mode = 0; // Operation mode: 0 = CW_CIRC, 1 = CCW_CIRC, 2 = CW_STAR, 3 = CCW_STAR
    float lastTriggerTime = 0; // Time of the last trigger input
	float triggerInterval = 100;
	float lastTargetVoltages[5] = {0.f, 0.f, 0.f, 0.f, 0.f}; // Initialize with default voltages, assuming start at 0V
    int dimmingCounter = 0;
    const int dimmingRate = 100; // Number of process calls before dimming, adjust for desired timing
	bool previousTriggerState = false;
	int prevMapping[5] = {0, 1, 2, 3, 4};  // Initialize with default mapping or actual initial mapping

	bool onTarget = true;

    dsp::SlewLimiter slewLimiters[5]; // One per output (A-E)

	PentaSequencer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SLEW_PARAM, 0.f, 1.f, 0.f, "Slew"); 
		configParam(KNOB1_PARAM, -5.f, 5.f, 0.f, "I");
		configParam(KNOB2_PARAM, -5.f, 5.f, 0.f, "II");
		configParam(KNOB3_PARAM, -5.f, 5.f, 0.f, "III");
		configParam(KNOB4_PARAM, -5.f, 5.f, 0.f, "IV");
		configParam(KNOB5_PARAM, -5.f, 5.f, 0.f, "V");
		configInput(TRIG_INPUT, "Trigger IN");
		configInput(SHAPE_INPUT, "Shape IN");
		configInput(SHIFT_INPUT, "Shift IN");
		configInput(DIR_INPUT, "Dir IN");
		configInput(RESET_INPUT, "Reset IN"); 
		configOutput(A_OUTPUT, "A");
		configOutput(B_OUTPUT, "B");
		configOutput(C_OUTPUT, "C");
		configOutput(D_OUTPUT, "D");
		configOutput(E_OUTPUT, "E");

	}

	void process(const ProcessArgs& args) override {
	
		// Accumulate time in the timer
		triggerIntervalTimer.process(args.sampleTime);

		// Read knob values and update outputs
		float knobValues[5] = {
			params[KNOB1_PARAM].getValue(),
			params[KNOB2_PARAM].getValue(),
			params[KNOB3_PARAM].getValue(),
			params[KNOB4_PARAM].getValue(),
			params[KNOB5_PARAM].getValue()
		};


		// Process reset input
		if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
			step = 0;  // Reset to the first step
			
			// Measure the trigger interval here
			triggerInterval = triggerIntervalTimer.time;  // Get the accumulated time since the last reset
			triggerIntervalTimer.reset();  // Reset the timer for the next trigger interval measurement

		}

	
		// Handle CIRC and STAR modes based on SHAPE_INPUT voltage
		if (inputs[SHAPE_INPUT].getVoltage() > 1.0f) {
			// Voltage > 1.0f indicates STAR mode
			starMode = true;
			circMode = false;
		} else {
			// Voltage 0.0f indicates CIRC mode
			circMode = true;
			starMode = false;
		}

		// Handle CW and CCW modes based on DIR_INPUT voltage
		if (inputs[DIR_INPUT].getVoltage() > 1.0f) {
			// Voltage > 1.0f indicates CCW mode
			ccwMode = true;
			cwMode = false;
		} else {
			// Voltage 0.0f indicates CW mode
			cwMode = true;
			ccwMode = false;
		}


        // Define Knob to Output maps
		//                      A  B  C  D  E
		int CIRC_CW_map[5]  =  {0, 1, 2, 3, 4};
		int STAR_CW_map[5]  =  {0, 3, 1, 4, 2}; 
		int CIRC_CCW_map[5] =  {0, 4, 3, 2, 1};
		int STAR_CCW_map[5] =  {0, 2, 4, 1, 3}; 


		int* currentMapping;
        int* newMapping = nullptr;  // Pointer to hold the new mapping based on the current mode

		// Determine the new mapping based on the mode
		if (cwMode && circMode) {
			newMapping = CIRC_CW_map;
		} else if (cwMode && starMode) {
			newMapping = STAR_CW_map;
		} else if (ccwMode && circMode) {
			newMapping = CIRC_CCW_map;
		} else if (ccwMode && starMode) {
			newMapping = STAR_CCW_map;
		}

		// Check if the mapping has changed
		bool mappingChanged = false;
		for (int i = 0; i < 5; ++i) {
			if (newMapping[i] != prevMapping[i]) {
				mappingChanged = true;
				break;  // No need to continue if a change is found
			}
		}

		// Update currentMapping and prevMapping if there's a change
		if (mappingChanged) {
			currentMapping = newMapping;  // Update the current mapping
			for (int i = 0; i < 5; ++i) {
				prevMapping[i] = newMapping[i];  // Update prevMapping for the next comparison
			}

			// Reset the timer due to mapping change
			triggerInterval = triggerIntervalTimer.time;  // Get the accumulated time since the last reset
			triggerIntervalTimer.reset();  // Reset the timer for the next trigger interval measurement
		}
	
		
		// Detect a rising signal on TRIG_INPUT or manual trigger button press
		bool manualTriggerPressed = params[MANUAL_TRIGGER_PARAM].getValue() > 0.0f;
		bool currentTriggerState = (inputs[TRIG_INPUT].isConnected() && inputs[TRIG_INPUT].getVoltage() > 1.0f) || manualTriggerPressed;  // Include manual trigger


		if (currentTriggerState && !previousTriggerState) {
			   for (int i = 0; i < 5; ++i) {
					 // Update the last target voltage for the next cycle			
					lastTargetVoltages[i] =  knobValues[currentMapping[(step + i) % 5]];
				}

				step = (step + 1) % 5 ;  // Increment step by 1 for CCW

				// Measure the trigger interval here
				triggerInterval = triggerIntervalTimer.time;  // Get the accumulated time since the last reset
				triggerIntervalTimer.reset();  // Reset the timer for the next trigger interval measurement
		}

		// Update previousTriggerState at the end of the process cycle
		previousTriggerState = currentTriggerState;


		// Map the knobs and set output voltages with dynamic slew limiting based on trigger interval
		int knobMapping[5];
		for (int i = 0; i < 5; ++i) {
	
			int knobIndex;
				// For CW mode, use the standard incrementing mapping
				knobIndex = currentMapping[(step + i) % 5];
			if (cwMode) {
			} else {
				// For CCW mode, adjust the mapping to decrement
				knobIndex = currentMapping[(step + i) % 5]; // Adjust index for CCW rotation
			}
			float targetVoltage = knobValues[knobIndex];

			knobMapping[i] = knobIndex;  // Store the knob mapping for later use

			targetVoltage = knobValues[knobIndex];
			float slewRate = params[SLEW_PARAM].getValue(); // This gives a value between 0 and 1


			// Calculate the absolute voltage difference from the last target
			float voltageDifference = fabs(targetVoltage - lastTargetVoltages[i]);


			// Adjust slewSpeed based on the voltage difference and trigger interval
			// Ensure triggerInterval is non-zero to avoid division by zero
			float adjustedTriggerInterval = fmax(triggerInterval, 1e-6f);
			float slewSpeed = voltageDifference / adjustedTriggerInterval; // Voltage difference per second

			// Apply the SLEW_PARAM knob to scale the slewSpeed, adding 1e-6 to avoid division by zero
			slewSpeed *= 1.0f / (slewRate + 1e-6f);

			// Set the rise and fall speeds of the slew limiter to the calculated slew speed
			slewLimiters[i].setRiseFall(slewSpeed, slewSpeed);

			// Process the target voltage through the slew limiter
			float slewedVoltage = slewLimiters[i].process(args.sampleTime, targetVoltage);

			//Transpose the output voltage by the transpose input
			float transpose = inputs[SHIFT_INPUT].getVoltage(); 		
			float outputVoltage = slewedVoltage+transpose;
		
			//Clamp the output voltage to normal range
			outputVoltage = clamp(outputVoltage, -10.0f, 10.0f);

			// Set the output voltage to the slewed voltage
			outputs[A_OUTPUT + i].setVoltage(outputVoltage);
		}

		
		// Increment dimming counter and check if it's time to dim the lights
		if (++dimmingCounter >= dimmingRate) {
			for (int i = 0; i < LIGHTS_LEN; ++i) {
				float currentBrightness = lights[i].getBrightness();
				// Apply a less aggressive dimming factor for inner and outer lights
				if (i == INNERA_LIGHT || i == INNERB_LIGHT || i == INNERC_LIGHT || i == INNERD_LIGHT || i == INNERE_LIGHT ||
					i == OUTERA_LIGHT || i == OUTERB_LIGHT || i == OUTERC_LIGHT || i == OUTERD_LIGHT || i == OUTERE_LIGHT) {
					lights[i].setBrightness(currentBrightness * 0.98f); // Slower dimming for inner and outer lights
				} else {
					// More aggressive dimming for the rest
					lights[i].setBrightness(currentBrightness * 0.90f);
				}
			}
			dimmingCounter = 0; // Reset counter after dimming
		}


		// Update output group lights logic and step lights...
		for (int output = 0; output < 5; ++output) {
			int knob = knobMapping[output]; // The current knob mapped to this output

			// Update the output group lights
			for (int subLight = 0; subLight <= knob; ++subLight) {
				// Boost the brightness if not currently in a dimming cycle
				if (dimmingCounter == 0) {
					lights[AA_LIGHT + output * 5 + subLight].setBrightness(1.0f);
				}
			}

			// Update the step lights to indicate which knob corresponds to Node A
			if (output == 0) { // Node A corresponds to the first output
				for (int i = 0; i < 5; ++i) {
					if (i == knob) {
						// Light up the step light under the knob controlling Node A
						lights[STEP1_LIGHT + i].setBrightness(1.0f);
					} else {
						// Turn off other step lights
						lights[STEP1_LIGHT + i].setBrightness(0.0f);
					}
				}
			}
		}


		// Inner Lights for STAR Track Movements
		if (starMode) {
			switch (step) {
				case 0: //E->A->B
					lights[INNERA_LIGHT].setBrightness(1.0f);
					lights[INNERE_LIGHT].setBrightness(1.0f);
					break;
				case 1: // E->A->B
					lights[INNERE_LIGHT].setBrightness(1.0f);
					lights[INNERD_LIGHT].setBrightness(1.0f);
					break;
				case 2: // C->D->E
					lights[INNERD_LIGHT].setBrightness(1.0f);
					lights[INNERC_LIGHT].setBrightness(1.0f);
					break;
				case 3: // B->C->D
					lights[INNERC_LIGHT].setBrightness(1.0f);
					lights[INNERB_LIGHT].setBrightness(1.0f);
					break;
				case 4: //A->B->C
					lights[INNERB_LIGHT].setBrightness(1.0f);
					lights[INNERA_LIGHT].setBrightness(1.0f);
					break;
			}
		}

		// Outer Lights for CIRC Track Movements
		if (circMode) {
			switch (step) {
				case 0: //E->A->B
					lights[OUTERA_LIGHT].setBrightness(1.0f);
					lights[OUTERE_LIGHT].setBrightness(1.0f);
					break;
				case 1: // E->A->B
					lights[OUTERE_LIGHT].setBrightness(1.0f);
					lights[OUTERD_LIGHT].setBrightness(1.0f);
					break;
				case 2: // C->D->E
					lights[OUTERD_LIGHT].setBrightness(1.0f);
					lights[OUTERC_LIGHT].setBrightness(1.0f);
					break;
				case 3: // B->C->D
					lights[OUTERC_LIGHT].setBrightness(1.0f);
					lights[OUTERB_LIGHT].setBrightness(1.0f);
					break;
				case 4: //A->B->C
					lights[OUTERB_LIGHT].setBrightness(1.0f);
					lights[OUTERA_LIGHT].setBrightness(1.0f);
					break;
			}
		}		
		
			
	}//void
};//module


struct PentaSequencerWidget : ModuleWidget {
	PentaSequencerWidget(PentaSequencer* module) {
		setModule(module);

		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/PentaSequencer.svg"),
			asset::plugin(pluginInstance, "res/PentaSequencer-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.001, 44.06)), module, PentaSequencer::SLEW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(9.281, 77.271)), module, PentaSequencer::KNOB3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.23, 92.394)), module, PentaSequencer::KNOB2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.213, 96.263)), module, PentaSequencer::KNOB1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(56.197, 92.394)), module, PentaSequencer::KNOB5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(67.146, 77.271)), module, PentaSequencer::KNOB4_PARAM));

		addParam(createParamCentered<TL1105>(mm2px(Vec(7.235, 105)), module, PentaSequencer::MANUAL_TRIGGER_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.235, 112.373)), module, PentaSequencer::TRIG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.67, 112.373)), module, PentaSequencer::SHAPE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.105, 112.373)), module, PentaSequencer::SHIFT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.54, 112.373)), module, PentaSequencer::DIR_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(68.975, 112.373)), module, PentaSequencer::RESET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.287, 70.309)), module, PentaSequencer::A_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.478, 52.214)), module, PentaSequencer::B_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.639, 23.158)), module, PentaSequencer::C_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.652, 23.333)), module, PentaSequencer::D_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.813, 52.274)), module, PentaSequencer::E_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.143, 84.666)), module, PentaSequencer::STEP3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.172, 99.422)), module, PentaSequencer::STEP2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(38.379, 103.301)), module, PentaSequencer::STEP1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(56.476, 99.422)), module, PentaSequencer::STEP5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(67.423, 84.336)), module, PentaSequencer::STEP4_LIGHT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(38.287, 77.713)), module, PentaSequencer::AA_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(31.262, 72.607)), module, PentaSequencer::AB_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(33.938, 64.355)), module, PentaSequencer::AC_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(42.636, 64.305)), module, PentaSequencer::AD_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(45.294, 72.56)), module, PentaSequencer::AE_LIGHT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(13.478, 59.618)), module, PentaSequencer::BA_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(6.454, 54.512)), module, PentaSequencer::BB_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(9.129, 46.261)), module, PentaSequencer::BC_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(17.827, 46.211)), module, PentaSequencer::BD_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(20.485, 54.466)), module, PentaSequencer::BE_LIGHT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(22.639, 30.563)), module, PentaSequencer::CA_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(15.614, 25.457)), module, PentaSequencer::CB_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(18.29, 17.205)), module, PentaSequencer::CC_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(26.987, 17.156)), module, PentaSequencer::CD_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(29.645, 25.41)), module, PentaSequencer::CE_LIGHT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(53.652, 30.737)), module, PentaSequencer::DA_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(46.628, 25.631)), module, PentaSequencer::DB_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(49.304, 17.38)), module, PentaSequencer::DC_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(58.001, 17.33)), module, PentaSequencer::DD_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(60.66, 25.585)), module, PentaSequencer::DE_LIGHT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(62.813, 59.679)), module, PentaSequencer::EA_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(55.788, 54.573)), module, PentaSequencer::EB_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(58.464, 46.321)), module, PentaSequencer::EC_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(67.162, 46.272)), module, PentaSequencer::ED_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(69.82, 54.526)), module, PentaSequencer::EE_LIGHT));

		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(30.438, 54.683)), module, PentaSequencer::INNERA_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(25.619, 40.522)), module, PentaSequencer::INNERB_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(38.101, 31.358)), module, PentaSequencer::INNERC_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(50.999, 40.762)), module, PentaSequencer::INNERD_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(46.124, 54.783)), module, PentaSequencer::INNERE_LIGHT));

		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(21.274, 67.501)), module, PentaSequencer::OUTERA_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(10.548, 35.306)), module, PentaSequencer::OUTERB_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(38.201, 14.859)), module, PentaSequencer::OUTERC_LIGHT));		
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(65.979, 36.066)), module, PentaSequencer::OUTERD_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(55.34, 67.448)), module, PentaSequencer::OUTERE_LIGHT));
	}
};

Model* modelPentaSequencer = createModel<PentaSequencer, PentaSequencerWidget>("PentaSequencer");