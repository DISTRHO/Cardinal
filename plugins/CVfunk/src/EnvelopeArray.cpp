////////////////////////////////////////////////////////////
//
//   Envelope Array
//
//   written by Cody Geary
//   Copyright 2024, MIT License
//
//   Six related envelopes with end of function gates
//
////////////////////////////////////////////////////////////


#include "plugin.hpp"
using simd::float_4;

//Envelope shape adapted from Befaco shapeDelta function
static float_4 Envelope(float_4 delta, float_4 tau, float shape) {
	float_4 lin = simd::sgn(delta) * 10.f / tau;
	if (shape > 0.f) {
		float_4 log = simd::sgn(delta) * 40.f / tau / (simd::fabs(delta) + 1.f);
		return simd::crossfade(lin, log, shape * 1.49f); //1.49 pushes this var to extreme
	}
	else {
		float_4 exp = M_E * delta / tau;
		return simd::crossfade(lin, exp, -shape * 0.99f); //0.99 is the max we can go also.
	}
}

struct EnvelopeArray : Module {
    enum ParamId {
        SLANT_PARAM,
        CURVE_PARAM,
        TIME1_PARAM,
        TIME6_PARAM,
        SLANT_ATTEN_PARAM,
        CURVE_ATTEN_PARAM,
        TIME1_ATTEN_PARAM,
        TIME6_ATTEN_PARAM,
        TIME1_RANGE_BUTTON,  
        TIME6_RANGE_BUTTON,

        /////////////////////
		//SECRET_PARAM, //hidden param for tuning variables        
        /////////////////////

        PARAMS_LEN  
    };
    enum InputId {
        SLANT_INPUT,
        CURVE_INPUT,
        TIME1_INPUT,
        TIME6_INPUT,
        _1_INPUT,
        _2_INPUT,
        _3_INPUT,
        _4_INPUT,
        _5_INPUT,
        _6_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        _1_OUTPUT,
        _2_OUTPUT,
        _3_OUTPUT,
        _4_OUTPUT,
        _5_OUTPUT,
        _6_OUTPUT,
        EOF1_OUTPUT,
        EOF2_OUTPUT,
        EOF3_OUTPUT,
        EOF4_OUTPUT,
        EOF5_OUTPUT,
        EOF6_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        _1_LIGHT,
        _2_LIGHT,
        _3_LIGHT,
        _4_LIGHT,
        _5_LIGHT,
        _6_LIGHT,
        _7_LIGHT,
        _8_LIGHT,
        _9_LIGHT,
        _10_LIGHT,
        _11_LIGHT,
        _12_LIGHT,
        TIME1_LED1_LIGHT,  
        TIME1_LED2_LIGHT,  
        TIME1_LED3_LIGHT,  
        TIME6_LED1_LIGHT,  
        TIME6_LED2_LIGHT,  
        TIME6_LED3_LIGHT,  
        LIGHTS_LEN  
    };
    
	enum SpeedRange {
        HIGH,
        MID,
        LOW
    };

    SpeedRange time1Range;
    SpeedRange time6Range;

	// Define an array to store time variables
	float time_x[6] = {0.0f}; // Initialize all elements to 0.0f
	float_4 out[6][4] = {};
	float_4 gate[6][4] = {}; // use simd __m128 logic instead of bool

	float_4 gate_no_output[6][4] = {{0.0f}}; // Initialize with all elements set to true

	dsp::TSchmittTrigger<float_4> trigger_4[6][4];

    int processSkipCounter = 0;
    int processSkipRate = 2;  // Update the envelope every 2 process cycles to save CPU


    // Serialization method to save module state
    json_t* dataToJson() override {
        json_t* rootJ = json_object();

        // Save the state of time1Range and time6Range
        json_object_set_new(rootJ, "time1Range", json_integer(time1Range));
        json_object_set_new(rootJ, "time6Range", json_integer(time6Range));

        return rootJ;
    }

    // Deserialization method to load module state
    void dataFromJson(json_t* rootJ) override {
        // Load the state of time1Range
        json_t* time1RangeJ = json_object_get(rootJ, "time1Range");
        if (time1RangeJ) time1Range = static_cast<SpeedRange>(json_integer_value(time1RangeJ));

        // Load the state of time6Range
        json_t* time6RangeJ = json_object_get(rootJ, "time6Range");
        if (time6RangeJ) time6Range = static_cast<SpeedRange>(json_integer_value(time6RangeJ));
    }
    

	EnvelopeArray() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SLANT_PARAM, -1.f, 1.f, -.75f, "Slant");
		configParam(CURVE_PARAM, -1.f, 1.f, -.75f, "Curve");
		configParam(TIME1_PARAM, 0.0f, 1.0f, 0.4f, "First Width");
		configParam(TIME6_PARAM, 0.0f, 1.0f, 0.75f, "Last Width");
		configParam(SLANT_ATTEN_PARAM, -1.0f, 1.0f, 1.f, "");
		configParam(CURVE_ATTEN_PARAM, -1.f, 1.f, 1.F, "");
		configParam(TIME1_ATTEN_PARAM, -1.f, 1.f, 1.f, "");
		configParam(TIME6_ATTEN_PARAM, -1.f, 1.f, 1.f, "");
		
        /////////////////////
		//configParam(SECRET_PARAM,-5.0, 10.0f, 4.7f, "Mapping to a test knob"); //only used for calibration
        /////////////////////

		configInput(SLANT_INPUT, "Slant IN");
		configInput(CURVE_INPUT, "Curve IN");
		configInput(TIME1_INPUT, "First Width IN");
		configInput(TIME6_INPUT, "Last Width IN");
		configInput(_1_INPUT, "IN 1");
		configInput(_2_INPUT, "IN 2");
		configInput(_3_INPUT, "IN 3");
		configInput(_4_INPUT, "IN 4");
		configInput(_5_INPUT, "IN 5");
		configInput(_6_INPUT, "IN 6");
		configOutput(_1_OUTPUT, "OUT 1");
		configOutput(_2_OUTPUT, "OUT 2");
		configOutput(_3_OUTPUT, "OUT 3");
		configOutput(_4_OUTPUT, "OUT 4");
		configOutput(_5_OUTPUT, "OUT 5");
		configOutput(_6_OUTPUT, "OUT 6");
		configOutput(EOF1_OUTPUT, "GATE 1");
		configOutput(EOF2_OUTPUT, "GATE 2");
		configOutput(EOF3_OUTPUT, "GATE 3");
		configOutput(EOF4_OUTPUT, "GATE 4");
		configOutput(EOF5_OUTPUT, "GATE 5");
		configOutput(EOF6_OUTPUT, "GATE 6");


		// Default initialization of time ranges to MID
		time1Range = MID;
		time6Range = MID;


	}

    void process(const ProcessArgs &args) override {	
 
		 // Button logic for Time1
		if (params[TIME1_RANGE_BUTTON].getValue() > 0) {
			time1Range = static_cast<SpeedRange>((time1Range + 1) % 3);
			params[TIME1_RANGE_BUTTON].setValue(0);
		}

		// Button logic for Time6
		if (params[TIME6_RANGE_BUTTON].getValue() > 0) {
			time6Range = static_cast<SpeedRange>((time6Range + 1) % 3);
			params[TIME6_RANGE_BUTTON].setValue(0);
		}

		// Update LED lights for Time1
		lights[TIME1_LED1_LIGHT].setBrightness(time1Range == HIGH ? 1.0f : 0.0f);
		lights[TIME1_LED2_LIGHT].setBrightness(time1Range == MID ? 1.0f : 0.0f);
		lights[TIME1_LED3_LIGHT].setBrightness(time1Range == LOW ? 1.0f : 0.0f);

		// Update LED lights for Time6
		lights[TIME6_LED1_LIGHT].setBrightness(time6Range == HIGH ? 1.0f : 0.0f);
		lights[TIME6_LED2_LIGHT].setBrightness(time6Range == MID ? 1.0f : 0.0f);
		lights[TIME6_LED3_LIGHT].setBrightness(time6Range == LOW ? 1.0f : 0.0f);
 
        // Read inputs and parameters...
        float slant = params[SLANT_PARAM].getValue();
        float curve = params[CURVE_PARAM].getValue();
        
        /////////////////////
		//float secret = params[SECRET_PARAM].getValue();  //Get our calibration parameter here
		/////////////////////
             
        time_x[0] = params[TIME1_PARAM].getValue(); //Time interval for the first generator
        time_x[5] = params[TIME6_PARAM].getValue(); //Time interval for the last generator

		if (inputs[SLANT_INPUT].isConnected())
			slant += inputs[SLANT_INPUT].getVoltage()*params[SLANT_ATTEN_PARAM].getValue()*0.2; //scaled so 10V envelope spans the whole range
		if (inputs[CURVE_INPUT].isConnected())
			curve += inputs[CURVE_INPUT].getVoltage()*params[CURVE_ATTEN_PARAM].getValue()*0.2;
		if (inputs[TIME1_INPUT].isConnected())
			time_x[0] += inputs[TIME1_INPUT].getVoltage()*params[TIME1_ATTEN_PARAM].getValue()*0.1;
		if (inputs[TIME6_INPUT].isConnected())
			time_x[5] += inputs[TIME6_INPUT].getVoltage()*params[TIME6_ATTEN_PARAM].getValue()*0.1;


		//Clamp voltages after adding CV params
		slant = clamp(slant, -1.0f, 1.0f);
		curve = clamp(curve, -1.0f, 1.0f);
	//	time_x[0] = clamp(time_x[0], 0.0f,1.0f);  //let the CV push the time to the max limits of range, regardless of range settings.
	//	time_x[5] = clamp(time_x[5], 0.0f,1.0f);


		//Apply non-linear scaling to the slant knob to make it scale more naturally
		slant = copysign(pow(fabs(slant), 2), slant); // Apply the power rule while preserving the sign


		//Set time ranges based on the range selectors.
		//Updated so that all channels use the same, smaller minTime setting
		if (time1Range==HIGH){
			time_x[0] *= 1.5f; 
		} else if (time1Range==MID){
			time_x[0] = time_x[0]*1.5f + 1.4f; 
		} else if (time1Range==LOW){
			time_x[0] = time_x[0]*1.5f + 2.8f; 
		}

		if (time6Range==HIGH){
			time_x[5] *= 1.5f; 
		} else if (time6Range==MID){
			time_x[5] = time_x[5]*1.5f + 1.4f; 
		} else if (time6Range==LOW){
			time_x[5] = time_x[5]*1.5f + 2.8f; 
		}

		// Clamp time_x[0] and time_x[5] to be positive only
		time_x[0] = std::max(time_x[0], 0.0f);
		time_x[5] = std::max(time_x[5], 0.0f);


		//Adjust for non-linearity of the slant control
		float f_slant;
		if (abs(slant) <= 0.6f) {
			f_slant = (5.0f / 3.0f) * abs(slant);  // From 0 to 0.6
		} else {
			f_slant = -(5.0f / 2.0f) * (abs(slant) - 0.6f) + 1;  // From 0.6 to 1
		}
		float slant_abs = abs(slant) + f_slant * 0.1f;

		time_x[0] -= slant_abs / (2.4760985f * pow(time_x[0], -1.17f));
		time_x[5] -= slant_abs / (2.4760985f * pow(time_x[5], -1.17f));


		// Adjust for non-linearity of the curve control
		float f_curve;
		if (abs(curve) <= 0.75f) {
			f_curve = (4.0f / 3.0f) * abs(curve);  // From 0 to 0.75
		} else {
			f_curve = -4.0f * (abs(curve) - 0.75f) + 1;  // From 0.75 to 1
		}
		float curve_abs = abs(curve) + f_curve * -0.66f;
		float curve_abs2 = abs(curve) + f_curve * -0.33f;


		//Scale the time_x inputs to compensate for the increase in cycle time for different curve values.
		//For large curve values the slant compensation needs to be readjusted again
		float curve_scalefactor = 4.6;    //hand-calibrated
		float curve_scalefactor2 = 2.85;  //hand-calibrated 
		float slant_scalefactor1 = .45;   //hand-calibrated
		float slant_scalefactor2 = .4;    //hand-calibrated
		if (curve<0) {  //this is split into two parts because the log side of curve can scale further
			time_x[0] = time_x[0]-(curve_abs/curve_scalefactor)*(1 - (slant_abs*slant_scalefactor1) );
			time_x[5] = time_x[5]-(curve_abs/curve_scalefactor)*(1 - (slant_abs*slant_scalefactor1) );
		} else {
			time_x[0] = time_x[0]-(curve_abs2/curve_scalefactor2)*(1 - (slant_abs*slant_scalefactor2) );
			time_x[5] = time_x[5]-(curve_abs2/curve_scalefactor2)*(1 - (slant_abs*slant_scalefactor2) );
		}


		//clamp time to a max range where the decay is at least one sample value
		//(otherwise it will hold forever if the decay is too slow)
		time_x[0] = clamp(time_x[0], 0.0f,4.3f);
		time_x[5] = clamp(time_x[5], 0.0f,4.3f);



		//Adjust slant to be 0-1V range, for the Envelope function.
		slant = (slant+1.0f)/2; 

		// Calculate the step size between each time_x value
		float time_step = (time_x[5] - time_x[0]) / 2.236f;
	
		// Calculate the intermediate values using square root function interpolation
		if (time_x[5] >= time_x[0]) {
			time_x[1] = time_x[0] + 0.92f * time_step;
			time_x[2] = time_x[0] + 1.414f * time_step;
			time_x[3] = time_x[0] + 1.732f * time_step;
			time_x[4] = time_x[0] + 2.0f * time_step;
		} else {
			time_x[4] = time_x[5] - 0.92f * time_step;
			time_x[3] = time_x[5] - 1.414f * time_step;
			time_x[2] = time_x[5] - 1.732f * time_step;
			time_x[1] = time_x[5] - 2.0f * time_step;
		}
	


		int channels_in[6] = {};
		int channels_trig[6] = {};
		int channels[6] = {}; 	// the larger of in or trig (per-part)

		// determine number of channels:
		for (int part = 0; part < 6; part++) {

			channels_in[part]   = 0.0f;
			channels_trig[part] = inputs[_1_INPUT + part].getChannels();
			channels[part] = std::max(channels_in[part], channels_trig[part]);
			channels[part] = std::max(1, channels[part]);

			outputs[_1_OUTPUT + part].setChannels(channels[part]);
			outputs[EOF1_OUTPUT + part].setChannels(channels[part]);					
		}

		// total number of active polyphony engines, accounting for all parts
		int channels_max = channels[0]; // Initialize with the first value
		for (int i = 1; i < 4; ++i) {
			channels_max = std::max(channels_max, channels[i]);
		}


		//SKIP process computations ever other cycle to save CPU:
        if (++processSkipCounter >= processSkipRate) {
            processSkipCounter = 0;  // Reset counter

			// loop over six stage parts:
			for (int part = 0; part < 6; part++) {

				float_4 in[4] = {};
				float_4 in_trig[6][4] = {};
				float_4 riseCV[4] = {};
				float_4 fallCV[4] = {};
				float_4 cycle[4] = {};


				float minTime = .0001f; //set a very small minTime

				float_4 param_rise  = time_x[part] * (slant) * 10.0f;
			
				float_4 param_fall  = time_x[part] * (1.0f - slant) * 10.0f;
				float_4 param_trig  = 0.0f;
				float_4 param_cycle = 0.0f;

				for (int c = 0; c < channels[part]; c += 4) {
					riseCV[c / 4] = param_rise;
					fallCV[c / 4] = param_fall;
					cycle[c / 4] = param_cycle;
					in_trig[part][c / 4] = param_trig;
				}

				// Read inputs:
				if (inputs[_1_INPUT + part].isConnected()) {
					for (int c = 0; c < channels[part]; c += 4)
						in_trig[part][c / 4] += inputs[_1_INPUT + part].getPolyVoltageSimd<float_4>(c);
				} else {
					// Look for a trigger input in previous parts
					for (int prevPart = part - 1; prevPart >= 0; prevPart--) {
						if (inputs[_1_INPUT + prevPart].isConnected()) {
							// Found a trigger input in a previous part
							// Use its voltage for the current part's trigger input
							for (int c = 0; c < channels[prevPart]; c += 4)
								in_trig[part][c / 4] += inputs[_1_INPUT + prevPart].getPolyVoltageSimd<float_4>(c);
							break; // Exit the loop since we found a trigger input
						}
					}
				}

				// start processing:
				for (int c = 0; c < channels[part]; c += 4) {

					// process SchmittTriggers			
					// Convert triggers to processed triggers
					float_4 trig_mask = trigger_4[part][c / 4].process(in_trig[part][c / 4] / 2.0, 0.1, 2.0);
					//store the gate, but only of the gate_no_output is low
					gate[part][c / 4] = ifelse(trig_mask & (gate_no_output[part][c / 4] == 10.0f), float_4::mask(), gate[part][c / 4]);

					in[c / 4] = ifelse(gate[part][c / 4], 10.0f, in[c / 4]);			
					float_4 delta = in[c / 4] - out[part][c / 4];

					// rise / fall branching
					float_4 delta_gt_0 = delta > 0.f;
					float_4 delta_lt_0 = delta < 0.f;
					float_4 delta_eq_0 = ~(delta_lt_0 | delta_gt_0);

					float_4 rising  = simd::ifelse(delta_gt_0, (in[c / 4] - out[part][c / 4]) > 1e-6f, float_4::zero());
					float_4 falling = simd::ifelse(delta_lt_0, (in[c / 4] - out[part][c / 4]) < -1e-6f, float_4::zero());
					float_4 end_of_cycle = simd::andnot(falling, delta_lt_0);

					float_4 rateCV = ifelse(delta_gt_0, riseCV[c / 4], 0.f);
					rateCV = ifelse(delta_lt_0, fallCV[c / 4], rateCV);
					rateCV = clamp(rateCV, 0.f, 1e9f);

					float_4 rate = minTime * simd::pow(2.0f, rateCV);
				
					//Compute the change in output value
					out[part][c / 4] += Envelope(delta, rate, curve) * args.sampleTime;  

					// Clamp the output to ensure it stays between 0 and 10.0V
					out[part][c / 4] = simd::clamp(out[part][c / 4], simd::float_4(0.0f), simd::float_4(10.0f));

					gate[part][c / 4] = ifelse(simd::andnot(rising, delta_gt_0), 0.f, gate[part][c / 4]);
					gate[part][c / 4] = ifelse(end_of_cycle & (cycle[c / 4] >= 4.0f), float_4::mask(), gate[part][c / 4]);
					gate[part][c / 4] = ifelse(delta_eq_0, 0.f, gate[part][c / 4]);

					out[part][c / 4]  = ifelse(rising | falling, out[part][c / 4], in[c / 4]);
			
					// Determine the new gate output based on the voltage of out[part][c / 4]
					gate_no_output[part][c / 4] = ifelse(out[part][c / 4] == 0.0f, 10.0f, 0.0f);
				
					// Set the voltage for the outputs
					outputs[_1_OUTPUT + part].setVoltageSimd(out[part][c / 4], c);
					outputs[_1_OUTPUT + part + 6].setVoltageSimd(gate_no_output[part][c / 4], c);			

				} // for(int c, ...)

				if (channels[part] == 1) {
					lights[_1_LIGHT + part  ].setSmoothBrightness(out[part][0].s[0] / 10.0, args.sampleTime);
					lights[_1_LIGHT + 6 + part  ].setSmoothBrightness(gate_no_output[part][0].s[0] / 10.0, args.sampleTime);				
				}

			} // for (int part, ... )
		}
	}//void
};//module

struct EnvelopeArrayWidget : ModuleWidget {
	EnvelopeArrayWidget(EnvelopeArray* module) {
		setModule(module);
		
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/EnvelopeArray.svg"),
			asset::plugin(pluginInstance, "res/EnvelopeArray-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(29.337, 24.514)), module, EnvelopeArray::SLANT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(47.525, 24.514)), module, EnvelopeArray::CURVE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.228, 28.738)), module, EnvelopeArray::TIME1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(65.323, 28.738)), module, EnvelopeArray::TIME6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(29.337, 41.795)), module, EnvelopeArray::SLANT_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(47.525, 41.795)), module, EnvelopeArray::CURVE_ATTEN_PARAM));


        /////////////////////
		//addParam(createParamCentered<Trimpot>(mm2px(Vec(38.277, 45)), module, EnvelopeArray::SECRET_PARAM));
        /////////////////////


		addParam(createParamCentered<Trimpot>(mm2px(Vec(11.228, 45.315)), module, EnvelopeArray::TIME1_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(65.323, 45.315)), module, EnvelopeArray::TIME6_ATTEN_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.337, 55.194)), module, EnvelopeArray::SLANT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(47.525, 55.194)), module, EnvelopeArray::CURVE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.228, 58.715)), module, EnvelopeArray::TIME1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(65.323, 58.715)), module, EnvelopeArray::TIME6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.1, 78.815)), module, EnvelopeArray::_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.459, 78.815)), module, EnvelopeArray::_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.818, 78.815)), module, EnvelopeArray::_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.178, 78.815)), module, EnvelopeArray::_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56.537, 78.815)), module, EnvelopeArray::_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(68.896, 78.815)), module, EnvelopeArray::_6_INPUT));

		// For Time1 Group
		float groupStartXTime1 = 11.228 - 11.5; // Starting x-coordinate for the Time1 group
		addParam(createParamCentered<TL1105>(mm2px(Vec(groupStartXTime1 + 6.5, 15)), module, EnvelopeArray::TIME1_RANGE_BUTTON)); // Button is 5 mm wide, so +2.5 mm to center it

		// LEDs for Time1, positioned right of the button with 1 mm gaps
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(groupStartXTime1 + 12, 15)), module, EnvelopeArray::TIME1_LED1_LIGHT)); // First LED
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(groupStartXTime1 + 15, 15)), module, EnvelopeArray::TIME1_LED2_LIGHT)); // Second LED
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(groupStartXTime1 + 18, 15)), module, EnvelopeArray::TIME1_LED3_LIGHT)); // Third LED

		// For Time6 Group
		float groupStartXTime6 = 65.323 - 11.5; // Starting x-coordinate for the Time6 group
		addParam(createParamCentered<TL1105>(mm2px(Vec(groupStartXTime6 + 6.5, 15)), module, EnvelopeArray::TIME6_RANGE_BUTTON)); // Button centered

		// LEDs for Time6, positioned right of the button with 1 mm gaps
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(groupStartXTime6 + 12, 15)), module, EnvelopeArray::TIME6_LED1_LIGHT)); // First LED
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(groupStartXTime6 + 15, 15)), module, EnvelopeArray::TIME6_LED2_LIGHT)); // Second LED
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(groupStartXTime6 + 18, 15)), module, EnvelopeArray::TIME6_LED3_LIGHT)); // Third LED


		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.1, 93.125)), module, EnvelopeArray::_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.459, 93.125)), module, EnvelopeArray::_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.818, 93.125)), module, EnvelopeArray::_3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.178, 93.125)), module, EnvelopeArray::_4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(56.537, 93.125)), module, EnvelopeArray::_5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(68.896, 93.125)), module, EnvelopeArray::_6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.1, 112.33)), module, EnvelopeArray::EOF1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.459, 112.33)), module, EnvelopeArray::EOF2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.818, 112.33)), module, EnvelopeArray::EOF3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.178, 112.33)), module, EnvelopeArray::EOF4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(56.537, 112.33)), module, EnvelopeArray::EOF5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(68.896, 112.33)), module, EnvelopeArray::EOF6_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.1, 86.153)), module, EnvelopeArray::_1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.518, 86.153)), module, EnvelopeArray::_2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(31.819, 86.153)), module, EnvelopeArray::_3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(44.119, 86.153)), module, EnvelopeArray::_4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(56.42, 86.153)), module, EnvelopeArray::_5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(68.896, 86.153)), module, EnvelopeArray::_6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.1, 105.867)), module, EnvelopeArray::_7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.518, 105.867)), module, EnvelopeArray::_8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(31.819, 105.867)), module, EnvelopeArray::_9_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(44.119, 105.867)), module, EnvelopeArray::_10_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(56.42, 105.867)), module, EnvelopeArray::_11_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(68.896, 105.937)), module, EnvelopeArray::_12_LIGHT));
	}
};

Model* modelEnvelopeArray = createModel<EnvelopeArray, EnvelopeArrayWidget>("EnvelopeArray");