//***********************************************************************************************
//Impromptu Modular: Modules for VCV Rack by Marc Boulé
//
//See ./LICENSE.md for all licenses
//***********************************************************************************************


#include "../ImpromptuModular/src/comp/PanelTheme.hpp"



void loadThemeAndContrastFromDefault(int* panelTheme, float* panelContrast) {
	*panelTheme = 1;
	*panelContrast = panelContrastDefault;
}


bool isDark(int*) {
	return true;
}


void createPanelThemeMenu(ui::Menu*, int*, float*, SvgPanel*) {}


void PanelBaseWidget::draw(const DrawArgs& args) {
	nvgBeginPath(args.vg);
	NVGcolor baseColor;
	if (panelContrastSrc) {
		baseColor = nvgRGB(*panelContrastSrc, *panelContrastSrc, *panelContrastSrc);
	}
	else {
		baseColor = nvgRGB(panelContrastDefault, panelContrastDefault, panelContrastDefault);
	}
	nvgFillColor(args.vg, baseColor);
	nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
	nvgFill(args.vg);
	TransparentWidget::draw(args);
}


void InverterWidget::draw(const DrawArgs& args) {
	TransparentWidget::draw(args);
	{
		// nvgSave(args.vg);
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, SCHEME_WHITE);// this is the source, the current framebuffer is the dest	
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgGlobalCompositeBlendFuncSeparate(args.vg, 
			NVG_ONE_MINUS_DST_COLOR,// srcRGB
			NVG_ZERO,// dstRGB
			NVG_ONE_MINUS_DST_COLOR,// srcAlpha
			NVG_ONE);// dstAlpha
		// blend factor: https://github.com/memononen/nanovg/blob/master/src/nanovg.h#L86
		// OpenGL blend doc: https://www.khronos.org/opengl/wiki/Blending
		nvgFill(args.vg);
		nvgClosePath(args.vg);
		// nvgRestore(args.vg);
	}
}

