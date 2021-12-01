/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
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
 * This file is an edited version of VCVRack's Engine.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include <algorithm>
#include <set>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <tuple>
#include <pmmintrin.h>
#include <pthread.h>

#include <engine/Engine.hpp>
#include <settings.hpp>
#include <system.hpp>
#include <random.hpp>
#include <patch.hpp>
#include <plugin.hpp>
#include <helpers.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include "DistrhoUtils.hpp"

namespace rack {
namespace engine {


/** Allows multiple "reader" threads to obtain a lock simultaneously, but only one "writer" thread.
This implementation is currently just a wrapper for pthreads, which works on Linux/Mac/.
This is available in C++17 as std::shared_mutex, but unfortunately we're using C++11.
*/
struct ReadWriteMutex {
	pthread_rwlock_t rwlock;

	ReadWriteMutex() {
		if (pthread_rwlock_init(&rwlock, nullptr))
			throw Exception("pthread_rwlock_init failed");
	}
	~ReadWriteMutex() {
		pthread_rwlock_destroy(&rwlock);
	}
	void lockReader() {
		DISTRHO_SAFE_ASSERT_RETURN(pthread_rwlock_rdlock(&rwlock) == 0,);
	}
	void unlockReader() {
		DISTRHO_SAFE_ASSERT_RETURN(pthread_rwlock_unlock(&rwlock) == 0,);
	}
	void lockWriter() {
		DISTRHO_SAFE_ASSERT_RETURN(pthread_rwlock_wrlock(&rwlock) == 0,);
	}
	void unlockWriter() {
		DISTRHO_SAFE_ASSERT_RETURN(pthread_rwlock_unlock(&rwlock) == 0,);
	}
};

struct ReadLock {
	ReadWriteMutex& m;
	ReadLock(ReadWriteMutex& m) : m(m) {
		m.lockReader();
	}
	~ReadLock() {
		m.unlockReader();
	}
};

struct WriteLock {
	ReadWriteMutex& m;
	WriteLock(ReadWriteMutex& m) : m(m) {
		m.lockWriter();
	}
	~WriteLock() {
		m.unlockWriter();
	}
};


struct Engine::Internal {
	std::vector<Module*> modules;
	std::vector<Cable*> cables;
	std::set<ParamHandle*> paramHandles;

	// moduleId
	std::map<int64_t, Module*> modulesCache;
	// cableId
	std::map<int64_t, Cable*> cablesCache;
	// (moduleId, paramId)
	std::map<std::tuple<int64_t, int>, ParamHandle*> paramHandlesCache;

	float sampleRate = 0.f;
	float sampleTime = 0.f;
	int64_t block = 0;
	int64_t frame = 0;
	int64_t blockFrame = 0;
	double blockTime = 0.0;
	int blockFrames = 0;

	// Meter
	int meterCount = 0;
	double meterTotal = 0.0;
	double meterMax = 0.0;
	double meterLastTime = -INFINITY;
	double meterLastAverage = 0.0;
	double meterLastMax = 0.0;

	// Parameter smoothing
	Module* smoothModule = NULL;
	int smoothParamId = 0;
	float smoothValue = 0.f;

	/** Mutex that guards the Engine state, such as settings, Modules, and Cables.
	Writers lock when mutating the engine's state or stepping the block.
	Readers lock when using the engine's state.
	*/
	ReadWriteMutex mutex;
};


static void Engine_updateExpander_NoLock(Engine* that, Module* module, bool side) {
	Module::Expander& expander = side ? module->rightExpander : module->leftExpander;
	Module* oldExpanderModule = expander.module;

	if (expander.moduleId >= 0) {
		if (!expander.module || expander.module->id != expander.moduleId) {
			expander.module = that->getModule_NoLock(expander.moduleId);
		}
	}
	else {
		if (expander.module) {
			expander.module = NULL;
		}
	}

	if (expander.module != oldExpanderModule) {
		// Dispatch ExpanderChangeEvent
		Module::ExpanderChangeEvent e;
		e.side = side;
		module->onExpanderChange(e);
	}
}


static void Cable_step(Cable* that) {
	Output* output = &that->outputModule->outputs[that->outputId];
	Input* input = &that->inputModule->inputs[that->inputId];
	// Match number of polyphonic channels to output port
	int channels = output->channels;
	// Copy all voltages from output to input
	for (int c = 0; c < channels; c++) {
		float v = output->voltages[c];
		// Set 0V if infinite or NaN
		if (!std::isfinite(v))
			v = 0.f;
		input->voltages[c] = v;
	}
	// Set higher channel voltages to 0
	for (int c = channels; c < input->channels; c++) {
		input->voltages[c] = 0.f;
	}
	input->channels = channels;
}


/** Steps a single frame
*/
static void Engine_stepFrame(Engine* that) {
	Engine::Internal* internal = that->internal;

	// Param smoothing
	Module* smoothModule = internal->smoothModule;
	if (smoothModule) {
		int smoothParamId = internal->smoothParamId;
		float smoothValue = internal->smoothValue;
		Param* smoothParam = &smoothModule->params[smoothParamId];
		float value = smoothParam->value;
		// Use decay rate of roughly 1 graphics frame
		const float smoothLambda = 60.f;
		float newValue = value + (smoothValue - value) * smoothLambda * internal->sampleTime;
		if (value == newValue) {
			// Snap to actual smooth value if the value doesn't change enough (due to the granularity of floats)
			smoothParam->setValue(smoothValue);
			internal->smoothModule = NULL;
			internal->smoothParamId = 0;
		}
		else {
			smoothParam->setValue(newValue);
		}
	}

	// Step cables
	for (Cable* cable : internal->cables) {
		Cable_step(cable);
	}

	// Flip messages for each module
	for (Module* module : internal->modules) {
		if (module->leftExpander.messageFlipRequested) {
			std::swap(module->leftExpander.producerMessage, module->leftExpander.consumerMessage);
			module->leftExpander.messageFlipRequested = false;
		}
		if (module->rightExpander.messageFlipRequested) {
			std::swap(module->rightExpander.producerMessage, module->rightExpander.consumerMessage);
			module->rightExpander.messageFlipRequested = false;
		}
	}

	// Build ProcessArgs
	Module::ProcessArgs processArgs;
	processArgs.sampleRate = internal->sampleRate;
	processArgs.sampleTime = internal->sampleTime;
	processArgs.frame = internal->frame;

	// Step each module
	for (Module* module : internal->modules) {
		module->doProcess(processArgs);
	}

	++internal->frame;
}


static void Port_setDisconnected(Port* that) {
	that->channels = 0;
	for (int c = 0; c < PORT_MAX_CHANNELS; c++) {
		that->voltages[c] = 0.f;
	}
}


static void Port_setConnected(Port* that) {
	if (that->channels > 0)
		return;
	that->channels = 1;
}


static void Engine_updateConnected(Engine* that) {
	// Find disconnected ports
	std::set<Port*> disconnectedPorts;
	for (Module* module : that->internal->modules) {
		for (Input& input : module->inputs) {
			disconnectedPorts.insert(&input);
		}
		for (Output& output : module->outputs) {
			disconnectedPorts.insert(&output);
		}
	}
	for (Cable* cable : that->internal->cables) {
		// Connect input
		Input& input = cable->inputModule->inputs[cable->inputId];
		auto inputIt = disconnectedPorts.find(&input);
		if (inputIt != disconnectedPorts.end())
			disconnectedPorts.erase(inputIt);
		Port_setConnected(&input);
		// Connect output
		Output& output = cable->outputModule->outputs[cable->outputId];
		auto outputIt = disconnectedPorts.find(&output);
		if (outputIt != disconnectedPorts.end())
			disconnectedPorts.erase(outputIt);
		Port_setConnected(&output);
	}
	// Disconnect ports that have no cable
	for (Port* port : disconnectedPorts) {
		Port_setDisconnected(port);
	}
}


static void Engine_refreshParamHandleCache(Engine* that) {
	// Clear cache
	that->internal->paramHandlesCache.clear();
	// Add active ParamHandles to cache
	for (ParamHandle* paramHandle : that->internal->paramHandles) {
		if (paramHandle->moduleId >= 0) {
			that->internal->paramHandlesCache[std::make_tuple(paramHandle->moduleId, paramHandle->paramId)] = paramHandle;
		}
	}
}


Engine::Engine() {
	internal = new Internal;
}


Engine::~Engine() {
	// Clear modules, cables, etc
	clear();

	// Make sure there are no cables or modules in the rack on destruction.
	// If this happens, a module must have failed to remove itself before the RackWidget was destroyed.
	DISTRHO_SAFE_ASSERT(internal->cables.empty());
	DISTRHO_SAFE_ASSERT(internal->modules.empty());
	DISTRHO_SAFE_ASSERT(internal->paramHandles.empty());

	DISTRHO_SAFE_ASSERT(internal->modulesCache.empty());
	DISTRHO_SAFE_ASSERT(internal->cablesCache.empty());
	DISTRHO_SAFE_ASSERT(internal->paramHandlesCache.empty());

	delete internal;
}


void Engine::clear() {
	WriteLock lock(internal->mutex);
	clear_NoLock();
}


void Engine::clear_NoLock() {
	// Copy lists because we'll be removing while iterating
	std::set<ParamHandle*> paramHandles = internal->paramHandles;
	for (ParamHandle* paramHandle : paramHandles) {
		removeParamHandle_NoLock(paramHandle);
		// Don't delete paramHandle because they're normally owned by Module subclasses
	}
	std::vector<Cable*> cables = internal->cables;
	for (Cable* cable : cables) {
		removeCable_NoLock(cable);
		delete cable;
	}
	std::vector<Module*> modules = internal->modules;
	for (Module* module : modules) {
		removeModule_NoLock(module);
		delete module;
	}
}


void Engine::stepBlock(int frames) {
	// Start timer before locking
	double startTime = system::getTime();

	ReadLock lock(internal->mutex);
	// Configure thread
	random::init();

	internal->blockFrame = internal->frame;
	internal->blockTime = system::getTime();
	internal->blockFrames = frames;

	// Update expander pointers
	for (Module* module : internal->modules) {
		Engine_updateExpander_NoLock(this, module, false);
		Engine_updateExpander_NoLock(this, module, true);
	}

	// Step individual frames
	for (int i = 0; i < frames; i++) {
		Engine_stepFrame(this);
	}

	internal->block++;

	// Stop timer
	double endTime = system::getTime();
	double meter = (endTime - startTime) / (frames * internal->sampleTime);
	internal->meterTotal += meter;
	internal->meterMax = std::fmax(internal->meterMax, meter);
	internal->meterCount++;

	// Update meter values
	const double meterUpdateDuration = 1.0;
	if (startTime - internal->meterLastTime >= meterUpdateDuration) {
		internal->meterLastAverage = internal->meterTotal / internal->meterCount;
		internal->meterLastMax = internal->meterMax;
		internal->meterLastTime = startTime;
		internal->meterCount = 0;
		internal->meterTotal = 0.0;
		internal->meterMax = 0.0;
	}
}


void Engine::setMasterModule(Module* module) {
}


void Engine::setMasterModule_NoLock(Module* module) {
}


Module* Engine::getMasterModule() {
	return nullptr;
}


float Engine::getSampleRate() {
	return internal->sampleRate;
}


void Engine::setSampleRate(float sampleRate) {
	if (sampleRate == internal->sampleRate)
		return;
	WriteLock lock(internal->mutex);

	internal->sampleRate = sampleRate;
	internal->sampleTime = 1.f / sampleRate;
	// Dispatch SampleRateChangeEvent
	Module::SampleRateChangeEvent e;
	e.sampleRate = internal->sampleRate;
	e.sampleTime = internal->sampleTime;
	for (Module* module : internal->modules) {
		module->onSampleRateChange(e);
	}
}


void Engine::setSuggestedSampleRate(float suggestedSampleRate) {
}


float Engine::getSampleTime() {
	return internal->sampleTime;
}


void Engine::yieldWorkers() {
}


int64_t Engine::getBlock() {
	return internal->block;
}


int64_t Engine::getFrame() {
	return internal->frame;
}


void Engine::setFrame(int64_t frame) {
	internal->frame = frame;
}


int64_t Engine::getBlockFrame() {
	return internal->blockFrame;
}


double Engine::getBlockTime() {
	return internal->blockTime;
}


int Engine::getBlockFrames() {
	return internal->blockFrames;
}


double Engine::getBlockDuration() {
	return internal->blockFrames * internal->sampleTime;
}


double Engine::getMeterAverage() {
	return internal->meterLastAverage;
}


double Engine::getMeterMax() {
	return internal->meterLastMax;
}


size_t Engine::getNumModules() {
	return internal->modules.size();
}


size_t Engine::getModuleIds(int64_t* moduleIds, size_t len) {
	ReadLock lock(internal->mutex);
	size_t i = 0;
	for (Module* m : internal->modules) {
		if (i >= len)
			break;
		moduleIds[i] = m->id;
		i++;
	}
	return i;
}


std::vector<int64_t> Engine::getModuleIds() {
	ReadLock lock(internal->mutex);
	std::vector<int64_t> moduleIds;
	moduleIds.reserve(internal->modules.size());
	for (Module* m : internal->modules) {
		moduleIds.push_back(m->id);
	}
	return moduleIds;
}


void Engine::addModule(Module* module) {
	WriteLock lock(internal->mutex);
	DISTRHO_SAFE_ASSERT_RETURN(module,);
	// Check that the module is not already added
	auto it = std::find(internal->modules.begin(), internal->modules.end(), module);
	DISTRHO_SAFE_ASSERT_RETURN(it == internal->modules.end(),);
	// Set ID if unset or collides with an existing ID
	while (module->id < 0 || internal->modulesCache.find(module->id) != internal->modulesCache.end()) {
		// Randomly generate ID
		module->id = random::u64() % (1ull << 53);
	}
	// Add module
	internal->modules.push_back(module);
	internal->modulesCache[module->id] = module;
	// Dispatch AddEvent
	Module::AddEvent eAdd;
	module->onAdd(eAdd);
	// Dispatch SampleRateChangeEvent
	Module::SampleRateChangeEvent eSrc;
	eSrc.sampleRate = internal->sampleRate;
	eSrc.sampleTime = internal->sampleTime;
	module->onSampleRateChange(eSrc);
	// Update ParamHandles' module pointers
	for (ParamHandle* paramHandle : internal->paramHandles) {
		if (paramHandle->moduleId == module->id)
			paramHandle->module = module;
	}
}


void Engine::removeModule(Module* module) {
	WriteLock lock(internal->mutex);
	removeModule_NoLock(module);
}


void Engine::removeModule_NoLock(Module* module) {
	DISTRHO_SAFE_ASSERT_RETURN(module,);
	// Check that the module actually exists
	auto it = std::find(internal->modules.begin(), internal->modules.end(), module);
	DISTRHO_SAFE_ASSERT_RETURN(it != internal->modules.end(),);
	// Remove from widgets cache
	CardinalPluginModelHelper* const helper = dynamic_cast<CardinalPluginModelHelper*>(module->model);
	DISTRHO_SAFE_ASSERT_RETURN(helper != nullptr,);
	helper->removeCachedModuleWidget(module);
	// Dispatch RemoveEvent
	Module::RemoveEvent eRemove;
	module->onRemove(eRemove);
	// Update ParamHandles' module pointers
	for (ParamHandle* paramHandle : internal->paramHandles) {
		if (paramHandle->moduleId == module->id)
			paramHandle->module = NULL;
	}
	// If a param is being smoothed on this module, stop smoothing it immediately
	if (module == internal->smoothModule) {
		internal->smoothModule = NULL;
	}
	// Check that all cables are disconnected
	for (Cable* cable : internal->cables) {
		DISTRHO_SAFE_ASSERT(cable->inputModule != module);
		DISTRHO_SAFE_ASSERT(cable->outputModule != module);
	}
	// Update expanders of other modules
	for (Module* m : internal->modules) {
		if (m->leftExpander.module == module) {
			m->leftExpander.moduleId = -1;
			m->leftExpander.module = NULL;
		}
		if (m->rightExpander.module == module) {
			m->rightExpander.moduleId = -1;
			m->rightExpander.module = NULL;
		}
	}
	// Remove module
	internal->modulesCache.erase(module->id);
	internal->modules.erase(it);
	// Reset expanders
	module->leftExpander.moduleId = -1;
	module->leftExpander.module = NULL;
	module->rightExpander.moduleId = -1;
	module->rightExpander.module = NULL;
}


bool Engine::hasModule(Module* module) {
	ReadLock lock(internal->mutex);
	// TODO Performance could be improved by searching modulesCache, but more testing would be needed to make sure it's always valid.
	auto it = std::find(internal->modules.begin(), internal->modules.end(), module);
	return it != internal->modules.end();
}


Module* Engine::getModule(int64_t moduleId) {
	ReadLock lock(internal->mutex);
	return getModule_NoLock(moduleId);
}


Module* Engine::getModule_NoLock(int64_t moduleId) {
	auto it = internal->modulesCache.find(moduleId);
	if (it == internal->modulesCache.end())
		return NULL;
	return it->second;
}


void Engine::resetModule(Module* module) {
	WriteLock lock(internal->mutex);
	DISTRHO_SAFE_ASSERT_RETURN(module,);

	Module::ResetEvent eReset;
	module->onReset(eReset);
}


void Engine::randomizeModule(Module* module) {
	WriteLock lock(internal->mutex);
	DISTRHO_SAFE_ASSERT_RETURN(module,);

	Module::RandomizeEvent eRandomize;
	module->onRandomize(eRandomize);
}


void Engine::bypassModule(Module* module, bool bypassed) {
	DISTRHO_SAFE_ASSERT_RETURN(module,);
	if (module->isBypassed() == bypassed)
		return;

	WriteLock lock(internal->mutex);

	// Clear outputs and set to 1 channel
	for (Output& output : module->outputs) {
		// This zeros all voltages, but the channel is set to 1 if connected
		output.setChannels(0);
	}
	// Set bypassed state
	module->setBypassed(bypassed);
	if (bypassed) {
		// Dispatch BypassEvent
		Module::BypassEvent eBypass;
		module->onBypass(eBypass);
	}
	else {
		// Dispatch UnBypassEvent
		Module::UnBypassEvent eUnBypass;
		module->onUnBypass(eUnBypass);
	}
}


json_t* Engine::moduleToJson(Module* module) {
	ReadLock lock(internal->mutex);
	return module->toJson();
}


void Engine::moduleFromJson(Module* module, json_t* rootJ) {
	WriteLock lock(internal->mutex);
	module->fromJson(rootJ);
}


void Engine::prepareSaveModule(Module* module) {
	ReadLock lock(internal->mutex);
	Module::SaveEvent e;
	module->onSave(e);
}


void Engine::prepareSave() {
	ReadLock lock(internal->mutex);
	for (Module* module : internal->modules) {
		Module::SaveEvent e;
		module->onSave(e);
	}
}


size_t Engine::getNumCables() {
	return internal->cables.size();
}


size_t Engine::getCableIds(int64_t* cableIds, size_t len) {
	ReadLock lock(internal->mutex);
	size_t i = 0;
	for (Cable* c : internal->cables) {
		if (i >= len)
			break;
		cableIds[i] = c->id;
		i++;
	}
	return i;
}


std::vector<int64_t> Engine::getCableIds() {
	ReadLock lock(internal->mutex);
	std::vector<int64_t> cableIds;
	cableIds.reserve(internal->cables.size());
	for (Cable* c : internal->cables) {
		cableIds.push_back(c->id);
	}
	return cableIds;
}


void Engine::addCable(Cable* cable) {
	WriteLock lock(internal->mutex);
	DISTRHO_SAFE_ASSERT_RETURN(cable,);
	// Check cable properties
	DISTRHO_SAFE_ASSERT_RETURN(cable->inputModule,);
	DISTRHO_SAFE_ASSERT_RETURN(cable->outputModule,);
	bool outputWasConnected = false;
	for (Cable* cable2 : internal->cables) {
		// Check that the cable is not already added
		DISTRHO_SAFE_ASSERT_RETURN(cable2 != cable,);
		// Check that the input is not already used by another cable
		DISTRHO_SAFE_ASSERT_RETURN(!(cable2->inputModule == cable->inputModule && cable2->inputId == cable->inputId),);
		// Get connected status of output, to decide whether we need to call a PortChangeEvent.
		// It's best to not trust `cable->outputModule->outputs[cable->outputId]->isConnected()`
		if (cable2->outputModule == cable->outputModule && cable2->outputId == cable->outputId)
			outputWasConnected = true;
	}
	// Set ID if unset or collides with an existing ID
	while (cable->id < 0 || internal->cablesCache.find(cable->id) != internal->cablesCache.end()) {
		// Randomly generate ID
		cable->id = random::u64() % (1ull << 53);
	}
	// Add the cable
	internal->cables.push_back(cable);
	internal->cablesCache[cable->id] = cable;
	Engine_updateConnected(this);
	// Dispatch input port event
	{
		Module::PortChangeEvent e;
		e.connecting = true;
		e.type = Port::INPUT;
		e.portId = cable->inputId;
		cable->inputModule->onPortChange(e);
	}
	// Dispatch output port event if its state went from disconnected to connected.
	if (!outputWasConnected) {
		Module::PortChangeEvent e;
		e.connecting = true;
		e.type = Port::OUTPUT;
		e.portId = cable->outputId;
		cable->outputModule->onPortChange(e);
	}
}


void Engine::removeCable(Cable* cable) {
	WriteLock lock(internal->mutex);
	removeCable_NoLock(cable);
}


void Engine::removeCable_NoLock(Cable* cable) {
	DISTRHO_SAFE_ASSERT_RETURN(cable,);
	// Check that the cable is already added
	auto it = std::find(internal->cables.begin(), internal->cables.end(), cable);
	DISTRHO_SAFE_ASSERT_RETURN(it != internal->cables.end(),);
	// Remove the cable
	internal->cablesCache.erase(cable->id);
	internal->cables.erase(it);
	Engine_updateConnected(this);
	bool outputIsConnected = false;
	for (Cable* cable2 : internal->cables) {
		// Get connected status of output, to decide whether we need to call a PortChangeEvent.
		// It's best to not trust `cable->outputModule->outputs[cable->outputId]->isConnected()`
		if (cable2->outputModule == cable->outputModule && cable2->outputId == cable->outputId)
			outputIsConnected = true;
	}
	// Dispatch input port event
	{
		Module::PortChangeEvent e;
		e.connecting = false;
		e.type = Port::INPUT;
		e.portId = cable->inputId;
		cable->inputModule->onPortChange(e);
	}
	// Dispatch output port event if its state went from connected to disconnected.
	if (!outputIsConnected) {
		Module::PortChangeEvent e;
		e.connecting = false;
		e.type = Port::OUTPUT;
		e.portId = cable->outputId;
		cable->outputModule->onPortChange(e);
	}
}


bool Engine::hasCable(Cable* cable) {
	ReadLock lock(internal->mutex);
	// TODO Performance could be improved by searching cablesCache, but more testing would be needed to make sure it's always valid.
	auto it = std::find(internal->cables.begin(), internal->cables.end(), cable);
	return it != internal->cables.end();
}


Cable* Engine::getCable(int64_t cableId) {
	ReadLock lock(internal->mutex);
	auto it = internal->cablesCache.find(cableId);
	if (it == internal->cablesCache.end())
		return NULL;
	return it->second;
}


void Engine::setParamValue(Module* module, int paramId, float value) {
	// If param is being smoothed, cancel smoothing.
	if (internal->smoothModule == module && internal->smoothParamId == paramId) {
		internal->smoothModule = NULL;
		internal->smoothParamId = 0;
	}
	module->params[paramId].value = value;
}


float Engine::getParamValue(Module* module, int paramId) {
	return module->params[paramId].value;
}


void Engine::setParamSmoothValue(Module* module, int paramId, float value) {
	// If another param is being smoothed, jump value
	if (internal->smoothModule && !(internal->smoothModule == module && internal->smoothParamId == paramId)) {
		internal->smoothModule->params[internal->smoothParamId].value = internal->smoothValue;
	}
	internal->smoothParamId = paramId;
	internal->smoothValue = value;
	// Set this last so the above values are valid as soon as it is set
	internal->smoothModule = module;
}


float Engine::getParamSmoothValue(Module* module, int paramId) {
	if (internal->smoothModule == module && internal->smoothParamId == paramId)
		return internal->smoothValue;
	return module->params[paramId].value;
}


void Engine::addParamHandle(ParamHandle* paramHandle) {
	WriteLock lock(internal->mutex);
	// New ParamHandles must be blank.
	// This means we don't have to refresh the cache.
	DISTRHO_SAFE_ASSERT_RETURN(paramHandle->moduleId < 0,);

	// Check that the ParamHandle is not already added
	auto it = internal->paramHandles.find(paramHandle);
	DISTRHO_SAFE_ASSERT_RETURN(it == internal->paramHandles.end(),);

	// Add it
	internal->paramHandles.insert(paramHandle);
	// No need to refresh the cache because the moduleId is not set.
}


void Engine::removeParamHandle(ParamHandle* paramHandle) {
	WriteLock lock(internal->mutex);
	removeParamHandle_NoLock(paramHandle);
}


void Engine::removeParamHandle_NoLock(ParamHandle* paramHandle) {
	// Check that the ParamHandle is already added
	auto it = internal->paramHandles.find(paramHandle);
	DISTRHO_SAFE_ASSERT_RETURN(it != internal->paramHandles.end(),);

	// Remove it
	paramHandle->module = NULL;
	internal->paramHandles.erase(it);
	Engine_refreshParamHandleCache(this);
}


ParamHandle* Engine::getParamHandle(int64_t moduleId, int paramId) {
	ReadLock lock(internal->mutex);
	return getParamHandle_NoLock(moduleId, paramId);
}


ParamHandle* Engine::getParamHandle_NoLock(int64_t moduleId, int paramId) {
	auto it = internal->paramHandlesCache.find(std::make_tuple(moduleId, paramId));
	if (it == internal->paramHandlesCache.end())
		return NULL;
	return it->second;
}


ParamHandle* Engine::getParamHandle(Module* module, int paramId) {
	return getParamHandle(module->id, paramId);
}


void Engine::updateParamHandle(ParamHandle* paramHandle, int64_t moduleId, int paramId, bool overwrite) {
	WriteLock lock(internal->mutex);
	updateParamHandle_NoLock(paramHandle, moduleId, paramId, overwrite);
}


void Engine::updateParamHandle_NoLock(ParamHandle* paramHandle, int64_t moduleId, int paramId, bool overwrite) {
	// Check that it exists
	auto it = internal->paramHandles.find(paramHandle);
	DISTRHO_SAFE_ASSERT_RETURN(it != internal->paramHandles.end(),);

	// Set IDs
	paramHandle->moduleId = moduleId;
	paramHandle->paramId = paramId;
	paramHandle->module = NULL;
	// At this point, the ParamHandle cache might be invalid.

	if (paramHandle->moduleId >= 0) {
		// Replace old ParamHandle, or reset the current ParamHandle
		ParamHandle* oldParamHandle = getParamHandle_NoLock(moduleId, paramId);
		if (oldParamHandle) {
			if (overwrite) {
				oldParamHandle->moduleId = -1;
				oldParamHandle->paramId = 0;
				oldParamHandle->module = NULL;
			}
			else {
				paramHandle->moduleId = -1;
				paramHandle->paramId = 0;
				paramHandle->module = NULL;
			}
		}
	}

	// Set module pointer if the above block didn't reset it
	if (paramHandle->moduleId >= 0) {
		paramHandle->module = getModule_NoLock(paramHandle->moduleId);
	}

	Engine_refreshParamHandleCache(this);
}


json_t* Engine::toJson() {
	ReadLock lock(internal->mutex);
	json_t* rootJ = json_object();

	// modules
	json_t* modulesJ = json_array();
	for (Module* module : internal->modules) {
		json_t* moduleJ = module->toJson();
		json_array_append_new(modulesJ, moduleJ);
	}
	json_object_set_new(rootJ, "modules", modulesJ);

	// cables
	json_t* cablesJ = json_array();
	for (Cable* cable : internal->cables) {
		json_t* cableJ = cable->toJson();
		json_array_append_new(cablesJ, cableJ);
	}
	json_object_set_new(rootJ, "cables", cablesJ);

	return rootJ;
}


void Engine::fromJson(json_t* rootJ) {
	// Don't write-lock the entire method because most of it doesn't need it.

	// Write-locks
	clear();
	// modules
	json_t* modulesJ = json_object_get(rootJ, "modules");
	if (!modulesJ)
		return;
	size_t moduleIndex;
	json_t* moduleJ;
	json_array_foreach(modulesJ, moduleIndex, moduleJ) {
		// Get model
		plugin::Model* model;
		try {
			model = plugin::modelFromJson(moduleJ);
		}
		catch (Exception& e) {
			WARN("Cannot load model: %s", e.what());
			// APP->patch->log(e.what());
			continue;
		}

		// Create module
		Module* const module = model->createModule();
		DISTRHO_SAFE_ASSERT_CONTINUE(module != nullptr);

		// Create the widget too, needed by a few modules
		CardinalPluginModelHelper* const helper = dynamic_cast<CardinalPluginModelHelper*>(model);
		DISTRHO_SAFE_ASSERT_CONTINUE(helper != nullptr);

		app::ModuleWidget* const moduleWidget = helper->createModuleWidgetFromEngineLoad(module);
		DISTRHO_SAFE_ASSERT_CONTINUE(moduleWidget != nullptr);

		try {
			// This doesn't need a lock because the Module is not added to the Engine yet.
			module->fromJson(moduleJ);

			// Before 1.0, the module ID was the index in the "modules" array
			if (module->id < 0) {
				module->id = moduleIndex;
			}

			// Write-locks
			addModule(module);
		}
		catch (Exception& e) {
			WARN("Cannot load module: %s", e.what());
			// APP->patch->log(e.what());
			helper->removeCachedModuleWidget(module);
			delete module;
			continue;
		}
	}

	// cables
	json_t* cablesJ = json_object_get(rootJ, "cables");
	// Before 1.0, cables were called wires
	if (!cablesJ)
		cablesJ = json_object_get(rootJ, "wires");
	if (!cablesJ)
		return;
	size_t cableIndex;
	json_t* cableJ;
	json_array_foreach(cablesJ, cableIndex, cableJ) {
		// cable
		Cable* cable = new Cable;

		try {
			cable->fromJson(cableJ);

			// Before 1.0, the cable ID was the index in the "cables" array
			if (cable->id < 0) {
				cable->id = cableIndex;
			}

			// Write-locks
			addCable(cable);
		}
		catch (Exception& e) {
			WARN("Cannot load cable: %s", e.what());
			delete cable;
			// Don't log exceptions because missing modules create unnecessary complaining when cables try to connect to them.
			continue;
		}
	}
}


void Engine::startFallbackThread() {
}


} // namespace engine
} // namespace rack
