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
 * This file is an edited version of VCVRack's plugin.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include <algorithm>
#include <map>

#include <plugin.hpp>


namespace rack {
namespace plugin {


/** Given slug => fallback slug.
Correctly handles bidirectional fallbacks.
To request fallback slugs to be added to this list, open a GitHub issue.
*/
static const std::map<std::string, std::string> pluginSlugFallbacks = {
	{"VultModulesFree", "VultModules"},
	{"VultModules", "VultModulesFree"},
	{"AudibleInstrumentsPreview", "AudibleInstruments"},
	// {"", ""},
};


Plugin* getPlugin(const std::string& pluginSlug) {
	if (pluginSlug.empty())
		return NULL;

	auto it = std::find_if(plugins.begin(), plugins.end(), [=](Plugin* p) {
		return p->slug == pluginSlug;
	});
	if (it != plugins.end())
		return *it;
	return NULL;
}


Plugin* getPluginFallback(const std::string& pluginSlug) {
	if (pluginSlug.empty())
		return NULL;

	// Attempt example plugin
	Plugin* p = getPlugin(pluginSlug);
	if (p)
		return p;

	// Attempt fallback plugin slug
	auto it = pluginSlugFallbacks.find(pluginSlug);
	if (it != pluginSlugFallbacks.end())
		return getPlugin(it->second);

	return NULL;
}


/** Given slug => fallback slug.
Correctly handles bidirectional fallbacks.
To request fallback slugs to be added to this list, open a GitHub issue.
*/
using PluginModuleSlug = std::tuple<std::string, std::string>;
static const std::map<PluginModuleSlug, PluginModuleSlug> moduleSlugFallbacks = {
	{{"Core", "AudioInterface2"}, {"Cardinal", "HostAudio2"}},
	{{"Core", "AudioInterface"}, {"Cardinal", "HostAudio8"}},
	{{"Core", "AudioInterface16"}, {"Cardinal", "HostAudio8"}},
	{{"Core", "MIDIToCVInterface"}, {"Cardinal", "HostMIDI"}},
	{{"Core", "CV-MIDI"}, {"Cardinal", "HostMIDI"}},
	{{"Core", "Notes"}, {"Cardinal", "TextEditor"}},
	{{"MindMeld-ShapeMasterPro", "ShapeMasterPro"}, {"MindMeldModular", "ShapeMaster"}},
	{{"MindMeldModular", "ShapeMaster"}, {"MindMeld-ShapeMasterPro", "ShapeMasterPro"}},
	// {{"", ""}, {"", ""}},
};


Model* getModel(const std::string& pluginSlug, const std::string& modelSlug) {
	if (pluginSlug.empty() || modelSlug.empty())
		return NULL;

	Plugin* p = getPlugin(pluginSlug);
	if (!p)
		return NULL;

	return p->getModel(modelSlug);
}


Model* getModelFallback(const std::string& pluginSlug, const std::string& modelSlug) {
	if (pluginSlug.empty() || modelSlug.empty())
		return NULL;

	// Attempt exact plugin and model
	Model* m = getModel(pluginSlug, modelSlug);
	if (m)
		return m;

	// Attempt fallback module
	auto it = moduleSlugFallbacks.find(std::make_tuple(pluginSlug, modelSlug));
	if (it != moduleSlugFallbacks.end()) {
		Model* m = getModel(std::get<0>(it->second), std::get<1>(it->second));
		if (m)
			return m;
	}

	// Attempt fallback plugin
	auto it2 = pluginSlugFallbacks.find(pluginSlug);
	if (it2 != pluginSlugFallbacks.end()) {
		Model* m = getModel(it2->second, modelSlug);
		if (m)
			return m;
	}

	return NULL;
}


Model* modelFromJson(json_t* moduleJ) {
	// Get slugs
	json_t* pluginSlugJ = json_object_get(moduleJ, "plugin");
	if (!pluginSlugJ)
		throw Exception("\"plugin\" property not found in module JSON");
	std::string pluginSlug = json_string_value(pluginSlugJ);
	pluginSlug = normalizeSlug(pluginSlug);

	json_t* modelSlugJ = json_object_get(moduleJ, "model");
	if (!modelSlugJ)
		throw Exception("\"model\" property not found in module JSON");
	std::string modelSlug = json_string_value(modelSlugJ);
	modelSlug = normalizeSlug(modelSlug);

	// Get Model
	Model* model = getModelFallback(pluginSlug, modelSlug);
	if (!model)
		throw Exception("Could not find module %s/%s", pluginSlug.c_str(), modelSlug.c_str());
	return model;
}


bool isSlugValid(const std::string& slug) {
	for (char c : slug) {
		if (!(std::isalnum(c) || c == '-' || c == '_'))
			return false;
	}
	return true;
}


std::string normalizeSlug(const std::string& slug) {
	std::string s;
	for (char c : slug) {
		if (!(std::isalnum(c) || c == '-' || c == '_'))
			continue;
		s += c;
	}
	return s;
}


std::vector<Plugin*> plugins;


} // namespace plugin
} // namespace rack
