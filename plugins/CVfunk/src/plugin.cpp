#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelSteps);
	p->addModel(modelEnvelopeArray);
	p->addModel(modelPentaSequencer);
	p->addModel(modelImpulseController);
	p->addModel(modelSignals);
	p->addModel(modelRanges);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
