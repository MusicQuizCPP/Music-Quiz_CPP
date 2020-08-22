#pragma once

#include <chrono>
#include <string>
#include <cstdint>

namespace youtube {
    struct StreamInfo {
        std::string url;
        std::chrono::milliseconds duration;
        std::string quality;
        std::string author;
        std::string title;
        std::string mimeType;
        uint32_t iTag;

        friend std::ostream& operator<<(std::ostream& os, const StreamInfo& info);
    };

    std::ostream& operator<<(std::ostream& os, const StreamInfo& info);
}
