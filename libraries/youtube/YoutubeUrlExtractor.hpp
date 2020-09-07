#pragma once

#include <chrono>
#include <cstddef>
#include <map>
#include <ostream>

#include <nlohmann/json.hpp>

#include "HTTPSClient.hpp"
#include "StreamInfo.hpp"

namespace youtube {

class YoutubeUrlExtractor : public HTTPSClient {
public:
    /**
     * @brief Constructor.
     * @param[in] url youtube video url to extract information from
     */
    YoutubeUrlExtractor(std::string url);

    /**
     * @brief Get vector of available streams.
     * @return vector of available streams
     */
    std::vector<StreamInfo> getStreamInfo();

    /**
     * @brief Get extracted title
     * @return title
     */
    std::string getTitle();

    /**
     * @brief Get extracted author
     * @return author
     */
    std::string getAuthor();

    /**
     * @brief Get video stream info best fitting to prefered format
     * @param[in] preferedFormats formats to search for, in order of most prefered
     * @return stream info
     */
    StreamInfo getVideoStream(std::vector<std::string> preferedFormats = {"hd2160", "hd1440", "hd1080", "hd720", "large", "medium", "small", "tiny"});

    /**
     * @brief Get audio stream info best fitting to prefered format
     * @param[in] preferedFormats formats to search for, in order of most prefered
     * @return stream info
     */
    StreamInfo getAudioStream(std::vector<std::string> preferedFormats = {"large", "medium", "small", "tiny"});

    /**
     * @brief Get stream info best fitting to prefered format and mimeType
     * @param[in] preferedFormats formats to search for, in order of most prefered
     * @param[in] mimeType. Only videos which contains the given string in their mimeType will be selected
     * @return stream info
     */
    StreamInfo getStream(std::vector<std::string> preferedFormats, std::string mimeType);

    /**
     * @brief Extract youtube video ID from url
     * @param[in] url url to extract videoID from
     * @return stream info
     */
    std::string getVideoID(std::string url);

private:
    /**
     * @brief Split up URI formatted string in key:value pairs
     * @param[in] uri uri formatted string
     * @return map of key:values extracted from string
     */
    std::map<std::string, std::string> parseURI(std::string uri);

    /**
     * @brief Get youtube player json string containing stream info
     * @param[in] id video ID
     * @return youtube player json string
     */
    std::string getPlayerJsonStr(std::string id);

    std::string    _videoID;
    nlohmann::json _playerDataJson;

    static const inline std::vector<uint32_t> _itagBlacklist = {
        248, 399, 136, 247, 398, 135, 244, 397, 134, 395, 243, 396, 133, 242, 278, 394, 137, 160,
        315, // 2160p no audio
        313, // 2160p no audio
        308, // 1440p no audio
        271, // 1440p no audio
        303, // 1080p no audio
        299, // 1080p no audio
        303, // 720p no audio
        302, // 720p no audio
        298, // 720p no audio
    };
};
} // namespace youtube
