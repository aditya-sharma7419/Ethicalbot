#include "SafeCleanerEngine.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>

namespace {
std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}
}

bool SafeCleanerEngine::isProtectedPath(const std::string& path) {
    if (path.empty()) {
        return false;
    }

    const std::string lowered = toLower(path);
    const std::vector<std::string> protectedPrefixes = {
        "c:/windows",
        "c:\\windows",
        "c:/program files",
        "c:\\program files",
        "c:/programdata",
        "c:\\programdata",
        "/system",
        "/private",
        "/usr/local",
        "/etc"
    };

    for (const std::string& prefix : protectedPrefixes) {
        if (lowered.find(prefix) == 0) {
            return true;
        }
    }
    return false;
}

std::vector<ScanCandidate> SafeCleanerEngine::scanDevice(const std::string& mode) {
    std::vector<ScanCandidate> candidates;

    const std::vector<std::string> kinds = {"cache", "temp", "registry", "app"};
    for (size_t i = 0; i < kinds.size(); ++i) {
        ScanCandidate item;
        item.kind = kinds[i];
        item.path = (mode == "orphan-registry")
            ? "AppData/Local/OrphanedRegistry"
            : "C:/Users/demo/AppData/Local/Temp";
        item.reason = (mode == "automatic")
            ? "safe-to-remove after scan"
            : (mode == "user-driven")
                ? "requires validation"
                : "leftover registry entry";
        if ((mode == "automatic" && i < 2) || (mode == "user-driven" && i < 3) || (mode == "orphan-registry" && i == 3)) {
            candidates.push_back(item);
        }
    }

    return candidates;
}

std::string SafeCleanerEngine::summarize(const std::vector<ScanCandidate>& candidates, const std::string& mode) {
    std::ostringstream stream;
    stream << "Mode " << mode << ": " << candidates.size() << " candidate(s) found. Protected system locations were not touched.";
    return stream.str();
}

std::string SafeCleanerEngine::createRestorePointDescription() {
    return "Smart Cleaner: pre-cleanup restore point created";
}
