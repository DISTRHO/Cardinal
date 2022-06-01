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

#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#define assert(x)

#define ENUMS(name, count) name, name ## _LAST = name + (count) - 1

#define VEC_ARGS(v) (v).x, (v).y
#define RECT_ARGS(r) (r).pos.x, (r).pos.y, (r).size.x, (r).size.y

#define GLFW_MOUSE_BUTTON_LEFT 0
#define GLFW_MOUSE_BUTTON_RIGHT 0

enum NVGalign {
    NVG_ALIGN_LEFT,
    NVG_ALIGN_RIGHT
};
struct NVGcolor { float a; };
struct NVGpaint {};
struct NVGcontext {};
struct NSVGimage {};

inline NVGcolor nvgRGB(int r, int g, int b) { return {}; }
inline NVGcolor nvgRGBA(int r, int g, int b, int a) { return {}; }
inline NVGcolor nvgRGBf(float r, float g, float b) { return {}; }
inline NVGcolor nvgRGBAf(float r, float g, float b, float a) { return {}; }
inline NVGcolor nvgTransRGBA(NVGcolor, int) { return {}; }
inline NVGcolor nvgTransRGBAf(NVGcolor, float) { return {}; }
inline void nvgBeginPath(NVGcontext* vg) {}
inline void nvgFillColor(NVGcontext* vg, NVGcolor) {}
inline void nvgFillPaint(NVGcontext* vg, NVGpaint) {}
inline void nvgFill(NVGcontext* vg) {}
inline void nvgStrokeColor(NVGcontext* vg, NVGcolor) {}
inline void nvgStrokeWidth(NVGcontext* vg, float) {}
inline void nvgStroke(NVGcontext* vg) {}
inline void nvgRect(NVGcontext* vg, float a, float b, float c, float d) {}
inline void nvgImageSize(NVGcontext*, int, void*, void*) {}
inline NVGpaint nvgImagePattern(NVGcontext*, float, float, float, float, float, int handle, float) { return {}; }
inline void nvgRoundedRect(NVGcontext* vg, float, float, float, float, float) {}
inline void nvgFontSize(NVGcontext*, int) {}
inline void nvgFontFaceId(NVGcontext*, int) {}
inline void nvgTextLetterSpacing(NVGcontext*, float) {}
inline void nvgText(NVGcontext*, float, float, const char*, const char*) {}
inline void nvgTextAlign(NVGcontext*, NVGalign) {}
inline void nvgSave(NVGcontext*) {}
inline void nvgRestore(NVGcontext*) {}
inline void nvgScale(NVGcontext*, float, float) {}

struct json_t {};
inline json_t* json_boolean(bool) { return NULL; }
inline json_t* json_integer(int) { return NULL; }
inline json_t* json_object(void) { return NULL; }
inline json_t* json_object_get(json_t*, const char*) { return NULL; }
inline bool json_is_true(json_t*) { return false; }
inline bool json_boolean_value(json_t*) { return false; }
inline int json_integer_value(json_t*) { return 0; }
inline float json_number_value(json_t*) { return 0.f; }
inline void json_object_set_new(json_t*, const char*, json_t*) {}
inline json_t* json_array(void) { return NULL; }
inline json_t* json_array_get(json_t*, int) { return NULL; }
inline void json_array_insert_new(json_t*, int, json_t*) {}

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

namespace window {
struct Svg;
}

namespace math {

template <typename T>
inline bool isEven(T x) { return x % 2 == 0; }

template <typename T>
inline bool isOdd(T x) { return x % 2 != 0; }

inline int clamp(int x, int a, int b) { return std::max(std::min(x, b), a); }
inline int clampSafe(int x, int a, int b) { return (a <= b) ? clamp(x, a, b) : clamp(x, b, a); }

inline float clamp(float x, float a = 0.f, float b = 1.f) { return std::fmax(std::fmin(x, b), a); }
inline float clampSafe(float x, float a = 0.f, float b = 1.f) { return (a <= b) ? clamp(x, a, b) : clamp(x, b, a); }

inline int eucMod(int a, int b) {
    int mod = a % b;
    if (mod < 0) {
        mod += b;
    }
    return mod;
}

inline int eucDiv(int a, int b) {
    int div = a / b;
    int mod = a % b;
    if (mod < 0) {
        div -= 1;
    }
    return div;
}

inline void eucDivMod(int a, int b, int* div, int* mod) {
    *div = a / b;
    *mod = a % b;
    if (*mod < 0) {
        *div -= 1;
        *mod += b;
    }
}

inline int log2(int n) {
    int i = 0;
    while (n >>= 1) {
        i++;
    }
    return i;
}

template <typename T>
bool isPow2(T n) { return n > 0 && (n & (n - 1)) == 0; }

template <typename T>
T sgn(T x) { return x > 0 ? 1 : (x < 0 ? -1 : 0); }

#if defined __clang__
__attribute__((optnone))
#else
__attribute__((optimize("signed-zeros")))
#endif
inline float normalizeZero(float x) { return x + 0.f; }

inline float eucMod(float a, float b) {
    float mod = std::fmod(a, b);
    if (mod < 0.f) {
        mod += b;
    }
    return mod;
}

inline bool isNear(float a, float b, float epsilon = 1e-6f) { return std::fabs(a - b) <= epsilon; }

inline float chop(float x, float epsilon = 1e-6f) { return std::fabs(x) <= epsilon ? 0.f : x; }

inline float rescale(float x, float xMin, float xMax, float yMin, float yMax) {
    return yMin + (x - xMin) / (xMax - xMin) * (yMax - yMin);
}

inline float crossfade(float a, float b, float p) { return a + (b - a) * p; }

inline float interpolateLinear(const float* p, float x) {
    const int xi = x;
    const float xf = x - xi;
    return crossfade(p[xi], p[xi + 1], xf);
}

inline void complexMult(float ar, float ai, float br, float bi, float* cr, float* ci) {
    *cr = ar * br - ai * bi;
    *ci = ar * bi + ai * br;
}

struct Rect;

struct Vec {
    float x = 0.f;
    float y = 0.f;
    Vec() {}
    Vec(float xy) : x(xy), y(xy) {}
    Vec(float x, float y) : x(x), y(y) {}
    float& operator[](int i) { return (i == 0) ? x : y; }
    const float& operator[](int i) const { return (i == 0) ? x : y; }
    Vec neg() const { return Vec(-x, -y); }
    Vec plus(Vec b) const { return Vec(x + b.x, y + b.y); }
    Vec minus(Vec b) const { return Vec(x - b.x, y - b.y); }
    Vec mult(float s) const { return Vec(x * s, y * s); }
    Vec mult(Vec b) const { return Vec(x * b.x, y * b.y); }
    Vec div(float s) const { return Vec(x / s, y / s); }
    Vec div(Vec b) const { return Vec(x / b.x, y / b.y); }
    float dot(Vec b) const { return x * b.x + y * b.y; }
    float arg() const { return std::atan2(y, x); }
    float norm() const { return std::hypot(x, y); }
    Vec normalize() const { return div(norm()); }
    float square() const { return x * x + y * y; }
    float area() const { return x * y; }
    Vec rotate(float angle) {
        float sin = std::sin(angle);
        float cos = std::cos(angle);
        return Vec(x * cos - y * sin, x * sin + y * cos);
    }
    Vec flip() const { return Vec(y, x); }
    Vec min(Vec b) const { return Vec(std::fmin(x, b.x), std::fmin(y, b.y)); }
    Vec max(Vec b) const { return Vec(std::fmax(x, b.x), std::fmax(y, b.y)); }
    Vec abs() const { return Vec(std::fabs(x), std::fabs(y)); }
    Vec round() const { return Vec(std::round(x), std::round(y)); }
    Vec floor() const { return Vec(std::floor(x), std::floor(y)); }
    Vec ceil() const { return Vec(std::ceil(x), std::ceil(y)); }
    bool equals(Vec b) const { return x == b.x && y == b.y; }
    bool isZero() const { return x == 0.f && y == 0.f; }
    bool isFinite() const { return std::isfinite(x) && std::isfinite(y); }
    Vec clamp(Rect bound) const;
    Vec clampSafe(Rect bound) const;
    Vec crossfade(Vec b, float p) { return this->plus(b.minus(*this).mult(p)); }
    bool isEqual(Vec b) const { return equals(b); }
};

struct Rect {
    Vec pos;
    Vec size;
    Rect() {}
    Rect(Vec pos, Vec size) : pos(pos), size(size) {}
    Rect(float posX, float posY, float sizeX, float sizeY) : pos(Vec(posX, posY)), size(Vec(sizeX, sizeY)) {}
    static Rect fromMinMax(Vec a, Vec b) { return Rect(a, b.minus(a)); }
    static Rect fromCorners(Vec a, Vec b) { return fromMinMax(a.min(b), a.max(b)); }
    static Rect inf() { return Rect(Vec(-INFINITY, -INFINITY), Vec(INFINITY, INFINITY)); }
    bool contains(Vec v) const {
        return (pos.x <= v.x) && (size.x == INFINITY || v.x < pos.x + size.x)
            && (pos.y <= v.y) && (size.y == INFINITY || v.y < pos.y + size.y);
    }
    bool contains(Rect r) const {
        return (pos.x <= r.pos.x) && (r.pos.x - size.x <= pos.x - r.size.x)
            && (pos.y <= r.pos.y) && (r.pos.y - size.y <= pos.y - r.size.y);
    }
    bool intersects(Rect r) const {
        return (r.size.x == INFINITY || pos.x < r.pos.x + r.size.x) && (size.x == INFINITY || r.pos.x < pos.x + size.x)
            && (r.size.y == INFINITY || pos.y < r.pos.y + r.size.y) && (size.y == INFINITY || r.pos.y < pos.y + size.y);
    }
    bool equals(Rect r) const { return pos.equals(r.pos) && size.equals(r.size); }
    float getLeft() const { return pos.x; }
    float getRight() const { return (size.x == INFINITY) ? INFINITY : (pos.x + size.x); }
    float getTop() const { return pos.y; }
    float getBottom() const { return (size.y == INFINITY) ? INFINITY : (pos.y + size.y); }
    float getWidth() const { return size.x; }
    float getHeight() const { return size.y; }
    Vec getCenter() const { return pos.plus(size.mult(0.5f)); }
    Vec getTopLeft() const { return pos; }
    Vec getTopRight() const { return Vec(getRight(), getTop()); }
    Vec getBottomLeft() const { return Vec(getLeft(), getBottom()); }
    Vec getBottomRight() const { return Vec(getRight(), getBottom()); }
    Rect clamp(Rect bound) const {
        Rect r;
        r.pos.x = math::clampSafe(pos.x, bound.pos.x, bound.pos.x + bound.size.x);
        r.pos.y = math::clampSafe(pos.y, bound.pos.y, bound.pos.y + bound.size.y);
        r.size.x = math::clamp(pos.x + size.x, bound.pos.x, bound.pos.x + bound.size.x) - r.pos.x;
        r.size.y = math::clamp(pos.y + size.y, bound.pos.y, bound.pos.y + bound.size.y) - r.pos.y;
        return r;
    }
    Rect nudge(Rect bound) const {
        Rect r;
        r.size = size;
        r.pos.x = math::clampSafe(pos.x, bound.pos.x, bound.pos.x + bound.size.x - size.x);
        r.pos.y = math::clampSafe(pos.y, bound.pos.y, bound.pos.y + bound.size.y - size.y);
        return r;
    }
    Rect expand(Rect b) const {
        Rect r;
        r.pos.x = std::fmin(pos.x, b.pos.x);
        r.pos.y = std::fmin(pos.y, b.pos.y);
        r.size.x = std::fmax(pos.x + size.x, b.pos.x + b.size.x) - r.pos.x;
        r.size.y = std::fmax(pos.y + size.y, b.pos.y + b.size.y) - r.pos.y;
        return r;
    }
    Rect intersect(Rect b) const {
        Rect r;
        r.pos.x = std::fmax(pos.x, b.pos.x);
        r.pos.y = std::fmax(pos.y, b.pos.y);
        r.size.x = std::fmin(pos.x + size.x, b.pos.x + b.size.x) - r.pos.x;
        r.size.y = std::fmin(pos.y + size.y, b.pos.y + b.size.y) - r.pos.y;
        return r;
    }
    Rect zeroPos() const {
        return Rect(Vec(), size);
    }
    Rect grow(Vec delta) const {
        Rect r;
        r.pos = pos.minus(delta);
        r.size = size.plus(delta.mult(2.f));
        return r;
    }
    Rect shrink(Vec delta) const {
        Rect r;
        r.pos = pos.plus(delta);
        r.size = size.minus(delta.mult(2.f));
        return r;
    }
    Vec interpolate(Vec p) {
        return pos.plus(size.mult(p));
    }
    bool isContaining(Vec v) const { return contains(v); }
    bool isIntersecting(Rect r) const { return intersects(r); }
    bool isEqual(Rect r) const { return equals(r); }
};

inline Vec Vec::clamp(Rect bound) const {
    return Vec(math::clamp(x, bound.pos.x, bound.pos.x + bound.size.x),
               math::clamp(y, bound.pos.y, bound.pos.y + bound.size.y));
}

inline Vec Vec::clampSafe(Rect bound) const {
    return Vec(math::clampSafe(x, bound.pos.x, bound.pos.x + bound.size.x),
               math::clampSafe(y, bound.pos.y, bound.pos.y + bound.size.y));
}
inline Vec operator+(const Vec& a) { return a; }
inline Vec operator-(const Vec& a) { return a.neg(); }
inline Vec operator+(const Vec& a, const Vec& b) { return a.plus(b); }
inline Vec operator-(const Vec& a, const Vec& b) { return a.minus(b); }
inline Vec operator*(const Vec& a, const Vec& b) { return a.mult(b); }
inline Vec operator*(const Vec& a, const float& b) { return a.mult(b); }
inline Vec operator*(const float& a, const Vec& b) { return b.mult(a); }
inline Vec operator/(const Vec& a, const Vec& b) { return a.div(b); }
inline Vec operator/(const Vec& a, const float& b) { return a.div(b); }
inline Vec operator+=(Vec& a, const Vec& b) { return a = a.plus(b); }
inline Vec operator-=(Vec& a, const Vec& b) { return a = a.minus(b); }
inline Vec operator*=(Vec& a, const Vec& b) { return a = a.mult(b); }
inline Vec operator*=(Vec& a, const float& b) { return a = a.mult(b); }
inline Vec operator/=(Vec& a, const Vec& b) { return a = a.div(b); }
inline Vec operator/=(Vec& a, const float& b) { return a = a.div(b); }
inline bool operator==(const Vec& a, const Vec& b) { return a.equals(b); }
inline bool operator!=(const Vec& a, const Vec& b) { return !a.equals(b); }

inline bool operator==(const Rect& a, const Rect& b) { return a.equals(b); }
inline bool operator!=(const Rect& a, const Rect& b) { return !a.equals(b); }

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

struct LightInfo {
    Module* module = NULL;
    int lightId = -1;
    std::string name;
    std::string description;
    virtual ~LightInfo() {}
    virtual std::string getName() { return name; }
    virtual std::string getDescription() { return description; }
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
    float getNormalVoltage(float normalVoltage, int channel = 0) {
        return isConnected() ? getVoltage(channel) : normalVoltage;
    }
    float getNormalPolyVoltage(float normalVoltage, int channel) {
        return isConnected() ? getPolyVoltage(channel) : normalVoltage;
    }
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
    std::vector<LightInfo*> lightInfos;
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
        for (LightInfo* lightInfo : lightInfos) {
            if (lightInfo)
                delete lightInfo;
        }
    }
    void config(int numParams, int numInputs, int numOutputs, int numLights = 0) {
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
        lightInfos.resize(numLights);
    } 
    template <class TParamQuantity = ParamQuantity>
    TParamQuantity* configParam(int paramId, float minValue, float maxValue, float defaultValue, std::string name = "", std::string unit = "", float displayBase = 0.f, float displayMultiplier = 1.f, float displayOffset = 0.f) {
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
    template <class TLightInfo = LightInfo>
    TLightInfo* configLight(int lightId, std::string name = "") {
        if (lightInfos[lightId])
            delete lightInfos[lightId];

        TLightInfo* info = new TLightInfo;
        info->LightInfo::module = this;
        info->LightInfo::lightId = lightId;
        info->LightInfo::name = name;
        lightInfos[lightId] = info;
        return info;
    }
    void configBypass(int inputId, int outputId) {
//         // Check that output is not yet routed
//         for (BypassRoute& br : bypassRoutes) {
//             assert(br.outputId != outputId);
//         }
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
    LightInfo* getLightInfo(int index) { return lightInfos[index]; }
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
    // virtual json_t* paramsToJson();
    // virtual void paramsFromJson(json_t* rootJ);
    virtual json_t* dataToJson() { return NULL; }
    virtual void dataFromJson(json_t* rootJ) {}
    struct SampleRateChangeEvent {
        float sampleRate;
        float sampleTime;
    };
    virtual void onSampleRateChange(const SampleRateChangeEvent&) {
        onSampleRateChange();
    }
    struct ResetEvent {};
    virtual void onReset(const ResetEvent&) {} // TODO
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
    bool visible = false;
    bool requestedDelete = false;

    virtual ~Widget() {}
    math::Rect getBox() { return {}; }
    void setBox(math::Rect box) {}
    math::Vec getPosition() { return {}; }
    void setPosition(math::Vec pos) {}
    math::Vec getSize() { return {}; }
    void setSize(math::Vec size) {}
    widget::Widget* getParent() { return NULL; }
    bool isVisible() { return false; }
    void setVisible(bool visible) {}
    void show() {}
    void hide() {}
    void requestDelete() {}

    virtual math::Rect getChildrenBoundingBox() { return {}; }
    virtual math::Rect getVisibleChildrenBoundingBox() { return {}; }
    bool isDescendantOf(Widget* ancestor) { return false; }
    virtual math::Vec getRelativeOffset(math::Vec v, Widget* ancestor) { return {}; }
    math::Vec getAbsoluteOffset(math::Vec v) { return {}; }
    virtual float getRelativeZoom(Widget* ancestor) { return 0.f; }
    float getAbsoluteZoom() { return 0.f; }
    virtual math::Rect getViewport(math::Rect r) { return {}; }

    template <class T> T* getAncestorOfType() { return NULL; }
    template <class T> T* getFirstDescendantOfType() { return NULL; }

    bool hasChild(Widget* child) { return false; }
    void addChild(Widget* child) {}
    void addChildBottom(Widget* child) {}
    void addChildBelow(Widget* child, Widget* sibling) {}
    void addChildAbove(Widget* child, Widget* sibling) {}
    void removeChild(Widget* child) {}
    void clearChildren() {}

    virtual void step() {}
    struct DrawArgs {
        NVGcontext* vg = NULL;
        math::Rect clipBox;
        void* fb = NULL;
    };
    virtual void draw(const DrawArgs&) {}
    // DEPRECATED virtual void draw(NVGcontext* vg) {}
    virtual void drawLayer(const DrawArgs& args, int layer) {}
    void drawChild(Widget* child, const DrawArgs& args, int layer = 0) {}

    using BaseEvent = widget::BaseEvent;

    struct PositionBaseEvent {
        math::Vec pos;
    };

    struct HoverEvent : BaseEvent, PositionBaseEvent {
        math::Vec mouseDelta;
    };
    virtual void onHover(const HoverEvent&) {}

    struct ButtonEvent : BaseEvent, PositionBaseEvent {
        int button;
        int action;
        int mods;
    };
    virtual void onButton(const ButtonEvent&) {}

    struct DoubleClickEvent : BaseEvent {};
    virtual void onDoubleClick(const DoubleClickEvent&) {}

    struct KeyBaseEvent {
        int key;
        int scancode;
        std::string keyName;
        int action;
        int mods;
    };

    struct HoverKeyEvent : BaseEvent, PositionBaseEvent, KeyBaseEvent {};
    virtual void onHoverKey(const HoverKeyEvent&) {}

    struct TextBaseEvent {
        int codepoint;
    };
    struct HoverTextEvent : BaseEvent, PositionBaseEvent, TextBaseEvent {};
    virtual void onHoverText(const HoverTextEvent&) {}

    struct HoverScrollEvent : BaseEvent, PositionBaseEvent {
        math::Vec scrollDelta;
    };
    virtual void onHoverScroll(const HoverScrollEvent&) {}

    struct EnterEvent : BaseEvent {};
    virtual void onEnter(const EnterEvent&) {}

    struct LeaveEvent : BaseEvent {};
    virtual void onLeave(const LeaveEvent&) {}

    struct SelectEvent : BaseEvent {};
    virtual void onSelect(const SelectEvent&) {}

    struct DeselectEvent : BaseEvent {};
    virtual void onDeselect(const DeselectEvent&) {}

    struct SelectKeyEvent : BaseEvent, KeyBaseEvent {};
    virtual void onSelectKey(const SelectKeyEvent&) {}

    struct SelectTextEvent : BaseEvent, TextBaseEvent {};
    virtual void onSelectText(const SelectTextEvent&) {}

    struct DragBaseEvent : BaseEvent {
        int button;
    };

    struct DragStartEvent : DragBaseEvent {};
    virtual void onDragStart(const DragStartEvent&) {}

    struct DragEndEvent : DragBaseEvent {};
    virtual void onDragEnd(const DragEndEvent&) {}

    struct DragMoveEvent : DragBaseEvent {
        math::Vec mouseDelta;
    };
    virtual void onDragMove(const DragMoveEvent&) {}

    struct DragHoverEvent : DragBaseEvent, PositionBaseEvent {
        Widget* origin = NULL;
        math::Vec mouseDelta;
    };
    virtual void onDragHover(const DragHoverEvent&) {
    }

    struct DragEnterEvent : DragBaseEvent {
        Widget* origin = NULL;
    };
    virtual void onDragEnter(const DragEnterEvent&) {}

    struct DragLeaveEvent : DragBaseEvent {
        Widget* origin = NULL;
    };
    virtual void onDragLeave(const DragLeaveEvent&) {}

    struct DragDropEvent : DragBaseEvent {
        Widget* origin = NULL;
    };
    virtual void onDragDrop(const DragDropEvent&) {}

    struct PathDropEvent : BaseEvent, PositionBaseEvent {
        PathDropEvent(const std::vector<std::string>& paths) : paths(paths) {}
        const std::vector<std::string>& paths;
    };
    virtual void onPathDrop(const PathDropEvent&) {}

    struct ActionEvent : BaseEvent {};
    virtual void onAction(const ActionEvent&) {}

    struct ChangeEvent : BaseEvent {};
    virtual void onChange(const ChangeEvent&) {}

    struct DirtyEvent : BaseEvent {};
    virtual void onDirty(const DirtyEvent&) {}

    struct RepositionEvent : BaseEvent {};
    virtual void onReposition(const RepositionEvent&) {}

    struct ResizeEvent : BaseEvent {};
    virtual void onResize(const ResizeEvent&) {}

    struct AddEvent : BaseEvent {};
    virtual void onAdd(const AddEvent&) {}

    struct RemoveEvent : BaseEvent {};
    virtual void onRemove(const RemoveEvent&) {}

    struct ShowEvent : BaseEvent {};
    virtual void onShow(const ShowEvent&) {}

    struct HideEvent : BaseEvent {};
    virtual void onHide(const HideEvent&) {}

    struct ContextCreateEvent : BaseEvent {
        NVGcontext* vg;
    };
    virtual void onContextCreate(const ContextCreateEvent&) {}

    struct ContextDestroyEvent : BaseEvent {
        NVGcontext* vg;
    };
    virtual void onContextDestroy(const ContextDestroyEvent&) {}
};

struct OpaqueWidget : Widget {
};

struct SvgWidget : Widget {
    std::shared_ptr<window::Svg> svg;
    void wrap() {}
    void setSvg(std::shared_ptr<window::Svg>) {}
};

struct TransparentWidget : Widget {
};

} // namespace widget

namespace app {

static constexpr const float RACK_GRID_WIDTH = 15;
static constexpr const float RACK_GRID_HEIGHT = 380;
static const math::Vec RACK_GRID_SIZE = math::Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
static const math::Vec RACK_OFFSET = RACK_GRID_SIZE.mult(math::Vec(2000, 100));

struct ParamWidget;
struct PortWidget;

struct CircularShadow : widget::TransparentWidget {
    float blurRadius;
    float opacity;
};

struct LightWidget : widget::TransparentWidget {
    NVGcolor bgColor, color, borderColor;
};

struct ModuleWidget : widget::OpaqueWidget {
    // plugin::Model* model = NULL;
    engine::Module* module = NULL;

    // plugin::Model* getModel() { return NULL; }
    // void setModel(plugin::Model*) {}

    engine::Module* getModule() { return NULL; }
    void setModule(engine::Module*) {}

    widget::Widget* getPanel() { return NULL; }
    void setPanel(widget::Widget*) {}
    void setPanel(std::shared_ptr<window::Svg>) {}

    void addParam(ParamWidget*) {}
    void addInput(PortWidget*) {}
    void addOutput(PortWidget*) {}
    ParamWidget* getParam(int paramId) { return NULL; }
    PortWidget* getInput(int portId) { return NULL; }
    PortWidget* getOutput(int portId) { return NULL; }
    std::vector<ParamWidget*> getParams() { return {}; }
    std::vector<PortWidget*> getPorts() { return {}; }
    std::vector<PortWidget*> getInputs() { return {}; }
    std::vector<PortWidget*> getOutputs() { return {}; }

    virtual void appendContextMenu(ui::Menu*) {}

    json_t* toJson() { return NULL; }
    void fromJson(json_t*) {}
    bool pasteJsonAction(json_t*) { return false; }
    void copyClipboard() {}
    bool pasteClipboardAction() { return false; }
    void load(std::string) {}
    void loadAction(std::string) {}
    void loadTemplate() {}
    void loadDialog() {}
    void save(std::string) {}
    void saveTemplate() {}
    void saveTemplateDialog() {}
    bool hasTemplate() { return false; }
    void clearTemplate() {}
    void clearTemplateDialog() {}
    void saveDialog() {}
    void disconnect() {}
    void resetAction() {}
    void randomizeAction() {}
    void appendDisconnectActions(/*history::ComplexAction*/void*) {}
    void disconnectAction() {}
    void cloneAction(bool cc = true) {}
    void bypassAction(bool) {}
    void removeAction() {}
    void createContextMenu() {}
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
//     widget::FramebufferWidget* fb;
    CircularShadow* shadow;
//     widget::TransformWidget* tw;
    widget::SvgWidget* sw;
    void setSvg(std::shared_ptr<window::Svg>) {}
};

struct SvgPanel : widget::Widget {
//     widget::FramebufferWidget* fb;
//     widget::SvgWidget* sw;
//     PanelBorder* panelBorder;
    void setBackground(std::shared_ptr<window::Svg>) {}
};

struct SvgPort : PortWidget {
//     widget::FramebufferWidget* fb;
    CircularShadow* shadow;
//     widget::SvgWidget* sw;
    void setSvg(std::shared_ptr<window::Svg>) {}
};

struct SvgScrew : widget::Widget {
//     widget::FramebufferWidget* fb;
    widget::SvgWidget* sw;
    void setSvg(std::shared_ptr<window::Svg>) {}
};

struct SvgSlider : app::SliderKnob {
//     widget::FramebufferWidget* fb;
    widget::SvgWidget* background;
    widget::SvgWidget* handle;
    math::Vec minHandlePos, maxHandlePos;
    void setBackgroundSvg(std::shared_ptr<window::Svg>) {}
    void setHandleSvg(std::shared_ptr<window::Svg>) {}
    void setHandlePos(math::Vec minHandlePos, math::Vec maxHandlePos) {}
    void setHandlePosCentered(math::Vec minHandlePosCentered, math::Vec maxHandlePosCentered) {}
};

struct SvgSwitch : Switch {
// 	widget::FramebufferWidget* fb;
// 	CircularShadow* shadow;
// 	widget::SvgWidget* sw;
// 	std::vector<std::shared_ptr<window::Svg>> frames;
    bool latch = false;
    void addFrame(std::shared_ptr<window::Svg>) {}
};

struct Scene : widget::OpaqueWidget {
//     RackScrollWidget* rackScroll;
//     RackWidget* rack;
    widget::Widget* menuBar;
    widget::Widget* browser;
    math::Vec getMousePos() { return {}; }
};

} // namespace app

namespace asset {

inline std::string plugin(void* instance, const char* path) { return {}; }
inline std::string system(const char* path) { return {}; }

} // namespace asset

namespace componentlibrary {

static constexpr const NVGcolor SCHEME_BLACK_TRANSPARENT = {};
static constexpr const NVGcolor SCHEME_BLACK = {};
static constexpr const NVGcolor SCHEME_WHITE = {};
static constexpr const NVGcolor SCHEME_RED = {};
static constexpr const NVGcolor SCHEME_ORANGE = {};
static constexpr const NVGcolor SCHEME_YELLOW = {};
static constexpr const NVGcolor SCHEME_GREEN = {};
static constexpr const NVGcolor SCHEME_CYAN = {};
static constexpr const NVGcolor SCHEME_BLUE = {};
static constexpr const NVGcolor SCHEME_PURPLE = {};
static constexpr const NVGcolor SCHEME_LIGHT_GRAY = {};
static constexpr const NVGcolor SCHEME_DARK_GRAY = {};

template <typename TBase = app::ModuleLightWidget>
struct TSvgLight : TBase {
//     widget::FramebufferWidget* fb;
//     widget::SvgWidget* sw;
    void setSvg(std::shared_ptr<window::Svg>) {}
};
using SvgLight = TSvgLight<>;

template <typename TBase = app::ModuleLightWidget>
struct TGrayModuleLightWidget : TBase {};
using GrayModuleLightWidget = TGrayModuleLightWidget<>;

template <typename TBase = GrayModuleLightWidget>
struct TWhiteLight : TBase {};
using WhiteLight = TWhiteLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TRedLight : TBase {};
using RedLight = TRedLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TGreenLight : TBase {};
using GreenLight = TGreenLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TBlueLight : TBase {};
using BlueLight = TBlueLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TYellowLight : TBase {};
using YellowLight = TYellowLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TGreenRedLight : TBase {};
using GreenRedLight = TGreenRedLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TRedGreenBlueLight : TBase {};
using RedGreenBlueLight = TRedGreenBlueLight<>;

template <typename TBase>
struct LargeLight : TSvgLight<TBase> {};

template <typename TBase>
struct MediumLight : TSvgLight<TBase> {};

template <typename TBase>
struct SmallLight : TSvgLight<TBase> {};

template <typename TBase>
struct TinyLight : TSvgLight<TBase> {};

template <typename TBase = GrayModuleLightWidget>
struct LargeSimpleLight : TBase {};

template <typename TBase = GrayModuleLightWidget>
struct MediumSimpleLight : TBase {};

template <typename TBase = GrayModuleLightWidget>
struct SmallSimpleLight : TBase {};

template <typename TBase = GrayModuleLightWidget>
struct TinySimpleLight : TBase {};

template <typename TBase>
struct RectangleLight : TBase {};

template <typename TBase>
struct VCVBezelLight : TBase {};
template <typename TBase>
using LEDBezelLight = VCVBezelLight<TBase>;

template <typename TBase>
struct PB61303Light : TBase {};

struct RoundKnob : app::SvgKnob {
    widget::SvgWidget* bg;
};
struct RoundBlackKnob : RoundKnob {};
struct RoundSmallBlackKnob : RoundKnob {};
struct RoundLargeBlackKnob : RoundKnob {};
struct RoundBigBlackKnob : RoundKnob {};
struct RoundHugeBlackKnob : RoundKnob {};
struct RoundBlackSnapKnob : RoundBlackKnob {};
struct Davies1900hKnob : app::SvgKnob {};
struct Davies1900hWhiteKnob : Davies1900hKnob {};
struct Davies1900hBlackKnob : Davies1900hKnob {};
struct Davies1900hRedKnob : Davies1900hKnob {};
struct Davies1900hLargeWhiteKnob : Davies1900hKnob {};
struct Davies1900hLargeBlackKnob : Davies1900hKnob {};
struct Davies1900hLargeRedKnob : Davies1900hKnob {};
struct Rogan : app::SvgKnob {
    widget::SvgWidget* bg;
    widget::SvgWidget* fg;
};
struct Rogan6PSWhite : Rogan {};
struct Rogan5PSGray : Rogan {};
struct Rogan3PSBlue : Rogan {};
struct Rogan3PSRed : Rogan {};
struct Rogan3PSGreen : Rogan {};
struct Rogan3PSWhite : Rogan {};
struct Rogan3PBlue : Rogan {};
struct Rogan3PRed : Rogan {};
struct Rogan3PGreen : Rogan {};
struct Rogan3PWhite : Rogan {};
struct Rogan2SGray : Rogan {};
struct Rogan2PSBlue : Rogan {};
struct Rogan2PSRed : Rogan {};
struct Rogan2PSGreen : Rogan {};
struct Rogan2PSWhite : Rogan {};
struct Rogan2PBlue : Rogan {};
struct Rogan2PRed : Rogan {};
struct Rogan2PGreen : Rogan {};
struct Rogan2PWhite : Rogan {};
struct Rogan1PSBlue : Rogan {};
struct Rogan1PSRed : Rogan {};
struct Rogan1PSGreen : Rogan {};
struct Rogan1PSWhite : Rogan {};
struct Rogan1PBlue : Rogan {};
struct Rogan1PRed : Rogan {};
struct Rogan1PGreen : Rogan {};
struct Rogan1PWhite : Rogan {};
struct SynthTechAlco : app::SvgKnob {
    widget::SvgWidget* bg;
};
struct Trimpot : app::SvgKnob {
    widget::SvgWidget* bg;
};
struct BefacoBigKnob : app::SvgKnob {
    widget::SvgWidget* bg;
};
struct BefacoTinyKnob : app::SvgKnob {
    widget::SvgWidget* bg;
};
struct BefacoSlidePot : app::SvgSlider {};

struct VCVSlider : app::SvgSlider {};
using LEDSlider = VCVSlider;

struct VCVSliderHorizontal : app::SvgSlider {};
using LEDSliderHorizontal = VCVSliderHorizontal;

template <typename TBase, typename TLightBase = RedLight>
struct LightSlider : TBase {
    app::ModuleLightWidget* light = NULL;
    app::ModuleLightWidget* getLight() { return light; }
};

template <typename TBase>
struct VCVSliderLight : RectangleLight<TSvgLight<TBase>> {};
template <typename TBase>
using LEDSliderLight = VCVSliderLight<TBase>;

template <typename TLightBase = RedLight>
struct VCVLightSlider : LightSlider<VCVSlider, VCVSliderLight<TLightBase>> {};
template <typename TLightBase = RedLight>
using LEDLightSlider = VCVLightSlider<TLightBase>;

struct LEDSliderGreen : VCVLightSlider<GreenLight> {};
struct LEDSliderRed : VCVLightSlider<RedLight> {};
struct LEDSliderYellow : VCVLightSlider<YellowLight> {};
struct LEDSliderBlue : VCVLightSlider<BlueLight> {};
struct LEDSliderWhite : VCVLightSlider<WhiteLight> {};

template <typename TLightBase = RedLight>
struct VCVLightSliderHorizontal : LightSlider<VCVSliderHorizontal, TLightBase> {};
template <typename TLightBase = RedLight>
using LEDLightSliderHorizontal = VCVLightSliderHorizontal<TLightBase>;

struct PJ301MPort : app::SvgPort {};
struct PJ3410Port : app::SvgPort {};
struct CL1362Port : app::SvgPort {};

template <typename TSwitch>
struct MomentarySwitch : TSwitch {};

struct NKK : app::SvgSwitch {};
struct CKSS : app::SvgSwitch {};
struct CKSSThree : app::SvgSwitch {};
struct CKSSThreeHorizontal : app::SvgSwitch {};
struct CKD6 : app::SvgSwitch {};
struct TL1105 : app::SvgSwitch {};
struct VCVButton : app::SvgSwitch {};
using LEDButton = VCVButton;
struct VCVLatch : VCVButton {};

template <typename TLight>
struct VCVLightButton : VCVButton {
    app::ModuleLightWidget* light = NULL;
    app::ModuleLightWidget* getLight() { return light; }
};
template <typename TLight>
using LEDLightButton = VCVLightButton<TLight>;

template <typename TLight>
struct VCVLightLatch : VCVLightButton<TLight> {};

struct BefacoSwitch : app::SvgSwitch {};

struct BefacoPush : app::SvgSwitch {};

struct VCVBezel : app::SvgSwitch {};
using LEDBezel = VCVBezel;

struct VCVBezelLatch : VCVBezel {};

template <typename TLightBase = WhiteLight>
struct VCVLightBezel : VCVBezel {
    app::ModuleLightWidget* light = NULL;
    app::ModuleLightWidget* getLight() { return light; }
};
template <typename TLightBase = WhiteLight>
using LEDLightBezel = VCVLightBezel<TLightBase>;

template <typename TLightBase = WhiteLight>
struct VCVLightBezelLatch : VCVLightBezel<TLightBase> {};

struct PB61303 : app::SvgSwitch {};

struct ScrewSilver : app::SvgScrew {};
struct ScrewBlack : app::SvgScrew {};

} // namespace componentlibrary

namespace dsp {

static constexpr const float FREQ_C4 = 261.6256f;
static constexpr const float FREQ_A4 = 440.0000f;
static constexpr const float FREQ_SEMITONE = 1.0594630943592953f;

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

// template <typename T>
// T amplitudeToDb(T amp) {
//     return simd::log10(amp) * 20;
// }

template <typename T>
T dbToAmplitude(T db) { return std::pow(10, db / 20); }

// template <typename T>
// T quadraticBipolar(T x) {
//     return simd::sgn(x) * (x * x);
// }

template <typename T>
T cubic(T x) { return x * x * x; }

// template <typename T>
// T quarticBipolar(T x) {
//     return simd::sgn(x) * (x * x * x * x);
// }

template <typename T>
T quintic(T x) { return x * x * x * x * x; }

// template <typename T>
// T sqrtBipolar(T x) {
//     return simd::sgn(x) * simd::sqrt(x);
// }

// template <typename T>
// T exponentialBipolar(T b, T x) {
//     return (simd::pow(b, x) - simd::pow(b, -x)) / (b - 1.f / b);
// }

template <size_t CHANNELS, typename T = float>
struct Frame { T samples[CHANNELS]; };

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

struct BooleanTrigger {
    bool state = true;
    void reset() {
        state = true;
    }
    bool process(bool state) {
        bool triggered = (state && !this->state);
        this->state = state;
        return triggered;
    }
};

template <typename T = float>
struct TSchmittTrigger {
    T state;
    TSchmittTrigger() {
        reset();
    }
    void reset() {
        state = T::mask();
    }
    T process(T in, T offThreshold = 0.f, T onThreshold = 1.f) {
        T on = (in >= onThreshold);
        T off = (in <= offThreshold);
        T triggered = ~state & on;
        state = on | (state & ~off);
        return triggered;
    }
    T isHigh() {
        return state;
    }
};


template <>
struct TSchmittTrigger<float> {
    bool state = true;
    void reset() {
        state = true;
    }
    bool process(float in, float offThreshold = 0.f, float onThreshold = 1.f) {
        if (state) {
            if (in <= offThreshold) {
                state = false;
            }
        }
        else {
            if (in >= onThreshold) {
                state = true;
                return true;
            }
        }
        return false;
    }
    bool isHigh() {
        return state;
    }
};

typedef TSchmittTrigger<> SchmittTrigger;

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

template <typename T = float>
struct TTimer {
    T time = 0.f;
    void reset() {
        time = 0.f;
    }
    T process(T deltaTime) {
        time += deltaTime;
        return time;
    }
    T getTime() {
        return time;
    }
};

typedef TTimer<> Timer;

struct ClockDivider {
    uint32_t clock = 0;
    uint32_t division = 1;
    void reset() { clock = 0; }
    void setDivision(uint32_t division) { this->division = division; }
    uint32_t getDivision() { return division; }
    uint32_t getClock() { return clock; }
    bool process() {
        clock++;
        if (clock >= division) {
            clock = 0;
            return true;
        }
        return false;
    }
};

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

template <typename T, size_t S>
struct RingBuffer {
    std::atomic<size_t> start{0};
    std::atomic<size_t> end{0};
    T data[S];
    void push(T t) {
        size_t i = end % S;
        data[i] = t;
        end++;
    }
    void pushBuffer(const T* t, int n) {
        size_t i = end % S;
        size_t e1 = i + n;
        size_t e2 = (e1 < S) ? e1 : S;
        std::memcpy(&data[i], t, sizeof(T) * (e2 - i));
        if (e1 > S) {
            std::memcpy(data, &t[S - i], sizeof(T) * (e1 - S));
        }
        end += n;
    }
    T shift() {
        size_t i = start % S;
        T t = data[i];
        start++;
        return t;
    }
    void shiftBuffer(T* t, size_t n) {
        size_t i = start % S;
        size_t s1 = i + n;
        size_t s2 = (s1 < S) ? s1 : S;
        std::memcpy(t, &data[i], sizeof(T) * (s2 - i));
        if (s1 > S) {
            std::memcpy(&t[S - i], data, sizeof(T) * (s1 - S));
        }
        start += n;
    }
    void clear() {
        start = end.load();
    }
    bool empty() const {
        return start >= end;
    }
    bool full() const {
        return end - start >= S;
    }
    size_t size() const {
        return end - start;
    }
    size_t capacity() const {
        return S - size();
    }
};

template <typename T, size_t S>
struct DoubleRingBuffer {
    std::atomic<size_t> start{0};
    std::atomic<size_t> end{0};
    T data[2 * S];
    void push(T t) {
        size_t i = end % S;
        data[i] = t;
        data[i + S] = t;
        end++;
    }
    T shift() {
        size_t i = start % S;
        T t = data[i];
        start++;
        return t;
    }
    void clear() {
        start = end.load();
    }
    bool empty() const {
        return start >= end;
    }
    bool full() const {
        return end - start >= S;
    }
    size_t size() const {
        return end - start;
    }
    size_t capacity() const {
        return S - size();
    }
    T* endData() {
        size_t i = end % S;
        return &data[i];
    }
    void endIncr(size_t n) {
        size_t i = end % S;
        size_t e1 = i + n;
        size_t e2 = (e1 < S) ? e1 : S;
        std::memcpy(&data[S + i], &data[i], sizeof(T) * (e2 - i));
        if (e1 > S) {
            std::memcpy(data, &data[S], sizeof(T) * (e1 - S));
        }
        end += n;
    }
    const T* startData() const {
        size_t i = start % S;
        return &data[i];
    }
    void startIncr(size_t n) {
        start += n;
    }
};

template <typename T, size_t S, size_t N>
struct AppleRingBuffer {
    size_t start = 0;
    size_t end = 0;
    T data[N];
    void returnBuffer() {
        size_t s = size();
        std::memmove(data, &data[start], sizeof(T) * s);
        start = 0;
        end = s;
    }
    void push(T t) {
        if (end + 1 > N) {
            returnBuffer();
        }
        data[end++] = t;
    }
    T shift() {
        return data[start++];
    }
    bool empty() const {
        return start == end;
    }
    bool full() const {
        return end - start == S;
    }
    size_t size() const {
        return end - start;
    }
    size_t capacity() const {
        return S - size();
    }
    T* endData(size_t n) {
        if (end + n > N) {
            returnBuffer();
        }
        return &data[end];
    }
    void endIncr(size_t n) {
        end += n;
    }
    const T* startData() const {
        return &data[start];
    }
    void startIncr(size_t n) {
        start += n;
    }
};

} // namespace dsp

namespace event {
using Base = widget::BaseEvent;
using PositionBase = widget::Widget::PositionBaseEvent;
using KeyBase = widget::Widget::KeyBaseEvent;
using TextBase = widget::Widget::TextBaseEvent;
using Hover = widget::Widget::HoverEvent;
using Button = widget::Widget::ButtonEvent;
using DoubleClick = widget::Widget::DoubleClickEvent;
using HoverKey = widget::Widget::HoverKeyEvent;
using HoverText = widget::Widget::HoverTextEvent;
using HoverScroll = widget::Widget::HoverScrollEvent;
using Enter = widget::Widget::EnterEvent;
using Leave = widget::Widget::LeaveEvent;
using Select = widget::Widget::SelectEvent;
using Deselect = widget::Widget::DeselectEvent;
using SelectKey = widget::Widget::SelectKeyEvent;
using SelectText = widget::Widget::SelectTextEvent;
using DragBase = widget::Widget::DragBaseEvent;
using DragStart = widget::Widget::DragStartEvent;
using DragEnd = widget::Widget::DragEndEvent;
using DragMove = widget::Widget::DragMoveEvent;
using DragHover = widget::Widget::DragHoverEvent;
using DragEnter = widget::Widget::DragEnterEvent;
using DragLeave = widget::Widget::DragLeaveEvent;
using DragDrop = widget::Widget::DragDropEvent;
using PathDrop = widget::Widget::PathDropEvent;
using Action = widget::Widget::ActionEvent;
using Change = widget::Widget::ChangeEvent;
using Dirty = widget::Widget::DirtyEvent;
using Reposition = widget::Widget::RepositionEvent;
using Resize = widget::Widget::ResizeEvent;
using Add = widget::Widget::AddEvent;
using Remove = widget::Widget::RemoveEvent;
using Show = widget::Widget::ShowEvent;
using Hide = widget::Widget::HideEvent;
} // namespace event

namespace plugin {

struct Model {
    virtual ~Model() {}
    virtual engine::Module* createModule() = 0;
};

struct Plugin {
};

} // namespace plugin

namespace random {

struct Xoroshiro128Plus {
    uint64_t state[2] = {};
    void seed(uint64_t s0, uint64_t s1) {
        state[0] = s0;
        state[1] = s1;
        operator()();
    }
    bool isSeeded() { return state[0] || state[1]; }
    static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
    uint64_t operator()() {
        uint64_t s0 = state[0];
        uint64_t s1 = state[1];
        uint64_t result = s0 + s1;
        s1 ^= s0;
        state[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
        state[1] = rotl(s1, 36);
        return result;
    }
    constexpr uint64_t min() const { return 0; }
    constexpr uint64_t max() const { return UINT64_MAX; }
};

Xoroshiro128Plus& local();

template <typename T>
T get() { return local()(); }

template <>
inline uint32_t get() { return get<uint64_t>() >> 32; }

template <>
inline uint16_t get() { return get<uint64_t>() >> 48; }

template <>
inline uint8_t get() { return get<uint64_t>() >> 56; }

template <>
inline bool get() { return get<uint64_t>() >> 63; }

template <>
inline float get() { return get<uint32_t>() * 2.32830629e-10f; }

template <>
inline double get() { return get<uint64_t>() * 5.421010862427522e-20; }

inline uint64_t u64() { return get<uint64_t>(); }
inline uint32_t u32() { return get<uint32_t>(); }
inline float uniform() { return get<float>(); }

inline float normal() {
    const float radius = std::sqrt(-2.f * std::log(1.f - get<float>()));
    const float theta = 2.f * M_PI * get<float>();
    return radius * std::sin(theta);
}

inline void buffer(uint8_t* out, size_t len) {
    Xoroshiro128Plus& rng = local();
    for (size_t i = 0; i < len; i += 4) {
        uint64_t r = rng();
        out[i] = r;
        if (i + 1 < len)
            out[i + 1] = r >> 8;
        if (i + 2 < len)
            out[i + 2] = r >> 16;
        if (i + 3 < len)
            out[i + 3] = r >> 24;
    }
}

inline std::vector<uint8_t> vector(size_t len) {
    std::vector<uint8_t> v(len);
    buffer(v.data(), len);
    return v;
}

} // namespace random

namespace settings {

const bool tooltips = false;

} // namespace settings

namespace string {

inline std::string fV(const char* format, va_list args) {
    // va_lists cannot be reused but we need it twice, so clone args.
    va_list args2;
    va_copy(args2, args);
    // Compute size of required buffer
    int size = vsnprintf(NULL, 0, format, args);
    if (size < 0)
        return "";
    // Create buffer
    std::string s;
    s.resize(size);
    vsnprintf(&s[0], size + 1, format, args2);
    return s;
}

__attribute__((format(printf, 1, 2)))
inline std::string f(const char* format, ...) {
    va_list args;
    va_start(args, format);
    std::string s = fV(format, args);
    va_end(args);
    return s;
}

} // namespace string

namespace ui {

struct Button : widget::OpaqueWidget {
    std::string text;
    Quantity* quantity = NULL;
};

struct ChoiceButton : Button {
};

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
    virtual Menu* createChildMenu() { return NULL; }
};

struct MenuLabel : MenuEntry {
    std::string text;
};

struct MenuSeparator : MenuEntry {
};

struct Tooltip : widget::Widget {
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

struct Svg {
// 	NSVGimage* handle = NULL;
// 	~Svg();
// 	/** Don't call this directly. Use `Svg::load()` for caching. */
// 	void loadFile(const std::string& filename);
// 	void loadString(const std::string& str);
// 	math::Vec getSize();
// 	int getNumShapes();
// 	int getNumPaths();
// 	int getNumPoints();
// 	void draw(NVGcontext* vg);
    static std::shared_ptr<Svg> load(const std::string&) { return {}; }
};

inline void svgDraw(NVGcontext*, NSVGimage*) {}

struct Font {
    int handle;
};

struct Image {
    int handle;
};

struct Window {
    inline std::shared_ptr<Font> loadFont(const std::string&) { return {}; }
    inline std::shared_ptr<Image> loadImage(const std::string&) { return {}; }
    inline std::shared_ptr<Svg> loadSvg(const std::string&) { return {}; }
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
plugin::Model* createModel(std::string) {
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
inline TWidget* createWidget(math::Vec pos) {
    return NULL;
}

template <class TWidget>
inline TWidget* createWidgetCentered(math::Vec) {
    return NULL;
}

inline app::SvgPanel* createPanel(std::string) {
    return NULL;
}

template <class TParamWidget>
inline TParamWidget* createParam(math::Vec, engine::Module*, int) {
    return NULL;
}

template <class TParamWidget>
inline TParamWidget* createParamCentered(math::Vec, engine::Module*, int) {
    return NULL;
}

template <class TPortWidget>
inline TPortWidget* createInput(math::Vec, engine::Module*, int) {
    return NULL;
}

template <class TPortWidget>
inline TPortWidget* createInputCentered(math::Vec, engine::Module*, int) {
    return NULL;
}

template <class TPortWidget>
inline TPortWidget* createOutput(math::Vec pos, engine::Module*, int) {
    return NULL;
}

template <class TPortWidget>
inline TPortWidget* createOutputCentered(math::Vec pos, engine::Module*, int) {
    return NULL;
}

template <class TModuleLightWidget>
inline TModuleLightWidget* createLight(math::Vec, engine::Module*, int) {
    return NULL;
}

template <class TModuleLightWidget>
inline TModuleLightWidget* createLightCentered(math::Vec, engine::Module*, int) {
    return NULL;
}

template <class TParamWidget>
inline TParamWidget* createLightParam(math::Vec, engine::Module*, int, int) {
    return NULL;
}

template <class TParamWidget>
inline TParamWidget* createLightParamCentered(math::Vec, engine::Module*, int, int) {
    return NULL;
}

template <class TMenu = ui::Menu>
inline TMenu* createMenu() {
    return NULL;
}

template <class TMenuLabel = ui::MenuLabel>
inline TMenuLabel* createMenuLabel(std::string) {
    return NULL;
}

template <class TMenuItem = ui::MenuItem>
inline TMenuItem* createMenuItem(std::string, std::string rt = "") {
    return NULL;
}

template <class TMenuItem = ui::MenuItem>
inline TMenuItem* createMenuItem(std::string, std::string rightText, std::function<void()>, bool d = false, bool ac = false) {
    return NULL;
}

template <class TMenuItem = ui::MenuItem>
inline ui::MenuItem* createCheckMenuItem(std::string, std::string, std::function<bool()>, std::function<void()>, bool d = false, bool ac = false) {
    return NULL;
}

template <class TMenuItem = ui::MenuItem>
inline ui::MenuItem* createBoolMenuItem(std::string, std::string, std::function<bool()>, std::function<void(bool)>, bool d = false, bool ac = false) {
    return NULL;
}

template <typename T>
inline ui::MenuItem* createBoolPtrMenuItem(std::string, std::string, T*) {
    return NULL;
}

template <class TMenuItem = ui::MenuItem>
inline ui::MenuItem* createSubmenuItem(std::string, std::string, std::function<void(ui::Menu*)>, bool d = false) {
    return NULL;
}

template <class TMenuItem = ui::MenuItem>
inline ui::MenuItem* createIndexSubmenuItem(std::string, std::vector<std::string>, std::function<size_t()>, std::function<void(size_t)>, bool d = false, bool ac = false) {
    return NULL;
}

template <typename T>
inline ui::MenuItem* createIndexPtrSubmenuItem(std::string, std::vector<std::string>, T*) {
    return NULL;
}

struct Context {
    app::Scene _scene;
    engine::Engine _engine;
    window::Window _window;
    engine::Engine* engine = &_engine;
    app::Scene* scene = &_scene;
    window::Window* window = &_window;
};

Context* contextGet();
void contextSet(Context* context);

} // namespace rack

#define APP rack::contextGet()
