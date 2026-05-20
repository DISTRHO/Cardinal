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

if [ -n "${MACOS_APP_CERTIFICATE}" ] && [ -n "${MACOS_INSTALLER_CERTIFICATE}" ] && [ -n "${MACOS_CERTIFICATE_PASSWORD}" ]; then
    security create-keychain -p "" $(pwd)/keychain.db
    security unlock-keychain -p "" $(pwd)/keychain.db
    echo -n "${MACOS_APP_CERTIFICATE}" | base64 --decode -o cert.p12
    security import cert.p12 -P "${MACOS_CERTIFICATE_PASSWORD}" -A -t cert -f pkcs12 -k $(pwd)/keychain.db
    echo -n "${MACOS_INSTALLER_CERTIFICATE}" | base64 --decode -o cert.p12
    security import cert.p12 -P "${MACOS_CERTIFICATE_PASSWORD}" -A -t cert -f pkcs12 -k $(pwd)/keychain.db
    rm cert.p12
    security list-keychain -d user -s $(pwd)/keychain.db

    MACOS_APP_DEV_ID="$(security find-identity -v $(pwd)/keychain.db | grep 'Developer ID Application:' | head -n 1 | cut -d' ' -f 5-99 | sed 's/\"//g')"
    codesign -s "${MACOS_APP_DEV_ID}" --deep --force --verbose --option=runtime au/*.component
    codesign -s "${MACOS_APP_DEV_ID}" --deep --force --verbose --option=runtime clap/*.clap
    codesign -s "${MACOS_APP_DEV_ID}" --deep --force --verbose --option=runtime jack/*.app
    codesign -s "${MACOS_APP_DEV_ID}" --deep --force --verbose --option=runtime native/*.app
    codesign -s "${MACOS_APP_DEV_ID}" --deep --force --verbose --option=runtime vst2/*.vst
    codesign -s "${MACOS_APP_DEV_ID}" --deep --force --verbose --option=runtime vst3/*.vst3
    codesign -s "${MACOS_APP_DEV_ID}" --force --verbose --option=runtime lv2/*.lv2/*.dylib

    MACOS_INSTALLER_DEV_ID="$(security find-identity -v $(pwd)/keychain.db | grep 'Developer ID Installer:' | head -n 1 | cut -d' ' -f 5-99 | sed 's/\"//g')"
    PKG_SIGN_ARGS=(--sign "${MACOS_INSTALLER_DEV_ID}")
fi

pkgbuild \
  --identifier "studio.kx.distrho.cardinal.resources" \
  --install-location "/Library/Application Support/Cardinal/" \
  --root "${PWD}/res/" \
  "${PKG_SIGN_ARGS[@]}" \
  ../dpf-cardinal-resources.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.jack" \
  --component-plist "../utils/macOS/Build_JACK.plist" \
  --install-location "/Applications/" \
  --root "${PWD}/jack/" \
  "${PKG_SIGN_ARGS[@]}" \
  ../dpf-cardinal-jack.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.native" \
  --component-plist "../utils/macOS/Build_Native.plist" \
  --install-location "/Applications/" \
  --root "${PWD}/native/" \
  "${PKG_SIGN_ARGS[@]}" \
  ../dpf-cardinal-native.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.components" \
  --install-location "/Library/Audio/Plug-Ins/Components/" \
  --root "${PWD}/au/" \
  "${PKG_SIGN_ARGS[@]}" \
  ../dpf-cardinal-components.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.lv2bundles" \
  --install-location "/Library/Audio/Plug-Ins/LV2/" \
  --root "${PWD}/lv2/" \
  "${PKG_SIGN_ARGS[@]}" \
  ../dpf-cardinal-lv2bundles.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.vst2bundles" \
  --install-location "/Library/Audio/Plug-Ins/VST/" \
  --root "${PWD}/vst2/" \
  "${PKG_SIGN_ARGS[@]}" \
  ../dpf-cardinal-vst2bundles.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.vst3bundles" \
  --install-location "/Library/Audio/Plug-Ins/VST3/" \
  --root "${PWD}/vst3/" \
  "${PKG_SIGN_ARGS[@]}" \
  ../dpf-cardinal-vst3bundles.pkg

pkgbuild \
  --identifier "studio.kx.distrho.plugins.cardinal.clapbundles" \
  --install-location "/Library/Audio/Plug-Ins/CLAP/" \
  --root "${PWD}/clap/" \
  "${PKG_SIGN_ARGS[@]}" \
  ../dpf-cardinal-clapbundles.pkg

cd ..

sed -e "s|@builddir@|${PWD}/build|" \
    utils/macOS/package.xml.in > build/package.xml

productbuild \
  --distribution build/package.xml \
  --identifier "studio.kx.distrho.cardinal" \
  --package-path "${PWD}" \
  --version 0 \
  "${PKG_SIGN_ARGS[@]}" \
  Cardinal-macOS.pkg

if [ -n "${MACOS_NOTARIZATION_USER}" ] && [ -n "${MACOS_NOTARIZATION_PASS}" ] && [ -n "${MACOS_NOTARIZATION_TEAM}" ]; then
  xcrun notarytool submit Cardinal-macOS.pkg \
    --apple-id ${MACOS_NOTARIZATION_USER} \
    --password ${MACOS_NOTARIZATION_PASS} \
    --team-id ${MACOS_NOTARIZATION_TEAM} \
    --wait
  xcrun stapler staple Cardinal-macOS.pkg
fi
