////////////////////////////////////////////////////////////
//
//   Impulse Controller
//
//   written by Cody Geary
//   Copyright 2024, MIT License
//
//   Decay envelopes propagate across a network of 24 nodes
//
////////////////////////////////////////////////////////////

#include "plugin.hpp"

struct ImpulseController : Module {
	enum ParamId {
		LAG_PARAM,
		DECAY_PARAM,
		SPREAD_PARAM,
		LAG_ATT_PARAM,     // Attenuverter for LAG_PARAM
        SPREAD_ATT_PARAM,  // Attenuverter for DECAY_PARAM
        DECAY_ATT_PARAM,   // Attenuverter for SPREAD_PARAM
        TRIGGER_BUTTON,
		PARAMS_LEN
	};
	enum InputId {
		_00_INPUT,
		LAG_INPUT,
		DECAY_INPUT,
		SPREAD_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		_01_OUTPUT, _02_OUTPUT, _03_OUTPUT, _04_OUTPUT,
		_05_OUTPUT, _06_OUTPUT, _07_OUTPUT, _08_OUTPUT,
		_09_OUTPUT, _10_OUTPUT, _11_OUTPUT, _12_OUTPUT,
		_13_OUTPUT, _14_OUTPUT, _15_OUTPUT, _16_OUTPUT,
		_17_OUTPUT, _18_OUTPUT, _19_OUTPUT, _20_OUTPUT,
		_21_OUTPUT, _22_OUTPUT, _23_OUTPUT, _24_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		_00A_LIGHT, _00B_LIGHT,
		_01A_LIGHT, _01B_LIGHT,
		_02A_LIGHT, _02B_LIGHT, _02C_LIGHT, _02D_LIGHT, _02E_LIGHT,
		_03A_LIGHT, _03B_LIGHT, _03C_LIGHT, _03D_LIGHT, _03E_LIGHT,
		_04A_LIGHT, _04B_LIGHT, _04C_LIGHT,
		_05A_LIGHT, _05B_LIGHT, _05C_LIGHT, _05D_LIGHT, _05E_LIGHT,
		_06A_LIGHT, _06B_LIGHT, _06C_LIGHT, _06D_LIGHT, _06E_LIGHT,
		_07A_LIGHT, _07B_LIGHT, _07C_LIGHT,
		_08A_LIGHT, _08B_LIGHT, _08C_LIGHT, _08D_LIGHT, _08E_LIGHT,
		_09A_LIGHT, _09B_LIGHT, _09C_LIGHT, _09D_LIGHT, _09E_LIGHT,
		_10A_LIGHT, _10B_LIGHT,
		_11A_LIGHT, _11B_LIGHT,
		_12A_LIGHT, _12B_LIGHT, _12C_LIGHT, _12D_LIGHT, _12E_LIGHT,
		_13A_LIGHT, _13B_LIGHT, _13C_LIGHT, _13D_LIGHT, _13E_LIGHT,
		_14A_LIGHT, _14B_LIGHT,
		_15A_LIGHT, _15B_LIGHT,
		_00OUT_LIGHT,
		_01OUT_LIGHT, _02OUT_LIGHT, _03OUT_LIGHT, _04OUT_LIGHT,
		_05OUT_LIGHT, _06OUT_LIGHT, _07OUT_LIGHT, _08OUT_LIGHT,
		_09OUT_LIGHT, _10OUT_LIGHT, _11OUT_LIGHT, _12OUT_LIGHT,
		_13OUT_LIGHT, _14OUT_LIGHT, _15OUT_LIGHT, _16OUT_LIGHT,
		_17OUT_LIGHT, _18OUT_LIGHT, _19OUT_LIGHT, _20OUT_LIGHT,
		_21OUT_LIGHT, _22OUT_LIGHT, _23OUT_LIGHT, _24OUT_LIGHT,
		LIGHTS_LEN
	};

    // Define the maximum number of nodes
    static constexpr int MAX_NODES = 24;
	
	// Define an array to store time variables
    float lag[24] = {0.0f}; // Time interval for each light group
    float groupElapsedTime[24] = {}; // Elapsed time since the last activation for each light group

    float accumulatedTime = 0.0f; // Accumulator for elapsed time, now an instance variable


    // Boolean array for active nodes management
    bool activeNodes[MAX_NODES] = {};

	//Keep track of input states so that we can avoid retriggering on gates
    bool previousInputState = false; 

	//Keep track of the time the one input is above the threshold
	float inputAboveThresholdTime = 0.0f; // Time in seconds

	// Define groups of lights
	const std::vector<std::vector<LightId>> lightGroups = {
		{_01OUT_LIGHT, _00A_LIGHT, _00B_LIGHT, _01A_LIGHT},
		{_02OUT_LIGHT, _01B_LIGHT, _02A_LIGHT, _02C_LIGHT, _02D_LIGHT},
		{_03OUT_LIGHT, _02B_LIGHT, _03A_LIGHT, _03C_LIGHT, _03D_LIGHT},	
		{_04OUT_LIGHT, _02E_LIGHT, _04A_LIGHT, _04B_LIGHT},	 
		{_05OUT_LIGHT, _03B_LIGHT, _05A_LIGHT, _05C_LIGHT, _05D_LIGHT},
		{_06OUT_LIGHT, _04C_LIGHT, _06A_LIGHT, _06C_LIGHT, _06D_LIGHT},		
		{_07OUT_LIGHT, _03E_LIGHT, _07A_LIGHT, _07B_LIGHT},		
		{_08OUT_LIGHT, _05B_LIGHT, _08A_LIGHT, _08C_LIGHT, _08D_LIGHT},		
		{_09OUT_LIGHT, _06B_LIGHT, _09A_LIGHT, _09C_LIGHT, _09D_LIGHT},
		{_10OUT_LIGHT, _07C_LIGHT, _10A_LIGHT},
		{_11OUT_LIGHT, _05E_LIGHT, _11A_LIGHT},
		{_12OUT_LIGHT, _08E_LIGHT, _12A_LIGHT, _12C_LIGHT, _12D_LIGHT},
		{_13OUT_LIGHT, _08B_LIGHT, _13A_LIGHT, _13C_LIGHT, _13D_LIGHT},
		{_14OUT_LIGHT, _09E_LIGHT, _14A_LIGHT}, 
		{_15OUT_LIGHT, _09B_LIGHT, _15A_LIGHT},
		{_16OUT_LIGHT, _11B_LIGHT},
		{_17OUT_LIGHT, _10B_LIGHT},
		{_18OUT_LIGHT, _13E_LIGHT},
		{_19OUT_LIGHT, _06E_LIGHT},
		{_20OUT_LIGHT, _12B_LIGHT},
		{_21OUT_LIGHT, _13B_LIGHT},
		{_22OUT_LIGHT, _12E_LIGHT},
		{_23OUT_LIGHT, _14B_LIGHT},
		{_24OUT_LIGHT, _15B_LIGHT},
	};

	//Define the node-connected graph structure		
	const std::map<int, std::vector<int>> nodeConnections = {
		{0, {1}},
		{1, {2, 3}},
		{2, {4, 6}},
		{3, {5}},
		{4, {7, 10}},
		{5, {8, 18}},
		{6, {9}},
		{7, {11, 12}},
		{8, {13, 14}},
		{9, {16}},
		{10, {15}},
		{11, {19, 21}},
		{12, {17, 20}},
		{13, {22}},
		{14, {23}}
	};
	
	ImpulseController() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(LAG_PARAM, 0.0f, 1.0f, 0.1f, "Lag");
		configParam(SPREAD_PARAM, -1.0f, 1.f, 0.8f, "Spread");
		configParam(DECAY_PARAM, 0.0f, 1.0f, 0.6f, "Decay");
		
		configParam(LAG_ATT_PARAM, -1.0f, 1.0f, 0.5f, "Lag Attenuverter");
		configParam(SPREAD_ATT_PARAM, -1.0f, 1.0f, 0.5f, "Spread Attenuverter");
		configParam(DECAY_ATT_PARAM, -1.0f, 1.0f, 0.5f, "Decay Attenuverter");

		configInput(_00_INPUT, "IN");
		configInput(LAG_INPUT, "Lag");
		configInput(SPREAD_INPUT, "Spread");
		configInput(DECAY_INPUT, "Decay");
		configOutput(_01_OUTPUT, ""); configOutput(_02_OUTPUT, "");
		configOutput(_03_OUTPUT, ""); configOutput(_04_OUTPUT, "");
		configOutput(_05_OUTPUT, ""); configOutput(_06_OUTPUT, "");
		configOutput(_07_OUTPUT, ""); configOutput(_08_OUTPUT, "");
		configOutput(_09_OUTPUT, ""); configOutput(_10_OUTPUT, "");
		configOutput(_11_OUTPUT, ""); configOutput(_12_OUTPUT, "");
		configOutput(_13_OUTPUT, ""); configOutput(_14_OUTPUT, "");
		configOutput(_15_OUTPUT, ""); configOutput(_16_OUTPUT, "");
		configOutput(_17_OUTPUT, ""); configOutput(_18_OUTPUT, "");
		configOutput(_19_OUTPUT, ""); configOutput(_20_OUTPUT, "");
		configOutput(_21_OUTPUT, ""); configOutput(_22_OUTPUT, "");
		configOutput(_23_OUTPUT, ""); configOutput(_24_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
		const float baseSampleTime = 2.0f / 44100.0f; // Base sample time for 44.1 kHz  //cut the CPU by 50% 

		// Accumulate elapsed time
		accumulatedTime += args.sampleTime;

		// Only update at an equivalent frequency of 44.1 kHz
		if (accumulatedTime >= baseSampleTime) {
			//Process inputs to paramaters
			float decay = params[DECAY_PARAM].getValue();
			float spread = params[SPREAD_PARAM].getValue();
		 
			lag[0] = params[LAG_PARAM].getValue(); //Time interval for the first generator

			if (inputs[LAG_INPUT].isConnected())
				lag[0] += inputs[LAG_INPUT].getVoltage()*0.2*params[LAG_ATT_PARAM].getValue();
			if (inputs[SPREAD_INPUT].isConnected())
				spread += inputs[SPREAD_INPUT].getVoltage()*0.4*params[SPREAD_ATT_PARAM].getValue();
			if (inputs[DECAY_INPUT].isConnected())
				decay += inputs[DECAY_INPUT].getVoltage()*0.2*params[DECAY_ATT_PARAM].getValue();

			// Clamp the param values after adding voltages
			decay = clamp(decay, 0.00f, 1.0f); 
			spread = clamp(spread, -1.00f, 1.0f);
			lag[0] = clamp(lag[0], 0.0f, 1.0f);
			

			// Apply non-linear re-scaling to parameters to make them feel better
			lag[0] = pow(lag[0], 0.5); //
			spread = (spread >= 0 ? 1 : -1) * pow(abs(spread), 4);

			decay = 1 - pow(1 - decay, 1.5); 
			decay = decay*0.005f + 0.99499f;  //since the decay is so exponential, only values .99...1.0 are really useful for scaling.


			// Re-Clamp the param values after non-linear scaling
			decay = clamp(decay, 0.0f, 1.0f); 
			spread = clamp(spread, -0.9999f, 1.0f);
			lag[0] = clamp(lag[0], 0.001f, 1.0f);
			lag[0] *= 0.35f; //rescale the lag values
			
			lag[23] = 1.2f*spread * lag[0] + 1.2f*lag[0];
			
			// Scaling factor for the power scale
			float scalingFactor =  0.20f; // Adjust this to tune the scaling curve
			
			// Interpolate lag[1] to lag[22] using a power scale
			for (int i = 1; i < 23; i++) {
				float factor = powf(i / 23.0f, scalingFactor);
				lag[i] = lag[0] + (lag[23] - lag[0]) * factor;
			}
			

			//Set threshold to drop to before propagating to the next node, based on the spread input		
			//This function is an ellipse with the left at -1,1 and the right at 0.5,0

			float propagate_thresh = 0.0f;
			if (spread <0.25){
				propagate_thresh = 10.0f - 10.0f * sqrtf(1.0f - powf((spread + 0.25f) / 0.75f, 2.0f));
			}
	
			propagate_thresh = clamp (propagate_thresh, 0.01f, 10.0f);
		
			// Check for manual trigger or input trigger to activate the first node
			bool manualTriggerPressed = params[TRIGGER_BUTTON].getValue() > 0.0f;
			bool currentInputState = (inputs[_00_INPUT].isConnected() && inputs[_00_INPUT].getVoltage() > 1.0f) || manualTriggerPressed;
			if (currentInputState && !previousInputState && outputs[_01_OUTPUT].getVoltage() <= propagate_thresh) {
				activeNodes[0] = true; // Activate node 0
				groupElapsedTime[0] = 0.f; // Reset elapsed time for node 0
				for (LightId light : lightGroups[0]) {
					lights[light].setBrightness(1.0f); // Turn on all lights for node 0's group
				}
			}
			previousInputState = currentInputState;

			// Reset the trigger button state after processing to ensure it is ready for the next press
			if (manualTriggerPressed) {
				params[TRIGGER_BUTTON].setValue(0.0f);
			}		

			 
			if (inputs[_00_INPUT].isConnected()){
				float brightness = inputs[_00_INPUT].getVoltage() / 10.0f;
				lights[_00OUT_LIGHT].setSmoothBrightness(brightness, args.sampleTime);         
			}

			// Activate and Deactivate Nodes, Activate Child Nodes
			// Iterate over all possible nodes
			for (int node = 0; node < 24; ++node) { 
				// Check if the node is active
				if (activeNodes[node]) {
					// Increment the elapsed time for each active node
					groupElapsedTime[node] += args.sampleTime;  // This ensures the elapsed time is updated every cycle

					// Check if the output voltage is below the propagation threshold
					if (outputs[_01_OUTPUT + node].getVoltage() < propagate_thresh && groupElapsedTime[node]>0.8*lag[node]) {
						activeNodes[node] = false;
						// Check and activate child nodes if they are considered inactive
						if (nodeConnections.count(node) > 0) { // Ensure the node has connections defined
							for (int childNode : nodeConnections.at(node)) { 
								// Check if the child node is inactive 
								if (!activeNodes[childNode] ) { 
									activeNodes[childNode] = true;         // Activate the child node
									groupElapsedTime[childNode] = 0.f;     // Reset the child node's elapsed time								//	outputs[_01_OUTPUT + childNode].setVoltage(10.0f); // Set an initial voltage for the child node
									for (LightId light : lightGroups[childNode]) {
										lights[light].setBrightness(1.0f); // Turn on all lights for the child node's group
									}
								} 
							}
						}
					}
				}
			}

			float slewRate = 0.1f;
			// Map OUT light brightness to OUTPUT voltages
			for (int i = 0; i < 24; ++i) {
				// Directly map the light brightness to output voltage
				float brightness = lights[_01OUT_LIGHT + i].getBrightness(); // Get the brightness of the corresponding light
				float current_out = outputs[_01_OUTPUT + i].getVoltage(); // Get the voltage of the current output
				float difference = (brightness * 10.0f) - current_out;
				float voltageChange = difference;

				// If the voltage is increasing, apply slew limiting
				if (difference > 0) {
					 voltageChange = fmin(difference, slewRate);
				}				
				
				outputs[_01_OUTPUT + i].setVoltage( current_out + voltageChange );      // transition to new output voltage based on the light brightness
			}


			// Dim lights slowly for each light group
			for (int groupIndex = 0; groupIndex < int(lightGroups.size()); ++groupIndex) {
				// Calculate the dimming factor for the current group
				float dimmingFactor = decay + (0.99993f-decay)*2*(lag[groupIndex]);
				dimmingFactor = clamp(dimmingFactor,0.0f,0.99993f);

				// Apply the dimming factor to each light within the current group
				for (LightId lightId : lightGroups[groupIndex]) {
					float how_bright = lights[lightId].getBrightness();
					how_bright *= dimmingFactor; 
					lights[lightId].setBrightness(how_bright);
				}
			} 

			
			// After processing, reset the accumulated time
			accumulatedTime -= baseSampleTime; // Subtract to maintain precision and handle any excess
			
		}//if (accumulated_time...
	} // void process
}; //struct

struct ImpulseControllerWidget : ModuleWidget {
	ImpulseControllerWidget(ImpulseController* module) {
		setModule(module);

		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/ImpulseController.svg"),
			asset::plugin(pluginInstance, "res/ImpulseController-dark.svg")
		));


		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


        addParam(createParamCentered<TL1105>(mm2px(Vec(10.916, 65)), module, ImpulseController::TRIGGER_BUTTON));


		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.916, 72.73)), module, ImpulseController::_00_INPUT));

        // Attenuverter Knobs
        addParam(createParamCentered<Trimpot>(mm2px(Vec(11.064, 35.728)), module, ImpulseController::LAG_ATT_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(29.756, 35.728)), module, ImpulseController::SPREAD_ATT_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(48.449, 35.728)), module, ImpulseController::DECAY_ATT_PARAM));


		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.957, 24)), module, ImpulseController::LAG_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(29.649, 24)), module, ImpulseController::SPREAD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(48.342, 24)), module, ImpulseController::DECAY_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.171, 45.049)), module, ImpulseController::LAG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.864, 45.049)), module, ImpulseController::SPREAD_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.556, 45.049)), module, ImpulseController::DECAY_INPUT));


		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(29.445, 72.73)), module, ImpulseController::_01_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(47.974, 72.73)), module, ImpulseController::_02_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.503, 72.73)), module, ImpulseController::_03_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.503, 54.201)), module, ImpulseController::_04_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.031, 72.73)), module, ImpulseController::_05_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.031, 35.672)), module, ImpulseController::_06_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.031, 91.258)), module, ImpulseController::_07_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(103.56, 72.73)), module, ImpulseController::_08_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(103.56, 35.672)), module, ImpulseController::_09_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(103.56, 109.656)), module, ImpulseController::_10_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(103.56, 54.201)), module, ImpulseController::_11_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(122.089, 91.258)), module, ImpulseController::_12_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(122.089, 72.73)), module, ImpulseController::_13_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(122.089, 17.144)), module, ImpulseController::_14_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(122.089, 35.672)), module, ImpulseController::_15_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(122.089, 54.201)), module, ImpulseController::_16_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(122.089, 109.656)), module, ImpulseController::_17_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140.618, 54.201)), module, ImpulseController::_18_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(103.56, 17.144)), module, ImpulseController::_19_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140.618, 91.258)), module, ImpulseController::_20_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140.618, 72.73)), module, ImpulseController::_21_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140.618, 109.656)), module, ImpulseController::_22_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140.618, 17.144)), module, ImpulseController::_23_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140.618, 35.672)), module, ImpulseController::_24_OUTPUT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(20.181, 72.73)), module, ImpulseController::_00A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(23.139, 72.73)), module, ImpulseController::_00B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(38.706, 72.73)), module, ImpulseController::_01A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(41.664, 72.73)), module, ImpulseController::_01B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(57.235, 72.73)), module, ImpulseController::_02A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(60.193, 72.73)), module, ImpulseController::_02B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(54.284, 66.42)), module, ImpulseController::_02C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(57.236, 63.468)), module, ImpulseController::_02D_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(60.193, 60.511)), module, ImpulseController::_02E_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(75.764, 72.73)), module, ImpulseController::_03A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(78.721, 72.73)), module, ImpulseController::_03B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(72.813, 79.04)), module, ImpulseController::_03C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(75.765, 81.992)), module, ImpulseController::_03D_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(78.722, 84.949)), module, ImpulseController::_03E_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(72.813, 47.891)), module, ImpulseController::_04A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(75.765, 44.939)), module, ImpulseController::_04B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(78.722, 41.982)), module, ImpulseController::_04C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(94.292, 72.73)), module, ImpulseController::_05A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(97.25, 72.73)), module, ImpulseController::_05B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(91.342, 66.42)), module, ImpulseController::_05C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(94.294, 63.468)), module, ImpulseController::_05D_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(97.25, 60.511)), module, ImpulseController::_05E_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(94.292, 35.672)), module, ImpulseController::_06A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(97.25, 35.672)), module, ImpulseController::_06B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(91.342, 29.362)), module, ImpulseController::_06C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(94.294, 26.41)), module, ImpulseController::_06D_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(97.25, 23.454)), module, ImpulseController::_06E_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(91.342, 97.569)), module, ImpulseController::_07A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(94.294, 100.521)), module, ImpulseController::_07B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(97.25, 103.478)), module, ImpulseController::_07C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(112.821, 72.73)), module, ImpulseController::_08A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(115.779, 72.73)), module, ImpulseController::_08B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(109.871, 79.04)), module, ImpulseController::_08C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(112.823, 81.992)), module, ImpulseController::_08D_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(115.779, 84.949)), module, ImpulseController::_08E_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(112.821, 35.672)), module, ImpulseController::_09A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(115.779, 35.672)), module, ImpulseController::_09B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(109.871, 29.362)), module, ImpulseController::_09C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(112.823, 26.41)), module, ImpulseController::_09D_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(115.779, 23.454)), module, ImpulseController::_09E_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(112.821, 109.656)), module, ImpulseController::_10A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(115.779, 109.656)), module, ImpulseController::_10B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(112.821, 54.201)), module, ImpulseController::_11A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(115.779, 54.201)), module, ImpulseController::_11B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(131.35, 91.258)), module, ImpulseController::_12A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(134.308, 91.258)), module, ImpulseController::_12B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(128.4, 97.569)), module, ImpulseController::_12C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(131.352, 100.521)), module, ImpulseController::_12D_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(134.308, 103.478)), module, ImpulseController::_12E_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(131.35, 72.73)), module, ImpulseController::_13A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(134.308, 72.73)), module, ImpulseController::_13B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(128.4, 66.42)), module, ImpulseController::_13C_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(131.352, 63.468)), module, ImpulseController::_13D_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(134.308, 60.511)), module, ImpulseController::_13E_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(131.35, 17.144)), module, ImpulseController::_14A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(134.308, 17.144)), module, ImpulseController::_14B_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(131.35, 35.672)), module, ImpulseController::_15A_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(134.308, 35.672)), module, ImpulseController::_15B_LIGHT));

		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(17.23, 72.73)), module, ImpulseController::_00OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(35.756, 72.73)), module, ImpulseController::_01OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(54.284, 72.73)), module, ImpulseController::_02OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(72.813, 72.73)), module, ImpulseController::_03OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(72.813, 54.426)), module, ImpulseController::_04OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(91.342, 72.73)), module, ImpulseController::_05OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(91.342, 35.672)), module, ImpulseController::_06OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(91.342, 91.258)), module, ImpulseController::_07OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(109.871, 72.73)), module, ImpulseController::_08OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(109.871, 35.672)), module, ImpulseController::_09OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(109.871, 109.656)), module, ImpulseController::_10OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(109.871, 54.201)), module, ImpulseController::_11OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(128.4, 91.258)), module, ImpulseController::_12OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(128.4, 72.73)), module, ImpulseController::_13OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(128.4, 17.144)), module, ImpulseController::_14OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(128.4, 35.672)), module, ImpulseController::_15OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(129.392, 54.096)), module, ImpulseController::_16OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(128.53, 109.824)), module, ImpulseController::_17OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(146.928, 54.201)), module, ImpulseController::_18OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(109.871, 17.144)), module, ImpulseController::_19OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(146.928, 91.258)), module, ImpulseController::_20OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(146.928, 72.73)), module, ImpulseController::_21OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(146.928, 109.656)), module, ImpulseController::_22OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(146.928, 17.144)), module, ImpulseController::_23OUT_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(146.928, 35.672)), module, ImpulseController::_24OUT_LIGHT));
	}
};

Model* modelImpulseController = createModel<ImpulseController, ImpulseControllerWidget>("ImpulseController");