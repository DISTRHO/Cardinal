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
 * This file is an edited version of VCVRack's plugin/Model.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include <algorithm>

#include <plugin/Model.hpp>
#include <plugin.hpp>
#include <asset.hpp>
#include <system.hpp>
#include <settings.hpp>
#include <string.hpp>
#include <tag.hpp>
#include <ui/Menu.hpp>
#include <ui/MenuSeparator.hpp>
#include <helpers.hpp>

#include "../CardinalCommon.hpp"


namespace rack {
namespace plugin {


void Model::fromJson(json_t* rootJ) {
	DISTRHO_SAFE_ASSERT_RETURN(plugin != nullptr,);

	json_t* nameJ = json_object_get(rootJ, "name");
	if (nameJ)
		name = json_string_value(nameJ);
	if (name == "")
		throw Exception("No module name for slug %s", slug.c_str());

	json_t* descriptionJ = json_object_get(rootJ, "description");
	if (descriptionJ)
		description = json_string_value(descriptionJ);

	// Tags
	tagIds.clear();
	json_t* tagsJ = json_object_get(rootJ, "tags");
	if (tagsJ) {
		size_t i;
		json_t* tagJ;
		json_array_foreach(tagsJ, i, tagJ) {
			std::string tag = json_string_value(tagJ);
			int tagId = tag::findId(tag);

			// Omit duplicates
			auto it = std::find(tagIds.begin(), tagIds.end(), tagId);
			if (it != tagIds.end())
				continue;

			if (tagId >= 0)
				tagIds.push_back(tagId);
		}
	}

	// manualUrl
	json_t* manualUrlJ = json_object_get(rootJ, "manualUrl");
	if (manualUrlJ)
		manualUrl = json_string_value(manualUrlJ);

	// hidden
	json_t* hiddenJ = json_object_get(rootJ, "hidden");
	// Use `disabled` as an alias which was deprecated in Rack 2.0
	if (!hiddenJ)
		hiddenJ = json_object_get(rootJ, "disabled");
	if (hiddenJ) {
		// Don't un-hide Model if already hidden by C++
		if (json_boolean_value(hiddenJ))
			hidden = true;
	}
}


std::string Model::getFullName() {
	DISTRHO_SAFE_ASSERT_RETURN(plugin, {});
	return plugin->getBrand() + " " + name;
}


std::string Model::getFactoryPresetDirectory() {
	return asset::plugin(plugin, system::join("presets", slug));
}


std::string Model::getUserPresetDirectory() {
	return asset::user(system::join("presets", plugin->slug, slug));
}


std::string Model::getManualUrl() {
	if (!manualUrl.empty())
		return manualUrl;
	return plugin->manualUrl;
}


void Model::appendContextMenu(ui::Menu* menu, bool inBrowser) {
	// plugin
	menu->addChild(createMenuItem("Plugin: " + plugin->name, "", [=]() {
		patchUtils::openBrowser(plugin->pluginUrl);
	}, plugin->pluginUrl == ""));

	// version
	menu->addChild(createMenuLabel("Version: " + plugin->version));

	// author
	if (plugin->author != "") {
		menu->addChild(createMenuItem("Author: " + plugin->author, "", [=]() {
			patchUtils::openBrowser(plugin->authorUrl);
		}, plugin->authorUrl.empty()));
	}

	// license
	std::string license = plugin->license;
	if (string::startsWith(license, "https://") || string::startsWith(license, "http://")) {
		menu->addChild(createMenuItem("License: Open in browser", "", [=]() {
			patchUtils::openBrowser(license);
		}));
	}
	else if (license != "") {
		menu->addChild(createMenuLabel("License: " + license));
	}

	// tags
	if (!tagIds.empty()) {
		menu->addChild(createMenuLabel("Tags:"));
		for (int tagId : tagIds) {
			menu->addChild(createMenuLabel("• " + tag::getTag(tagId)));
		}
	}

	menu->addChild(new ui::MenuSeparator);

	// manual
	std::string manualUrl = getManualUrl();
	if (manualUrl != "") {
		menu->addChild(createMenuItem("User manual", RACK_MOD_CTRL_NAME "+F1", [=]() {
			patchUtils::openBrowser(manualUrl);
		}));
	}

	// donate
	if (plugin->donateUrl != "") {
		menu->addChild(createMenuItem("Donate", "", [=]() {
			patchUtils::openBrowser(plugin->donateUrl);
		}));
	}

	// source code
	if (plugin->sourceUrl != "") {
		menu->addChild(createMenuItem("Source code", "", [=]() {
			patchUtils::openBrowser(plugin->sourceUrl);
		}));
	}

	// changelog
	if (plugin->changelogUrl != "") {
		menu->addChild(createMenuItem("Changelog", "", [=]() {
			patchUtils::openBrowser(plugin->changelogUrl);
		}));
	}

	// Favorite
	std::string favoriteRightText = inBrowser ? (RACK_MOD_CTRL_NAME "+click") : "";
	if (isFavorite())
		favoriteRightText += " " CHECKMARK_STRING;
	menu->addChild(createMenuItem("Favorite", favoriteRightText,
		[=]() {
			setFavorite(!isFavorite());
		}
	));
}


bool Model::isFavorite() {
	const settings::ModuleInfo* mi = settings::getModuleInfo(plugin->slug, slug);
	return mi && mi->favorite;
}


void Model::setFavorite(bool favorite) {
	settings::ModuleInfo& mi = settings::moduleInfos[plugin->slug][slug];
	mi.favorite = favorite;
}


} // namespace plugin
} // namespace rack
