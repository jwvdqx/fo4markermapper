// Portable (no F4SE dependency) icon lookup for the PNG drop-in folder.
// Order: locations/<EditorID|0xFormID>.png  ->  types/<Type>.png  ->  none (keep vanilla).
#pragma once
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>

class IconResolver {
public:
    explicit IconResolver(std::filesystem::path root) : root_(std::move(root)) { Reload(); }

    void Reload() {
        types_.clear(); locations_.clear();
        Scan(root_ / "types", types_);
        Scan(root_ / "locations", locations_);
    }
    // markerType: e.g. "SettlementMarker" (trailing "Marker" optional). location: editor ID or "0x0001A2B3", may be empty.
    std::optional<std::filesystem::path> Resolve(std::string markerType, const std::string& location = {}) const {
        if (!location.empty())
            if (auto it = locations_.find(Key(location)); it != locations_.end()) return it->second;
        if (markerType.size() > 6 && Key(markerType).ends_with("marker")) markerType.resize(markerType.size() - 6);
        if (auto it = types_.find(Key(markerType)); it != types_.end()) return it->second;
        return std::nullopt;
    }
    size_t TypeCount() const { return types_.size(); }
    size_t LocationCount() const { return locations_.size(); }

private:
    using Map = std::unordered_map<std::string, std::filesystem::path>;
    static std::string Key(std::string s) { std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); }); return s; }
    static void Scan(const std::filesystem::path& dir, Map& out) {
        std::error_code ec;
        for (auto& e : std::filesystem::directory_iterator(dir, ec))
            if (e.is_regular_file() && Key(e.path().extension().string()) == ".png")
                out[Key(e.path().stem().string())] = e.path();
    }
    std::filesystem::path root_;
    Map types_, locations_;
};
