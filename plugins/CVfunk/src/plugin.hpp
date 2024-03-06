#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelSteps;
extern Model* modelEnvelopeArray;
extern Model* modelPentaSequencer;
extern Model* modelImpulseController;
extern Model* modelSignals;
extern Model* modelRanges;