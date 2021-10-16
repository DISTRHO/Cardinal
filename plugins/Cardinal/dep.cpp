// This source file compiles those nice implementation-in-header little libraries

#include <common.hpp> // for fopen_u8

#ifdef NDEBUG
# undef DEBUG
#endif
#include "OpenGL.hpp"

#define BLENDISH_IMPLEMENTATION
#include <blendish.h>

#define NANOSVG_IMPLEMENTATION
#define NANOSVG_ALL_COLOR_KEYWORDS
#include <nanosvg.h>

#include <library.hpp>
#include <network.hpp>

namespace rack {
namespace library {
    std::string appChangelogUrl;
    std::string appDownloadUrl;
    std::string appVersion;
    std::string loginStatus;
    std::map<std::string, UpdateInfo> updateInfos;
    std::string updateStatus;
    std::string updateSlug;
    float updateProgress = 0.f;
    bool isSyncing = false;
    bool restartRequested = false;
    void checkAppUpdate() {}
    void checkUpdates() {}
    bool hasUpdates() { return false; }
    bool isAppUpdateAvailable() { return false; }
    bool isLoggedIn() { return false; }
    void logIn(const std::string&, const std::string&) {}
    void logOut() {}
    void syncUpdate(const std::string&) {}
    void syncUpdates() {}
}
namespace network {
    std::string encodeUrl(const std::string&) { return {}; }
    json_t* requestJson(Method, const std::string&, json_t*, const CookieMap&) { return nullptr; }
    bool requestDownload(const std::string&, const std::string&, float*, const CookieMap&) { return false; }
}
}
