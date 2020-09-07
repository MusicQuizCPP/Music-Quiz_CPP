#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "StreamInfo.hpp"

namespace youtube {
class HTTPSClient;
class YoutubeUrlExtractor;

class YoutubeDownloader {
public:
    /**
     * @brief Constructor.
     * @param[in] url youtube video url to download
     * @param[in] preferedFormats list of prefered video formats in order of preference
     */

    YoutubeDownloader(std::string url, std::vector<std::string> preferedFormats = {"hd720", "large", "medium", "small", "tiny"});

    /**
     * @brief Destructor.
     */
    ~YoutubeDownloader();

    /**
     * @brief Get info about the downloading stream.
     * @return StreamInfo object holding various info about the stream
     */
    StreamInfo getStreamInfo() { return _streamInfo; }

    /**
     * @brief Read the next chunk of data from the stream
     *
     * @param[out] buf Buffer the data should be copied to
     * @param[in] size size of the buffer
     *
     * @return the amount of data read. returns 0 when stream ends.
     */
    int getChunk(void* buf, int size);

    /**
     * @brief Get number of bytes downloaded from the stream so far
     *
     * @return number of bytes downloaded so far
     */
    size_t getDownloadedBytes();

private:
    /**
     * @brief Restart the download attempt and read first chunk
     *
     * @param[out] buf Buffer the data should be copied to
     * @param[in] size size of the buffer
     *
     * @return the amount of data read. return 0 if no data.
     */
    int retry(void* buf, int size);

    std::string              _url;
    std::vector<std::string> _preferedFormats;
    HTTPSClient*             _downloadClient = nullptr;
    YoutubeUrlExtractor*     _urlExtractor   = nullptr;
    StreamInfo               _streamInfo;
    int                      _maxRetries      = 5;
    int                      _retryCount      = 0;
    size_t                   _downloadedBytes = 0;
};
} // namespace youtube