#!/bin/bash

cd $(dirname ${0})

diff -U3 ../Rack/include/midi.hpp ../../include/midi.hpp > diffs/midi.hpp.diff
diff -U3 ../Rack/include/dsp/fir.hpp ../../include/dsp/fir.hpp > diffs/dsp-fir.hpp.diff
diff -U3 ../Rack/include/engine/Port.hpp ../../include/engine/Port.hpp > diffs/engine-Port.hpp.diff
diff -U3 ../Rack/include/simd/Vector.hpp ../../include/simd/Vector.hpp > diffs/simd-Vector.hpp.diff

diff -U3 ../Rack/dep/oui-blendish/blendish.c blendish.c > diffs/blendish.c.diff

diff -U3 ../Rack/src/common.cpp common.cpp > diffs/common.cpp.diff
diff -U3 ../Rack/src/context.cpp context.cpp > diffs/context.cpp.diff
diff -U3 ../Rack/src/plugin.cpp plugin.cpp > diffs/plugin.cpp.diff
diff -U3 ../Rack/src/app/MenuBar.cpp MenuBar.cpp > diffs/MenuBar.cpp.diff
diff -U3 ../Rack/src/app/ModuleWidget.cpp ModuleWidget.cpp > diffs/ModuleWidget.cpp.diff
diff -U3 ../Rack/src/app/Scene.cpp Scene.cpp > diffs/Scene.cpp.diff
diff -U3 ../Rack/src/engine/Engine.cpp Engine.cpp > diffs/Engine.cpp.diff
diff -U3 ../Rack/src/dsp/minblep.cpp minblep.cpp > diffs/minblep.cpp.diff
diff -U3 ../Rack/src/plugin/Model.cpp Model.cpp > diffs/Model.cpp.diff
diff -U3 ../Rack/src/widget/OpenGlWidget.cpp OpenGlWidget.cpp > diffs/OpenGlWidget.cpp.diff
diff -U3 ../Rack/src/window/Window.cpp Window.cpp > diffs/Window.cpp.diff
