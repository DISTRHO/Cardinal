/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

/**
 * This file contains a substantial amount of code from VCVRack, adjusted for inline use
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#pragma once

#include <cmath>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <vector>

struct NVGcolor { float a; };
struct NVGpaint {};

inline NVGcolor nvgRGB(int r, int g, int b) { return {}; }
inline NVGcolor nvgRGBA(int r, int g, int b, int a) { return {}; }
inline NVGcolor nvgRGBf(float r, float g, float b) { return {}; }
inline NVGcolor nvgRGBAf(float r, float g, float b, float a) { return {}; }
inline void nvgBeginPath(void* vg) {}
inline void nvgFillColor(void* vg, NVGcolor) {}
inline void nvgFillPaint(void* vg, NVGpaint) {}
inline void nvgFill(void* vg) {}
inline void nvgStrokeColor(void* vg, NVGcolor) {}
inline void nvgStrokeWidth(void* vg, float) {}
inline void nvgStroke(void* vg) {}
inline void nvgRect(void* vg, float a, float b, float c, float d) {}
inline void nvgImageSize(void*, int, void*, void*) {}
inline NVGpaint nvgImagePattern(void*, float, float, float, float, float, int handle, float) { return {}; }

struct json_t {};
json_t* json_integer(int) { return NULL; }
json_t* json_object() { return NULL; }
json_t* json_object_get(json_t*, const char*) { return NULL; }
int json_integer_value(json_t*) { return 0; }
void json_object_set_new(json_t*, const char*, json_t*) {}

namespace rack {

struct Quantity {
    virtual ~Quantity() {}
    virtual void setValue(float value) {}
    virtual float getValue() { return 0.f; }
    virtual float getMinValue() { return 0.f; }
    virtual float getMaxValue() { return 1.f; }
    virtual float getDefaultValue() { return 0.f; }
// 	virtual float getDisplayValue();
// 	virtual void setDisplayValue(float displayValue);
// 	virtual int getDisplayPrecision();
// 	virtual std::string getDisplayValueString();
// 	virtual void setDisplayValueString(std::string s);
    virtual std::string getLabel() { return ""; }
    virtual std::string getUnit() { return ""; }
// 	virtual std::string getString();
// 	virtual void reset();
// 	virtual void randomize();
// 	bool isMin();
// 	bool isMax();
// 	void setMin();
// 	void setMax();
// 	void toggle();
// 	void moveValue(float deltaValue);
// 	float getRange();
// 	bool isBounded();
// 	float toScaled(float value);
// 	float fromScaled(float scaledValue);
// 	void setScaledValue(float scaledValue);
// 	float getScaledValue();
// 	void moveScaledValue(float deltaScaledValue);
};

namespace ui {
struct Menu;
}

namespace math {

inline int clamp(int x, int a, int b) {
    return std::max(std::min(x, b), a);
}

inline float clamp(float x, float a = 0.f, float b = 1.f) {
    return std::fmax(std::fmin(x, b), a);
}

struct Vec {
    float x = 0.f;
    float y = 0.f;
    Vec() {}
    Vec(float xy) : x(xy), y(xy) {}
    Vec(float x, float y) : x(x), y(y) {}
    Vec neg() const { return Vec(-x, -y); }
    Vec plus(Vec b) const { return Vec(x + b.x, y + b.y); }
    Vec minus(Vec b) const { return Vec(x - b.x, y - b.y); }
    Vec mult(float s) const { return Vec(x * s, y * s); }
    Vec mult(Vec b) const { return Vec(x * b.x, y * b.y); }
};

struct Rect {
    Vec pos;
    Vec size;
};

} // namespace math

namespace engine {

static constexpr const int PORT_MAX_CHANNELS = 16;

struct Module;

struct Engine {
    float getSampleRate() { return sampleRate; }
    // custom
    float sampleRate = 0.f;
};

struct Light {
    float value = 0.f;
    void setBrightness(float brightness) {
        value = brightness;
    }
    float getBrightness() {
        return value;
    }
    void setBrightnessSmooth(float brightness, float deltaTime, float lambda = 30.f) {
        if (brightness < value) {
            // Fade out light
            value += (brightness - value) * lambda * deltaTime;
        }
        else {
            // Immediately illuminate light
            value = brightness;
        }
    }
    void setSmoothBrightness(float brightness, float deltaTime) {
        setBrightnessSmooth(brightness, deltaTime);
    }
    void setBrightnessSmooth(float brightness, int frames = 1) {
        setBrightnessSmooth(brightness, frames / 44100.f);
    }
};

struct Param {
    float value = 0.f;
    float getValue() { return value; }
    void setValue(float value) { this->value = value; }
};

struct Port {
    union {
        float voltages[PORT_MAX_CHANNELS] = {};
        float value;
    };
    union {
        uint8_t channels = 0;
        uint8_t active;
    };
    Light plugLights[3];
    enum Type {
        INPUT,
        OUTPUT,
    };

    void setVoltage(float voltage, int channel = 0) { voltages[channel] = voltage; }
    float getVoltage(int channel = 0) { return voltages[channel]; }
    float getPolyVoltage(int channel) { return isMonophonic() ? getVoltage(0) : getVoltage(channel); }

    float getNormalVoltage(float normalVoltage, int channel = 0) { return isConnected() ? getVoltage(channel) : normalVoltage; }

    float getNormalPolyVoltage(float normalVoltage, int channel) { return isConnected() ? getPolyVoltage(channel) : normalVoltage; }

    float* getVoltages(int firstChannel = 0) { return &voltages[firstChannel]; }

    void readVoltages(float* v) {
        for (int c = 0; c < channels; c++) {
            v[c] = voltages[c];
        }
    }

    void writeVoltages(const float* v) {
        for (int c = 0; c < channels; c++) {
            voltages[c] = v[c];
        }
    }

    void clearVoltages() {
        for (int c = 0; c < channels; c++) {
            voltages[c] = 0.f;
        }
    }

    float getVoltageSum() {
        float sum = 0.f;
        for (int c = 0; c < channels; c++) {
            sum += voltages[c];
        }
        return sum;
    }

    float getVoltageRMS() {
        if (channels == 0) {
            return 0.f;
        }
        else if (channels == 1) {
            return std::fabs(voltages[0]);
        }
        else {
            float sum = 0.f;
            for (int c = 0; c < channels; c++) {
                sum += std::pow(voltages[c], 2);
            }
            return std::sqrt(sum);
        }
    }

//     template <typename T>
//     T getVoltageSimd(int firstChannel) {
//         return T::load(&voltages[firstChannel]);
//     }
// 
//     template <typename T>
//     T getPolyVoltageSimd(int firstChannel) {
//         return isMonophonic() ? getVoltage(0) : getVoltageSimd<T>(firstChannel);
//     }
// 
//     template <typename T>
//     T getNormalVoltageSimd(T normalVoltage, int firstChannel) {
//         return isConnected() ? getVoltageSimd<T>(firstChannel) : normalVoltage;
//     }
// 
//     template <typename T>
//     T getNormalPolyVoltageSimd(T normalVoltage, int firstChannel) {
//         return isConnected() ? getPolyVoltageSimd<T>(firstChannel) : normalVoltage;
//     }
// 
//     template <typename T>
//     void setVoltageSimd(T voltage, int firstChannel) {
//         voltage.store(&voltages[firstChannel]);
//     }

    void setChannels(int channels) {
        if (this->channels == 0) {
            return;
        }
        for (int c = channels; c < this->channels; c++) {
            voltages[c] = 0.f;
        }
        if (channels == 0) {
            channels = 1;
        }
        this->channels = channels;
    }

    int getChannels() { return channels; }
    bool isConnected() { return channels > 0; }
    bool isMonophonic() { return channels == 1; }
    bool isPolyphonic() { return channels > 1; }
    float normalize(float normalVoltage) { return getNormalVoltage(normalVoltage); }
};

struct Output : Port {};

struct Input : Port {};

struct PortInfo {
    Module* module = NULL;
    Port::Type type = Port::INPUT;
    int portId = -1;
    std::string name;
    std::string description;
    virtual ~PortInfo() {}
    virtual std::string getName() {
        if (name == "")
            return std::string("#") + std::to_string(portId + 1);
        return name;
    }
    std::string getFullName() {
        std::string name = getName();
        name += " ";
        name += (type == Port::INPUT) ? "input" : "output";
        return name;
    }
    virtual std::string getDescription() { return description; }
};

struct ParamQuantity : Quantity {
    Module* module = NULL;
    int paramId = -1;
    float minValue = 0.f;
    float maxValue = 1.f;
    float defaultValue = 0.f;
    std::string name;
    std::string unit;
    float displayBase = 0.f;
    float displayMultiplier = 1.f;
    float displayOffset = 0.f;
    int displayPrecision = 5;
    std::string description;
    bool resetEnabled = true;
    bool randomizeEnabled = true;
    bool smoothEnabled = false;
    bool snapEnabled = false;

//     Param* getParam();
//     /** If smoothEnabled is true, requests to the engine to smoothly move to a target value each sample. */
//     void setSmoothValue(float value);
//     float getSmoothValue();

//     void setValue(float value) override;
//     float getValue() override;
    float getMinValue() override { return minValue; }
    float getMaxValue() override { return maxValue; }
    float getDefaultValue() override { return defaultValue; }
//     float getDisplayValue() override;
//     void setDisplayValue(float displayValue) override;
//     std::string getDisplayValueString() override;
//     void setDisplayValueString(std::string s) override;
//     int getDisplayPrecision() override;
//     std::string getLabel() override;
//     std::string getUnit() override;
//     void reset() override;
//     void randomize() override;

    virtual std::string getDescription() { return description; }

//     virtual json_t* toJson();
//     virtual void fromJson(json_t* rootJ);
};

struct SwitchQuantity : ParamQuantity {
//     std::vector<std::string> labels;
//     std::string getDisplayValueString() override;
//     void setDisplayValueString(std::string s) override;
};

struct Module {
    std::vector<Param> params;
    std::vector<Input> inputs;
    std::vector<Output> outputs;
    std::vector<Light> lights;
    std::vector<ParamQuantity*> paramQuantities;
    std::vector<PortInfo*> inputInfos;
    std::vector<PortInfo*> outputInfos;
//     std::vector<LightInfo*> lightInfos;

    virtual ~Module() {
        for (ParamQuantity* paramQuantity : paramQuantities) {
            if (paramQuantity)
                delete paramQuantity;
        }
        for (PortInfo* inputInfo : inputInfos) {
            if (inputInfo)
                delete inputInfo;
        }
        for (PortInfo* outputInfo : outputInfos) {
            if (outputInfo)
                delete outputInfo;
        }
//         for (LightInfo* lightInfo : lightInfos) {
//             if (lightInfo)
//                 delete lightInfo;
//         }
    }

    void config(int numParams, int numInputs, int numOutputs, int numLights = 0) {
//         // This method should only be called once.
//         assert(params.empty() && inputs.empty() && outputs.empty() && lights.empty() && paramQuantities.empty());
        params.resize(numParams);
        inputs.resize(numInputs);
        outputs.resize(numOutputs);
        lights.resize(numLights);
        paramQuantities.resize(numParams);
        for (int i = 0; i < numParams; i++) {
            configParam(i, 0.f, 1.f, 0.f);
        }
        inputInfos.resize(numInputs);
        for (int i = 0; i < numInputs; i++) {
            configInput(i);
        }
        outputInfos.resize(numOutputs);
        for (int i = 0; i < numOutputs; i++) {
            configOutput(i);
        }
//         lightInfos.resize(numLights);
    } 

    template <class TParamQuantity = ParamQuantity>
    TParamQuantity* configParam(int paramId, float minValue, float maxValue, float defaultValue, std::string name = "", std::string unit = "", float displayBase = 0.f, float displayMultiplier = 1.f, float displayOffset = 0.f) {
//         assert(paramId < (int) params.size() && paramId < (int) paramQuantities.size());
        if (paramQuantities[paramId])
            delete paramQuantities[paramId];

        TParamQuantity* q = new TParamQuantity;
        q->ParamQuantity::module = this;
        q->ParamQuantity::paramId = paramId;
        q->ParamQuantity::minValue = minValue;
        q->ParamQuantity::maxValue = maxValue;
        q->ParamQuantity::defaultValue = defaultValue;
        q->ParamQuantity::name = name;
        q->ParamQuantity::unit = unit;
        q->ParamQuantity::displayBase = displayBase;
        q->ParamQuantity::displayMultiplier = displayMultiplier;
        q->ParamQuantity::displayOffset = displayOffset;
        paramQuantities[paramId] = q;

        Param* p = &params[paramId];
        p->value = q->getDefaultValue();
        return q;
    }

    template <class TSwitchQuantity = SwitchQuantity>
    TSwitchQuantity* configSwitch(int paramId, float minValue, float maxValue, float defaultValue, std::string name = "", std::vector<std::string> labels = {}) {
        TSwitchQuantity* sq = configParam<TSwitchQuantity>(paramId, minValue, maxValue, defaultValue, name);
        sq->labels = labels;
        return sq;
    }

    template <class TSwitchQuantity = SwitchQuantity>
    TSwitchQuantity* configButton(int paramId, std::string name = "") {
        TSwitchQuantity* sq = configParam<TSwitchQuantity>(paramId, 0.f, 1.f, 0.f, name);
        sq->randomizeEnabled = false;
        return sq;
    }

    template <class TPortInfo = PortInfo>
    TPortInfo* configInput(int portId, std::string name = "") {
//         assert(portId < (int) inputs.size() && portId < (int) inputInfos.size());
        if (inputInfos[portId])
            delete inputInfos[portId];

        TPortInfo* info = new TPortInfo;
        info->PortInfo::module = this;
        info->PortInfo::type = Port::INPUT;
        info->PortInfo::portId = portId;
        info->PortInfo::name = name;
        inputInfos[portId] = info;
        return info;
    }

    template <class TPortInfo = PortInfo>
    TPortInfo* configOutput(int portId, std::string name = "") {
//         assert(portId < (int) outputs.size() && portId < (int) outputInfos.size());
        if (outputInfos[portId])
            delete outputInfos[portId];

        TPortInfo* info = new TPortInfo;
        info->PortInfo::module = this;
        info->PortInfo::type = Port::OUTPUT;
        info->PortInfo::portId = portId;
        info->PortInfo::name = name;
        outputInfos[portId] = info;
        return info;
    }

//     template <class TLightInfo = LightInfo>
//     TLightInfo* configLight(int lightId, std::string name = "") {
//         assert(lightId < (int) lights.size() && lightId < (int) lightInfos.size());
//         if (lightInfos[lightId])
//             delete lightInfos[lightId];
// 
//         TLightInfo* info = new TLightInfo;
//         info->LightInfo::module = this;
//         info->LightInfo::lightId = lightId;
//         info->LightInfo::name = name;
//         lightInfos[lightId] = info;
//         return info;
//     }

    void configBypass(int inputId, int outputId) {
//         assert(inputId < (int) inputs.size());
//         assert(outputId < (int) outputs.size());
//         // Check that output is not yet routed
//         for (BypassRoute& br : bypassRoutes) {
//             assert(br.outputId != outputId);
//         }
// 
//         BypassRoute br;
//         br.inputId = inputId;
//         br.outputId = outputId;
//         bypassRoutes.push_back(br);
    }

    int getNumParams() { return params.size(); }
    Param& getParam(int index) { return params[index]; }
    int getNumInputs() { return inputs.size(); }
    Input& getInput(int index) { return inputs[index]; }
    int getNumOutputs() { return outputs.size(); }
    Output& getOutput(int index) { return outputs[index]; }
    int getNumLights() { return lights.size(); }
    Light& getLight(int index) { return lights[index]; }
    ParamQuantity* getParamQuantity(int index) { return paramQuantities[index]; }
    PortInfo* getInputInfo(int index) { return inputInfos[index]; }
    PortInfo* getOutputInfo(int index) { return outputInfos[index]; }
//     LightInfo* getLightInfo(int index) { return lightInfos[index]; }

    struct ProcessArgs {
        float sampleRate;
        float sampleTime;
        int64_t frame;
    };
    virtual void process(const ProcessArgs& args) {
        step();
    }
    virtual void step() {}

    // virtual void processBypass(const ProcessArgs& args);

    // virtual json_t* toJson();
    // virtual void fromJson(json_t* rootJ);

    /** Serializes the "params" object. */
    // virtual json_t* paramsToJson();
    // virtual void paramsFromJson(json_t* rootJ);

    virtual json_t* dataToJson() { return NULL; }
    virtual void dataFromJson(json_t* rootJ) {}

    struct SampleRateChangeEvent {
        float sampleRate;
        float sampleTime;
    };
    virtual void onSampleRateChange(const SampleRateChangeEvent& e) {
        onSampleRateChange();
    }

    struct ResetEvent {};
    virtual void onReset(const ResetEvent& e) {} // TODO

    virtual void onAdd() {}
    virtual void onRemove() {}
    virtual void onReset() {}
    virtual void onRandomize() {}
    virtual void onSampleRateChange() {}

    // private
    void doProcess(const ProcessArgs& args) {
//         if (!internal->bypassed)
            process(args);
//         else
//             processBypass(args);
 
//         if (args.frame % PORT_DIVIDER == 0) {
//             float portTime = args.sampleTime * PORT_DIVIDER;
//             for (Input& input : inputs) {
//                 Port_step(&input, portTime);
//             }
//             for (Output& output : outputs) {
//                 Port_step(&output, portTime);
//             }
//         }
    }
};

} // namespace engine

namespace widget {

struct BaseEvent {
};

struct Widget {
    math::Rect box;
    Widget* parent = NULL;
    std::list<Widget*> children;
    bool visible = true;
    bool requestedDelete = false;

    using BaseEvent = widget::BaseEvent;

    struct ActionEvent : BaseEvent {};
    virtual void onAction(const ActionEvent& e) {}

    struct ChangeEvent : BaseEvent {};
    virtual void onChange(const ChangeEvent& e) {}

    bool hasChild(Widget* child) { return false; }
    void addChild(Widget* child) {}
    void addChildBottom(Widget* child) {}
    void addChildBelow(Widget* child, Widget* sibling) {}
    void addChildAbove(Widget* child, Widget* sibling) {}
    void removeChild(Widget* child) {}
    void clearChildren() {}
    virtual void step() {}
    struct DrawArgs {
        void* vg = NULL;
        math::Rect clipBox;
        void* fb = NULL;
    };
	virtual void draw(const DrawArgs& args);
};

struct OpaqueWidget : Widget {
};

struct SvgWidget : Widget {
    void wrap() {}
    void setSvg(void* svg) {}
};

struct TransparentWidget : Widget {
};

} // namespace widget

namespace app {

static constexpr const float RACK_GRID_WIDTH = 15;
static constexpr const float RACK_GRID_HEIGHT = 380;
// static constexpr const math::Vec RACK_GRID_SIZE = math::Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
// static constexpr const math::Vec RACK_OFFSET = RACK_GRID_SIZE.mult(math::Vec(2000, 100));

struct CircularShadow : widget::TransparentWidget {
    float blurRadius;
    float opacity;
};

struct LightWidget : widget::TransparentWidget {
    NVGcolor bgColor, color, borderColor;
};

struct ModuleWidget : widget::OpaqueWidget {
//     plugin::Model* model = NULL;
    engine::Module* module = NULL;
    void setModel(void*) {}
    void setModule(void*) {}
    void setPanel(void*) {}
    void addParam(void*) {}
    void addInput(void*) {}
    void addOutput(void*) {}
	virtual void appendContextMenu(ui::Menu* menu) {}
};

struct MultiLightWidget : LightWidget {
//     std::vector<NVGcolor> baseColors;
//     int getNumColors();
    void addBaseColor(NVGcolor baseColor) {}
//     void setBrightnesses(const std::vector<float>& brightnesses);
};

struct ModuleLightWidget : MultiLightWidget {
//     engine::Module* module = NULL;
//     int firstLightId = -1;
//     ModuleLightWidget();
//     ~ModuleLightWidget();
//     engine::Light* getLight(int colorId);
//     engine::LightInfo* getLightInfo();
//     void createTooltip();
//     void destroyTooltip();
};

struct ParamWidget : widget::OpaqueWidget {
    engine::Module* module = NULL;
    int paramId = -1;
    virtual void initParamQuantity() {}
    engine::ParamQuantity* getParamQuantity() { return module ? module->paramQuantities[paramId] : NULL; }
    void createTooltip() {}
    void destroyTooltip() {}
    void createContextMenu();
    virtual void appendContextMenu(void* menu) {}
    void resetAction();
};

struct PortWidget : widget::OpaqueWidget {
};

struct Knob : ParamWidget {
    bool horizontal = false;
    bool smooth = true;
    bool snap = false;
    float speed = 1.f;
    bool forceLinear = false;
    float minAngle = -M_PI;
    float maxAngle = M_PI;
};

struct SliderKnob : Knob {
};

struct Switch : ParamWidget {
    bool momentary = false;
};

struct SvgKnob : Knob {
    CircularShadow* shadow;
    void setSvg(void* svg) {}
};

struct SvgPanel : widget::Widget {
//     widget::FramebufferWidget* fb;
//     widget::SvgWidget* sw;
//     PanelBorder* panelBorder;
    void setBackground(void* svg) {}
};

struct SvgPort : PortWidget {
//     widget::FramebufferWidget* fb;
    CircularShadow* shadow;
//     widget::SvgWidget* sw;
    void setSvg(void* svg) {}
};

struct SvgScrew : widget::Widget {
//     widget::FramebufferWidget* fb;
    widget::SvgWidget* sw;
    void setSvg(void* svg) {}
};

struct SvgSlider : app::SliderKnob {
//     widget::FramebufferWidget* fb;
    widget::SvgWidget* background;
//     widget::SvgWidget* handle;
    math::Vec minHandlePos, maxHandlePos;
    void setBackgroundSvg(void* svg) {}
    void setHandleSvg(void* svg) {}
    void setHandlePos(math::Vec minHandlePos, math::Vec maxHandlePos) {}
    void setHandlePosCentered(math::Vec minHandlePosCentered, math::Vec maxHandlePosCentered) {}
};

struct SvgSwitch : Switch {
// 	widget::FramebufferWidget* fb;
// 	CircularShadow* shadow;
// 	widget::SvgWidget* sw;
// 	std::vector<std::shared_ptr<window::Svg>> frames;
    bool latch = false;
    void addFrame(void* svg) {}
};

} // namespace app

namespace asset {

const char* plugin(void* instance, const char* path) {
    return NULL;
}

} // namespace asset

namespace componentlibrary {

static constexpr const NVGcolor SCHEME_LIGHT_GRAY = {};

template <typename TBase = app::ModuleLightWidget>
struct TSvgLight : TBase {
//     widget::FramebufferWidget* fb;
//     widget::SvgWidget* sw;
    void setSvg(void* svg) {}
};
using SvgLight = TSvgLight<>;

template <typename TBase = app::ModuleLightWidget>
struct TGrayModuleLightWidget : TBase {
};
using GrayModuleLightWidget = TGrayModuleLightWidget<>;

template <typename TBase = GrayModuleLightWidget>
struct TWhiteLight : TBase {
};
using WhiteLight = TWhiteLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TRedLight : TBase {
};
using RedLight = TRedLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TGreenLight : TBase {
};
using GreenLight = TGreenLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TBlueLight : TBase {
};
using BlueLight = TBlueLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TYellowLight : TBase {
};
using YellowLight = TYellowLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TGreenRedLight : TBase {
};
using GreenRedLight = TGreenRedLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TRedGreenBlueLight : TBase {
};
using RedGreenBlueLight = TRedGreenBlueLight<>;

template <typename TBase>
struct LargeLight : TSvgLight<TBase> {
};

template <typename TBase>
struct MediumLight : TSvgLight<TBase> {
};

template <typename TBase>
struct SmallLight : TSvgLight<TBase> {
};

template <typename TBase>
struct TinyLight : TSvgLight<TBase> {
};

struct ScrewBlack : app::SvgScrew {
};

} // namespace componentlibrary

namespace dsp {
    
inline float sinc(float x) {
    if (x == 0.f)
        return 1.f;
    x *= M_PI;
    return std::sin(x) / x;
}

// template <typename T>
// T sinc(T x) {
//     T zeromask = (x == 0.f);
//     x *= M_PI;
//     x = simd::sin(x) / x;
//     return simd::ifelse(zeromask, 1.f, x);
// }

template <typename T>
inline T blackmanHarris(T p) {
    return
      + T(0.35875)
      - T(0.48829) * std::cos(2 * T(M_PI) * p)
      + T(0.14128) * std::cos(4 * T(M_PI) * p)
      - T(0.01168) * std::cos(6 * T(M_PI) * p);
}

inline void blackmanHarrisWindow(float* x, int len) {
    for (int i = 0; i < len; i++) {
        x[i] *= blackmanHarris(float(i) / (len - 1));
    }
}

inline void boxcarLowpassIR(float* out, int len, float cutoff = 0.5f) {
    for (int i = 0; i < len; i++) {
        float t = i - (len - 1) / 2.f;
        out[i] = 2 * cutoff * sinc(2 * cutoff * t);
    }
}

template <int OVERSAMPLE, int QUALITY, typename T = float>
struct Decimator {
    T inBuffer[OVERSAMPLE * QUALITY];
    float kernel[OVERSAMPLE * QUALITY];
    int inIndex;
    Decimator(float cutoff = 0.9f) {
        boxcarLowpassIR(kernel, OVERSAMPLE * QUALITY, cutoff * 0.5f / OVERSAMPLE);
        blackmanHarrisWindow(kernel, OVERSAMPLE * QUALITY);
        reset();
    }
    void reset() {
        inIndex = 0;
        std::memset(inBuffer, 0, sizeof(inBuffer));
    }
    T process(T* in) {
        std::memcpy(&inBuffer[inIndex], in, OVERSAMPLE * sizeof(T));
        inIndex += OVERSAMPLE;
        inIndex %= OVERSAMPLE * QUALITY;
        T out = 0.f;
        for (int i = 0; i < OVERSAMPLE * QUALITY; i++) {
            int index = inIndex - 1 - i;
            index = (index + OVERSAMPLE * QUALITY) % (OVERSAMPLE * QUALITY);
            out += kernel[i] * inBuffer[index];
        }
        return out;
    }
};

struct PulseGenerator {
    float remaining = 0.f;
    void reset() { remaining = 0.f; }
    bool process(float deltaTime) {
        if (remaining > 0.f) {
            remaining -= deltaTime;
            return true;
        }
        return false;
    }
    void trigger(float duration = 1e-3f) {
        if (duration > remaining) {
            remaining = duration;
        }
    }
};

} // namespace dsp

namespace event {
// using Base = widget::BaseEvent;
// using PositionBase = widget::Widget::PositionBaseEvent;
// using KeyBase = widget::Widget::KeyBaseEvent;
// using TextBase = widget::Widget::TextBaseEvent;
// using Hover = widget::Widget::HoverEvent;
// using Button = widget::Widget::ButtonEvent;
// using DoubleClick = widget::Widget::DoubleClickEvent;
// using HoverKey = widget::Widget::HoverKeyEvent;
// using HoverText = widget::Widget::HoverTextEvent;
// using HoverScroll = widget::Widget::HoverScrollEvent;
// using Enter = widget::Widget::EnterEvent;
// using Leave = widget::Widget::LeaveEvent;
// using Select = widget::Widget::SelectEvent;
// using Deselect = widget::Widget::DeselectEvent;
// using SelectKey = widget::Widget::SelectKeyEvent;
// using SelectText = widget::Widget::SelectTextEvent;
// using DragBase = widget::Widget::DragBaseEvent;
// using DragStart = widget::Widget::DragStartEvent;
// using DragEnd = widget::Widget::DragEndEvent;
// using DragMove = widget::Widget::DragMoveEvent;
// using DragHover = widget::Widget::DragHoverEvent;
// using DragEnter = widget::Widget::DragEnterEvent;
// using DragLeave = widget::Widget::DragLeaveEvent;
// using DragDrop = widget::Widget::DragDropEvent;
// using PathDrop = widget::Widget::PathDropEvent;
using Action = widget::Widget::ActionEvent;
using Change = widget::Widget::ChangeEvent;
// using Dirty = widget::Widget::DirtyEvent;
// using Reposition = widget::Widget::RepositionEvent;
// using Resize = widget::Widget::ResizeEvent;
// using Add = widget::Widget::AddEvent;
// using Remove = widget::Widget::RemoveEvent;
// using Show = widget::Widget::ShowEvent;
// using Hide = widget::Widget::HideEvent;
} // namespace event

namespace plugin {

struct Model {
    virtual ~Model() {}
    virtual engine::Module* createModule() = 0;
};

struct Plugin {
};

} // namespace plugin

namespace ui {

struct Menu : widget::OpaqueWidget {
//     Menu* parentMenu = NULL;
//     Menu* childMenu = NULL;
//     MenuEntry* activeEntry = NULL;
//     BNDcornerFlags cornerFlags = BND_CORNER_NONE;
//     void setChildMenu(Menu* menu) {}
};

struct MenuEntry : widget::OpaqueWidget {
};

struct MenuItem : MenuEntry {
    std::string text;
    std::string rightText;
    bool disabled = false;
};

struct MenuLabel : MenuEntry {
    std::string text;
};

} // namespace ui

namespace window {

static constexpr const float SVG_DPI = 75.f;
static constexpr const float MM_PER_IN = 25.4f;

inline float in2px(float in) { return in * SVG_DPI; }
inline math::Vec in2px(math::Vec in) { return in.mult(SVG_DPI); }
inline float mm2px(float mm) { return mm * (SVG_DPI / MM_PER_IN); }
inline math::Vec mm2px(math::Vec mm) { return mm.mult(SVG_DPI / MM_PER_IN); }

struct Image {
    int handle;
};

struct Window {
    std::shared_ptr<Image> loadImage(const std::string&) { return {}; }
    void* loadSvg(const void*) { return NULL; }
};

};

using namespace app;
using namespace componentlibrary;
using namespace engine;
using namespace math;
using namespace ui;
using namespace widget;
using namespace window;
using plugin::Plugin;
using plugin::Model;

template <class TModule, class TModuleWidget>
plugin::Model* createModel(std::string slug) {
    struct TModel : plugin::Model {
        engine::Module* createModule() override {
            return new TModule;
        }
    };
    return new TModel;
}

template <typename T>
T* construct() { return NULL; }

template <typename T, typename F, typename V, typename... Args>
T* construct(F f, V v, Args... args) { return NULL; }

template <class TWidget>
TWidget* createWidget(math::Vec pos) {
    return NULL;
}

template <class TParamWidget>
TParamWidget* createParam(math::Vec pos, engine::Module* module, int paramId) {
    return NULL;
}

template <class TParamWidget>
TParamWidget* createParamCentered(math::Vec pos, engine::Module* module, int paramId) {
    return NULL;
}

template <class TPortWidget>
TPortWidget* createInput(math::Vec pos, engine::Module* module, int inputId) {
    return NULL;
}

template <class TPortWidget>
TPortWidget* createInputCentered(math::Vec pos, engine::Module* module, int inputId) {
    return NULL;
}

template <class TPortWidget>
TPortWidget* createOutput(math::Vec pos, engine::Module* module, int outputId) {
    return NULL;
}

template <class TPortWidget>
TPortWidget* createOutputCentered(math::Vec pos, engine::Module* module, int outputId) {
    return NULL;
}

template <class TModuleLightWidget>
TModuleLightWidget* createLight(math::Vec pos, engine::Module* module, int firstLightId) {
    return NULL;
}

template <class TModuleLightWidget>
TModuleLightWidget* createLightCentered(math::Vec pos, engine::Module* module, int firstLightId) {
    return NULL;
}

struct Context {
    engine::Engine _engine;
    window::Window _window;
    engine::Engine* engine = &_engine;
    window::Window* window = &_window;
};

Context* contextGet();
void contextSet(Context* context);

} // namespace rack

#define APP rack::contextGet()
