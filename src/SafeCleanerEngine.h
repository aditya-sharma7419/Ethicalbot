#pragma once

#include <string>
#include <vector>

struct ScanCandidate {
    std::string kind;
    std::string path;
    std::string reason;
};

class SafeCleanerEngine {
public:
    static bool isProtectedPath(const std::string& path);
    static std::vector<ScanCandidate> scanDevice(const std::string& mode);
    static std::string summarize(const std::vector<ScanCandidate>& candidates, const std::string& mode);
    static std::string createRestorePointDescription();
};
