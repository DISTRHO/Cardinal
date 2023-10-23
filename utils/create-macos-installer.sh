#!/bin/bash

set -e

if [ -d bin ]; then
  cd bin
else
  echo "Please run this script from the root folder"
  exit
fi

rm -rf res jack native au lv2 vst2 vst3 clap
mkdir jack native au lv2 vst2 vst3 clap

mv Cardinal.app jack/CardinalJACK.app
mv CardinalNative.app native/CardinalNative.app

mv *.component au/
mv *.lv2 lv2/
mv *.vst vst2/
mv *.vst3 vst3/
mv *.clap clap/
cp -RL lv2/Cardinal.lv2/resources res
rm -rf lv2/*.lv2/resources
rm -rf vst2/*.vst/Contents/Resources
rm -rf vst3/*.vst3/Contents/Resources
rm -rf clap/*.clap/Contents/Resources

pkgbuild \
  --identifier "studio.kx.distrho.cardinal.resources" \
  --install-location "/Library/Application Support/Cardinal/" \
  --root "${PWD}/res/" \
  ../dpf-cardinal-resources.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.jack" \
  --component-plist "../utils/macOS/Build_JACK.plist" \
  --install-location "/Applications/" \
  --root "${PWD}/jack/" \
  ../dpf-cardinal-jack.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.native" \
  --component-plist "../utils/macOS/Build_Native.plist" \
  --install-location "/Applications/" \
  --root "${PWD}/native/" \
  ../dpf-cardinal-native.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.components" \
  --install-location "/Library/Audio/Plug-Ins/Components/" \
  --root "${PWD}/au/" \
  ../dpf-cardinal-components.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.lv2bundles" \
  --install-location "/Library/Audio/Plug-Ins/LV2/" \
  --root "${PWD}/lv2/" \
  ../dpf-cardinal-lv2bundles.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.vst2bundles" \
  --install-location "/Library/Audio/Plug-Ins/VST/" \
  --root "${PWD}/vst2/" \
  ../dpf-cardinal-vst2bundles.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.vst3bundles" \
  --install-location "/Library/Audio/Plug-Ins/VST3/" \
  --root "${PWD}/vst3/" \
  ../dpf-cardinal-vst3bundles.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.clapbundles" \
  --install-location "/Library/Audio/Plug-Ins/CLAP/" \
  --root "${PWD}/clap/" \
  ../dpf-cardinal-clapbundles.pkg

cd ..

sed -e "s|@builddir@|${PWD}/build|" \
    utils/macOS/package.xml.in > build/package.xml

productbuild \
  --distribution build/package.xml \
  --identifier "studio.kx.distrho.cardinal" \
  --package-path "${PWD}" \
  --version 0 \
  Cardinal-macOS.pkg
