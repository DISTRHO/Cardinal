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

#include <rack.hpp>

using namespace rack;

namespace rack {
namespace app {

Knob::Knob() {}
Knob::~Knob() {}
void Knob::initParamQuantity() {}
void Knob::onHover(const HoverEvent& e) {}
void Knob::onButton(const ButtonEvent& e) {}
void Knob::onDragStart(const DragStartEvent& e) {}
void Knob::onDragEnd(const DragEndEvent& e) {}
void Knob::onDragMove(const DragMoveEvent& e) {}
void Knob::onDragLeave(const DragLeaveEvent& e) {}
void Knob::onHoverScroll(const HoverScrollEvent& e) {}
void Knob::onLeave(const LeaveEvent& e) {}

void LightWidget::draw(const DrawArgs& args) {}
void LightWidget::drawLayer(const DrawArgs& args, int layer) {}
void LightWidget::drawBackground(const DrawArgs& args) {}
void LightWidget::drawLight(const DrawArgs& args) {}
void LightWidget::drawHalo(const DrawArgs& args) {}

ModuleLightWidget::~ModuleLightWidget() {}
engine::Light* ModuleLightWidget::getLight(int colorId) { return nullptr; }
engine::LightInfo* ModuleLightWidget::getLightInfo() { return nullptr; }
void ModuleLightWidget::createTooltip() {}
void ModuleLightWidget::destroyTooltip() {}
void ModuleLightWidget::step() {}
void ModuleLightWidget::onHover(const HoverEvent& e) {}
void ModuleLightWidget::onEnter(const EnterEvent& e) {}
void ModuleLightWidget::onLeave(const LeaveEvent& e) {}

ModuleWidget::ModuleWidget() {}
ModuleWidget::~ModuleWidget() {}
plugin::Model* ModuleWidget::getModel() { return nullptr; }
void ModuleWidget::setModel(plugin::Model* model) {}
engine::Module* ModuleWidget::getModule() { return nullptr; }
void ModuleWidget::setModule(engine::Module* module) {}
widget::Widget* ModuleWidget::getPanel() { return nullptr; }
void ModuleWidget::setPanel(widget::Widget* panel) {}
void ModuleWidget::setPanel(std::shared_ptr<window::Svg> svg) {}
void ModuleWidget::addParam(ParamWidget* param) {}
void ModuleWidget::addInput(PortWidget* input) {}
void ModuleWidget::addOutput(PortWidget* output) {}
ParamWidget* ModuleWidget::getParam(int paramId) { return nullptr; }
PortWidget* ModuleWidget::getInput(int portId) { return nullptr; }
PortWidget* ModuleWidget::getOutput(int portId) { return nullptr; }
std::vector<ParamWidget*> ModuleWidget::getParams() { return {}; }
std::vector<PortWidget*> ModuleWidget::getPorts() { return {}; }
std::vector<PortWidget*> ModuleWidget::getInputs() { return {}; }
std::vector<PortWidget*> ModuleWidget::getOutputs() { return {}; }
void ModuleWidget::draw(const DrawArgs& args) {}
void ModuleWidget::drawLayer(const DrawArgs& args, int layer) {}
void ModuleWidget::onHover(const HoverEvent& e) {}
void ModuleWidget::onHoverKey(const HoverKeyEvent& e) {}
void ModuleWidget::onButton(const ButtonEvent& e) {}
void ModuleWidget::onDragStart(const DragStartEvent& e) {}
void ModuleWidget::onDragEnd(const DragEndEvent& e) {}
void ModuleWidget::onDragMove(const DragMoveEvent& e) {}
void ModuleWidget::onDragHover(const DragHoverEvent& e) {}
json_t* ModuleWidget::toJson() { return nullptr; }
void ModuleWidget::fromJson(json_t* rootJ) {}
bool ModuleWidget::pasteJsonAction(json_t* rootJ) { return false; }
void ModuleWidget::copyClipboard() {}
bool ModuleWidget::pasteClipboardAction() { return false; }
void ModuleWidget::load(std::string filename) {}
void ModuleWidget::loadAction(std::string filename) {}
void ModuleWidget::loadTemplate() {}
void ModuleWidget::loadDialog() {}
void ModuleWidget::save(std::string filename) {}
void ModuleWidget::saveTemplate() {}
void ModuleWidget::saveTemplateDialog() {}
bool ModuleWidget::hasTemplate() { return false; }
void ModuleWidget::clearTemplate() {}
void ModuleWidget::clearTemplateDialog() {}
void ModuleWidget::saveDialog() {}
void ModuleWidget::disconnect() {}
void ModuleWidget::resetAction() {}
void ModuleWidget::randomizeAction() {}
void ModuleWidget::appendDisconnectActions(history::ComplexAction* complexAction) {}
void ModuleWidget::disconnectAction() {}
void ModuleWidget::cloneAction(bool cloneCables) {}
void ModuleWidget::bypassAction(bool bypassed) {}
void ModuleWidget::removeAction() {}
void ModuleWidget::createContextMenu() {}
math::Vec& ModuleWidget::dragOffset() { static math::Vec r; return r; }
bool& ModuleWidget::dragEnabled() { static bool r; return r; }
math::Vec& ModuleWidget::oldPos() { static math::Vec r; return r; }
engine::Module* ModuleWidget::releaseModule() { return nullptr; }

int MultiLightWidget::getNumColors() { return 0; }
void MultiLightWidget::addBaseColor(NVGcolor baseColor) {}
void MultiLightWidget::setBrightnesses(const std::vector<float>& brightnesses) {}

ParamWidget::ParamWidget() {}
ParamWidget::~ParamWidget() {}
engine::ParamQuantity* ParamWidget::getParamQuantity() { return nullptr; }
void ParamWidget::createTooltip() {}
void ParamWidget::destroyTooltip() {}
void ParamWidget::step() {}
void ParamWidget::draw(const DrawArgs& args) {}
void ParamWidget::onButton(const ButtonEvent& e) {}
void ParamWidget::onDoubleClick(const DoubleClickEvent& e) {}
void ParamWidget::onEnter(const EnterEvent& e) {}
void ParamWidget::onLeave(const LeaveEvent& e) {}
void ParamWidget::createContextMenu() {}
void ParamWidget::resetAction() {}

PortWidget::PortWidget() {}
PortWidget::~PortWidget() {}
engine::Port* PortWidget::getPort() { return nullptr; }
engine::PortInfo* PortWidget::getPortInfo() { return nullptr; }
void PortWidget::createTooltip() {}
void PortWidget::destroyTooltip() {}
void PortWidget::createContextMenu() {}
void PortWidget::deleteTopCableAction() {}
void PortWidget::step() {}
void PortWidget::draw(const DrawArgs& args) {}
void PortWidget::onButton(const ButtonEvent& e) {}
void PortWidget::onEnter(const EnterEvent& e) {}
void PortWidget::onLeave(const LeaveEvent& e) {}
void PortWidget::onDragStart(const DragStartEvent& e) {}
void PortWidget::onDragEnd(const DragEndEvent& e) {}
void PortWidget::onDragDrop(const DragDropEvent& e) {}
void PortWidget::onDragEnter(const DragEnterEvent& e) {}
void PortWidget::onDragLeave(const DragLeaveEvent& e) {}

SliderKnob::SliderKnob() {}
void SliderKnob::onHover(const HoverEvent& e) {}
void SliderKnob::onButton(const ButtonEvent& e) {}

SvgKnob::SvgKnob() {}
void SvgKnob::setSvg(std::shared_ptr<window::Svg> svg) {}
void SvgKnob::onChange(const ChangeEvent& e) {}

SvgPort::SvgPort() {}
void SvgPort::setSvg(std::shared_ptr<window::Svg> svg) {}

SvgScrew::SvgScrew() {}
void SvgScrew::setSvg(std::shared_ptr<window::Svg> svg) {}

SvgSlider::SvgSlider() {}
void SvgSlider::setBackgroundSvg(std::shared_ptr<window::Svg> svg) {}
void SvgSlider::setHandleSvg(std::shared_ptr<window::Svg> svg) {}
void SvgSlider::setHandlePos(math::Vec minHandlePos, math::Vec maxHandlePos) {}
void SvgSlider::setHandlePosCentered(math::Vec minHandlePosCentered, math::Vec maxHandlePosCentered) {}
void SvgSlider::onChange(const ChangeEvent& e) {}

}

namespace widget {

FramebufferWidget::FramebufferWidget() {}
FramebufferWidget::~FramebufferWidget() {}
void FramebufferWidget::setDirty(bool dirty) {}
int FramebufferWidget::getImageHandle() { return 0; }
NVGLUframebuffer* FramebufferWidget::getFramebuffer() { return nullptr; }
math::Vec FramebufferWidget::getFramebufferSize() { return {}; }
void FramebufferWidget::deleteFramebuffer() {}
void FramebufferWidget::step() {}
void FramebufferWidget::draw(const DrawArgs& args) {}
void FramebufferWidget::render(math::Vec scale, math::Vec offsetF, math::Rect clipBox) {}
void FramebufferWidget::drawFramebuffer() {}
void FramebufferWidget::onDirty(const DirtyEvent& e) {}
void FramebufferWidget::onContextCreate(const ContextCreateEvent& e) {}
void FramebufferWidget::onContextDestroy(const ContextDestroyEvent& e) {}

SvgWidget::SvgWidget() {}
void SvgWidget::wrap() {}
void SvgWidget::setSvg(std::shared_ptr<window::Svg> svg) {}
void SvgWidget::draw(const DrawArgs& args) {}

Widget::~Widget() {}
math::Rect Widget::getBox() { return {}; }
void Widget::setBox(math::Rect box) {}
math::Vec Widget::getPosition() { return {}; }
void Widget::setPosition(math::Vec pos) {}
math::Vec Widget::getSize() { return {}; }
void Widget::setSize(math::Vec size) {}
widget::Widget* Widget::getParent() { return nullptr; }
bool Widget::isVisible() { return false; }
void Widget::setVisible(bool visible) {}
void Widget::requestDelete() {}
math::Rect Widget::getChildrenBoundingBox() { return {}; }
math::Rect Widget::getVisibleChildrenBoundingBox() { return {}; }
bool Widget::isDescendantOf(Widget* ancestor) { return false; }
math::Vec Widget::getRelativeOffset(math::Vec v, Widget* ancestor) { return {}; }
float Widget::getRelativeZoom(Widget* ancestor) { return 0.0f; }
math::Rect Widget::getViewport(math::Rect r) { return {}; }
bool Widget::hasChild(Widget* child) { return false; }
void Widget::addChild(Widget* child) {}
void Widget::addChildBottom(Widget* child) {}
void Widget::addChildBelow(Widget* child, Widget* sibling) {}
void Widget::addChildAbove(Widget* child, Widget* sibling) {}
void Widget::removeChild(Widget* child) {}
void Widget::clearChildren() {}
void Widget::step() {}
void Widget::draw(const DrawArgs& args) {}
void Widget::drawLayer(const DrawArgs& args, int layer) {}
void Widget::drawChild(Widget* child, const DrawArgs& args, int layer) {}

}

namespace window {

Svg::~Svg() {}
void Svg::loadFile(const std::string& filename) {}
void Svg::loadString(const std::string& str) {}
math::Vec Svg::getSize() { return {}; }
int Svg::getNumShapes() { return 0; }
int Svg::getNumPaths() { return 0; }
int Svg::getNumPoints() { return 0; }
void Svg::draw(NVGcontext* vg) {}
std::shared_ptr<Svg> Svg::load(const std::string& filename) { return {}; }

}

}
