#!/bin/bash

cd $(dirname ${0})

diff -U3 ../Rack/dep/oui-blendish/blendish.c blendish.c > diffs/blendish.c.diff
diff -U3 ../Rack/src/common.cpp common.cpp > diffs/common.cpp.diff
diff -U3 ../Rack/src/context.cpp context.cpp > diffs/context.cpp.diff
diff -U3 ../Rack/src/plugin.cpp plugin.cpp > diffs/plugin.cpp.diff
diff -U3 ../Rack/src/app/MenuBar.cpp MenuBar.cpp > diffs/MenuBar.cpp.diff
diff -U3 ../Rack/src/app/Scene.cpp Scene.cpp > diffs/Scene.cpp.diff
diff -U3 ../Rack/src/engine/Engine.cpp Engine.cpp > diffs/Engine.cpp.diff
diff -U3 ../Rack/src/plugin/Model.cpp Model.cpp > diffs/Model.cpp.diff
diff -U3 ../Rack/src/window/Window.cpp Window.cpp > diffs/Window.cpp.diff
