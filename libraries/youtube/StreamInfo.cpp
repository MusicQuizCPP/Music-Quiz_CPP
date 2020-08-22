#include "StreamInfo.hpp"

#include <ostream>

using namespace youtube;

std::ostream& youtube::operator<<(std::ostream& os, const StreamInfo& info)
{
    return os << "URL: " << info.url << "\n"
            << "durationMs: " << info.duration.count() << "\n"
            << "quality: " << info.quality << "\n"
            << "author: " << info.author << "\n"
            << "title: " << info.title << "\n"
            << "mimeType: " << info.mimeType << "\n"
            << "itag: " << info.iTag << "\n";
}
