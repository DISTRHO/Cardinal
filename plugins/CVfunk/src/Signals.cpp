////////////////////////////////////////////////////////////
//
//   Signals
//
//   written by Cody Geary
//   Copyright 2024, MIT License
//
//   Displays 6 signals, with pass through
//
////////////////////////////////////////////////////////////

#include "plugin.hpp"
#include "rack.hpp"

#include <array>
#include <vector>

using namespace rack;

float MAX_TIME = 10.0f; // Max window time in seconds
int MAX_BUFFER_SIZE; // Buffer size will be set in the constructor based on the sample rate


struct Signals : Module {
    enum ParamId {
        RANGE_PARAM,
		TRIGGER_ON_PARAM,
		RANGE_BUTTON_PARAM,
        NUM_PARAMS
    };
    enum InputId {
        ENV1_INPUT, ENV2_INPUT, ENV3_INPUT, ENV4_INPUT, ENV5_INPUT, ENV6_INPUT,
        NUM_INPUTS
    };
    enum OutputId {
		ENV1_OUTPUT, ENV2_OUTPUT, ENV3_OUTPUT, ENV4_OUTPUT, ENV5_OUTPUT, ENV6_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightId {
		TRIGGER_ON_LIGHT,
		LONG_LIGHT,
        NUM_LIGHTS
    };

	//Declare global variables

	float currentTimeSetting = 1.0f;
    std::array<std::vector<float>, 6> envelopeBuffers;
    std::array<int, 6> writeIndices = {}; // Track the current write position for each buffer
	float lastInputs[5]={};

    std::array<bool, 6> bufferCycledFlags = {false}; // Flags to indicate when each buffer has been cycled through
	std::array<float, 6> lastTriggerTime; // Time since the last trigger for each channel
	bool retriggerEnabled; 
	bool retriggerToggleProcessed = false;
	float forceRetriggerFlags[5]={};


	// Serialization method to save module state
	json_t* dataToJson() override {
		json_t* rootJ = json_object();

		// Save the state of retriggerEnabled as a boolean
		json_object_set_new(rootJ, "retriggerEnabled", json_boolean(retriggerEnabled));

		return rootJ;
	}

	// Deserialization method to load module state
	void dataFromJson(json_t* rootJ) override {
		// Load the state of retriggerEnabled
		json_t* retriggerEnabledJ = json_object_get(rootJ, "retriggerEnabled");
		if (retriggerEnabledJ) {
			// Use json_is_true() to check if the JSON value is true; otherwise, set to false
			retriggerEnabled = json_is_true(retriggerEnabledJ);
		}
	}


    Signals() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(RANGE_PARAM, 0.0f, 1.0f, 0.5f, "Range");
        configParam(TRIGGER_ON_PARAM, 0.f, 1.f, 1.f, "Retriggering"); // Toggle button
        configParam(RANGE_BUTTON_PARAM, 0.f, 1.f, 0.f, "Range Mode"); // Switch

        lastTriggerTime.fill(0.0f); // Initialize the trigger timers
        MAX_BUFFER_SIZE = int(static_cast<int>(APP->engine->getSampleRate() * MAX_TIME));

        for (auto &buffer : envelopeBuffers) {
            buffer.resize(MAX_BUFFER_SIZE, 0.0f); // Initialize buffers with zeros
        }
    }

	void process(const ProcessArgs& args) override {     
		float range = params[RANGE_PARAM].getValue(); // Range knob value [0, 1]

		//Set the Time Range by switch
		if (params[RANGE_BUTTON_PARAM].getValue()>0.5) {
			range *= 0.89;//wonky fix for range issue
			currentTimeSetting = MAX_TIME;
			lights[LONG_LIGHT].setBrightness( 1.0f );
		} else {
			currentTimeSetting = 1.0f;
			lights[LONG_LIGHT].setBrightness( 0.0f );
		}

		range = clamp(range, 0.000001f, .9999f); //avoid artifact at end of range

		for (int i = 0; i < NUM_INPUTS; ++i) {
			if (inputs[i].isConnected()) {
				float inputVoltage = inputs[i].getVoltage();

				// Force a retrigger for this channel if the flag is set
				if (forceRetriggerFlags[i]) {
					std::fill(envelopeBuffers[i].begin(), envelopeBuffers[i].end(), 0.0f);
					writeIndices[i] = 0;
					lastTriggerTime[i] = 0.0f; // Reset the timer for this channel
					forceRetriggerFlags[i] = false; // Clear the flag after retriggering
				}
							
				// Update the timer for this channel
				lastTriggerTime[i] += args.sampleTime;

				// Check for retrigger condition only if retriggering is enabled
				if (retriggerEnabled && inputVoltage > 1.0f 
					&& lastInputs[i] <= 1.0f 
					&& lastTriggerTime[i] >= (range*.99f*currentTimeSetting + .01f) ) {
				
					//	std::fill(envelopeBuffers[i].begin(), envelopeBuffers[i].end(), 0.0f);
						writeIndices[i] = 0;
						lastTriggerTime[i] = 0.0f; // Reset the timer
						forceRetriggerFlags[i] = false; // Clear the flag after retriggering
				} else {
					// Continue writing to buffer if not triggered or if retriggering is disabled
					envelopeBuffers[i][writeIndices[i]] = inputVoltage;
					writeIndices[i] = (writeIndices[i] + 1) % int((MAX_BUFFER_SIZE/MAX_TIME)*currentTimeSetting);
				}

				lastInputs[i] = inputVoltage;
			} else if (lastInputs[i] != 0.0f) {
				// Input was previously connected but now is not, reset the buffer once and set last input to 0
				std::fill(envelopeBuffers[i].begin(), envelopeBuffers[i].end(), 0.0f);
				writeIndices[i] = 0;
				lastInputs[i] = 0.0f; // Update lastInputs to indicate the channel is now inactive
				lastTriggerTime[i] = 0.0f; // Reset the timer
			}
			// If the input is not connected and last input is already 0, do nothing
		}//for(int i=0...

		// Toggle retriggerEnabled state when the button is pressed
		if (params[TRIGGER_ON_PARAM].getValue() > 0.5f && !retriggerToggleProcessed) {
			retriggerEnabled = !retriggerEnabled;
			retriggerToggleProcessed = true; // Mark that we've processed this toggle
			params[TRIGGER_ON_PARAM].setValue(0.0f); // Reset the button's parameter value to simulate a latch

			// If retriggering is now disabled, sync all channels
			if (!retriggerEnabled) {
				for (int i = 0; i < NUM_INPUTS; ++i) {
					std::fill(envelopeBuffers[i].begin(), envelopeBuffers[i].end(), 0.0f);
					writeIndices[i] = 0;
					lastTriggerTime[i] = 0.0f; // Reset the timer for each channel
				}
			}
		} else if (params[TRIGGER_ON_PARAM].getValue() <= 0.5f) {
			retriggerToggleProcessed = false; // Allow for another toggle when the button is released and pressed again
		}

		// Set light brightness based on the toggle state
		lights[TRIGGER_ON_LIGHT].setBrightness(retriggerEnabled ? 1.0f : 0.0f);

	
		for (int i = 0; i < NUM_INPUTS; ++i) {
			if (inputs[i].isConnected()) {
				// Directly pass through the input to the associated output
				outputs[i].setVoltage(inputs[i].getVoltage());
			} else {
				outputs[i].setVoltage(0.0f);
			}
		}
				
	}//void
};//module

struct WaveformDisplay : TransparentWidget {
    Signals* module;
    int channelId;
    NVGcolor waveformColor;

    WaveformDisplay(NVGcolor color) : waveformColor(color) {}


	void drawWaveform(const DrawArgs& args) {
		if (!module) return;

		const auto& buffer = module->envelopeBuffers[channelId];
		float range = (module->params[Signals::RANGE_PARAM].getValue())/(MAX_TIME/module->currentTimeSetting);

		//Adjust the Time Range if switched
				if (module->params[Signals::RANGE_BUTTON_PARAM].getValue()>0.5) {
					range *= 0.89; //wonky fix, at the slow scale the sample divisions isn't working perfectly right
				} 

		// Use a fixed number of samples to display
		int displaySamples = 1024;

		std::vector<Vec> points;

		// Calculate the y-coordinate of the first sample
		float firstSampleY;
		if (module->inputs[Signals::ENV1_INPUT + channelId].isConnected() && !buffer.empty()) {
			firstSampleY = box.size.y * (1.0f - (buffer.front() / 15.0f)); // Divisor sets Y scaling
		} else {
			firstSampleY = box.size.y; // If input is not connected, set to bottom of the box
		}

		// Add a line segment from (0, box.size.y) to the first sample
		points.push_back(Vec(0, box.size.y)); // Start from the origin (bottom left)
		points.push_back(Vec(0, firstSampleY)); // Line to the first sample's y-coordinate

		for (int i = 0; i < displaySamples; ++i) {
			// Calculate the index in the buffer considering the range
			int bufferIndex = int(i * ((buffer.size()-1)*range+1) / (displaySamples - 1));

			float x = (static_cast<float>(i) / (displaySamples - 1)) * box.size.x; // Map to x-coordinate
			float y; // Initialize y-coordinate

			// Check if the corresponding input is connected
			if (module->inputs[Signals::ENV1_INPUT + channelId].isConnected()) {
				y = box.size.y * (1.0f - (buffer[bufferIndex] / 15.0f)); // Divisor sets Y scaling
			} else {
				y = box.size.y; // Set y to 0 (at the bottom of the box) if input is not connected
			}

			points.push_back(Vec(x, y));
		}

		nvgBeginPath(args.vg);
		nvgStrokeWidth(args.vg, range + 1.5); 
		nvgStrokeColor(args.vg, waveformColor);

		//This codeblock makes the left edge of trigger-synced decay envelopes look solid
		if (module->retriggerEnabled) {
			// When retriggering is enabled, start from the origin
			nvgMoveTo(args.vg, 0, box.size.y); // Move to the origin (bottom left corner)
		} else {
			// When retriggering is disabled, start from the first actual data point
			if (!points.empty()) {
				nvgMoveTo(args.vg, points[0].x, points[0].y); // Move to the first data point
			}
		}

		// Draw line segments through all points
		for (size_t i = 1; i < points.size(); ++i) {
			nvgLineTo(args.vg, points[i].x, points[i].y);
		}

		nvgStroke(args.vg);
	}

	void draw(const DrawArgs& args) override {
		drawWaveform(args);
	}
};


struct SignalsWidget : ModuleWidget {
    SignalsWidget(Signals* module) {
        setModule(module);
              
        setPanel(createPanel(
			asset::plugin(pluginInstance, "res/Signals.svg"),
			asset::plugin(pluginInstance, "res/Signals-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // Range knob
        addParam(createParam<RoundBlackKnob>(mm2px(Vec(5, 14)), module, Signals::RANGE_PARAM));

		addParam(createParam<CKSS>(mm2px(Vec(22, 17)), module, Signals::RANGE_BUTTON_PARAM));
        addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(28, 19)), module, Signals::LONG_LIGHT));

        addParam(createParamCentered<TL1105>(mm2px(Vec(38, 19)), module, Signals::TRIGGER_ON_PARAM));
        addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(44, 19)), module, Signals::TRIGGER_ON_LIGHT));

		NVGcolor colors[6] = {
			nvgRGB(0xa0, 0xa0, 0xa0), // Even Lighter Grey
			nvgRGB(0x90, 0x90, 0x90), // Lighter Grey
			nvgRGB(0x80, 0x80, 0x80), // Grey
			nvgRGB(0x70, 0x70, 0x9b), // Grey-Blue
			nvgRGB(0x60, 0x60, 0x8b), // Darker Grey-Blue
			nvgRGB(0x50, 0x50, 0x7b)  // Even Darker Grey-Blue
		};

		float initialYPos = 75; 
		float spacing = 45; // Increase spacing
		for (int i = 0; i < 6; ++i) {
			float yPos = initialYPos + i * spacing; // Adjusted positioning and spacing

			addInput(createInput<PJ301MPort>(Vec(5, yPos+20), module, i));
			addOutput(createOutput<PJ301MPort>(Vec(148, yPos+20), module, i));

			WaveformDisplay* display = new WaveformDisplay(colors[i]);
			display->box.pos = Vec(40, yPos );
			display->box.size = Vec(100, 40); 
			display->module = module;
			display->channelId = i;
			addChild(display);
		}

    }
    
};
Model* modelSignals = createModel<Signals, SignalsWidget>("Signals");