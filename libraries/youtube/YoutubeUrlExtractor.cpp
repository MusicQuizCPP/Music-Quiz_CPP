#include "YoutubeUrlExtractor.hpp"
#include "YoutubeDownloadException.hpp"
#include <iostream>

#include <iostream>
#include <string>
#include <regex>

using namespace youtube;

namespace beast = boost::beast; // from <boost/beast.hpp>


//Stolen shamelessly from https://stackoverflow.com/questions/2673207/c-c-url-decode-library
static int percent_decode(std::string& out, const char* in) {
    static const char tbl[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
         0, 1, 2, 3, 4, 5, 6, 7,  8, 9,-1,-1,-1,-1,-1,-1,
        -1,10,11,12,13,14,15,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,10,11,12,13,14,15,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1
    };
    out.clear();

    char c, v1, v2;
    if(in != NULL) {
        while((c=*in++) != '\0') {
            if(c == '%') {
                if((v1=tbl[static_cast<unsigned char>(*in++)])<0 || 
                   (v2=tbl[static_cast<unsigned char>(*in++)])<0) {
                    out.clear();
                    return -1;
                }
                c = static_cast<char>((v1<<4)|v2);
            }
            out += c;
        }
    }
    return 0;
}

YoutubeUrlExtractor::YoutubeUrlExtractor(std::string url) : HTTPSClient("youtube.com"),
_videoID(getVideoID(url))
{
    std::string playerDataJsonStr = getPlayerJsonStr(_videoID);
    _playerDataJson = nlohmann::json::parse(playerDataJsonStr);
}

std::string YoutubeUrlExtractor::getVideoID(std::string url)
{
    try
    {
        if(url.find("youtu") == std::string::npos)
        {
            return "";
        }

        std::regex rgx("(?:v|embed|watch\?v)(?:=|\\/)([^\"&?\\/=%]{11})");

        std::smatch match;
        std::regex_search(url, match, rgx);

        return match[1];
    } catch( ... )
    {
        throw YoutubeDownloadException("Unable to extract videoID from URL: " + url);
    }
}

std::vector<StreamInfo> YoutubeUrlExtractor::getStreamInfo()
{
    std::vector<std::string> formatKeys{"formats", "adaptiveFormats"};
    std::vector<StreamInfo> infos;

    try{
        for(auto& key : formatKeys)
        {
            for(auto& el : _playerDataJson["streamingData"][key].items())
            {
                StreamInfo info;
                try {
                    info.quality = el.value()["quality"];
                } catch( ... ) {
                    info.quality = el.value()["qualityLabel"];
                }
                try {
                    info.url = std::string(el.value()["url"]);
                } catch( ... ) {
                    //Uses signatureCipher. Not supported yet
                    continue;
                }
                info.author = getAuthor();
                info.title  = getTitle();
                info.duration = std::chrono::milliseconds(std::stoi(std::string(el.value()["approxDurationMs"])));
                info.mimeType = el.value()["mimeType"];
                info.iTag = el.value()["itag"];
                std::string decodedVideoURL;
                if(percent_decode(decodedVideoURL, info.url.c_str()) != 0)
                {
                    throw YoutubeDownloadException("Error decoding url: " + info.url);
                }

                info.url = decodedVideoURL;
                if(std::find(_itagBlacklist.begin(), _itagBlacklist.end(), info.iTag) == _itagBlacklist.end())
                {
                    infos.push_back(info);
                }

            }
        }
    } catch ( std::exception& e ) {
        throw YoutubeDownloadException(std::string("Unable to extract stream information from youtube json: ") + e.what());
    }

    return infos;
}


StreamInfo YoutubeUrlExtractor::getVideoStream(std::vector<std::string> preferedFormats)
{
    return getStream(preferedFormats, "video");
}

StreamInfo YoutubeUrlExtractor::getAudioStream(std::vector<std::string> preferedFormats)
{
    return getStream(preferedFormats, "audio");
}

StreamInfo YoutubeUrlExtractor::getStream(std::vector<std::string> preferedFormats, std::string mimeType)
{
    auto infos = getStreamInfo();

    std::map<std::string, StreamInfo> m;
    for(auto& info: infos)
    {
        if(info.mimeType.find(mimeType) != std::string::npos)
        {
            m[info.quality] = info;
        }
    }

    for(auto& format : preferedFormats)
    {
        if(m.find(format) != m.end())
        {
            return m[format];
            break;
        }
    }

    throw YoutubeDownloadException("Unable to find requested stream format");
}

std::string YoutubeUrlExtractor::getTitle()
{
    try {
        return _playerDataJson["videoDetails"]["title"];
    } catch ( std::exception& e ) {
        throw YoutubeDownloadException(std::string("Unable to extract title from youtube json: ") + e.what());
    }
}

std::string YoutubeUrlExtractor::getAuthor()
{
    try {
        return _playerDataJson["videoDetails"]["author"];
    } catch ( std::exception& e ) {
        throw YoutubeDownloadException(std::string("Unable to extract author from youtube json: ") + e.what());
    }
}

std::string YoutubeUrlExtractor::getPlayerJsonStr(std::string id)
{
    // Circumvent age restriction to pretend access through googleapis.com
    std::string eurl = "https://youtube.googleapis.com/v/" + id;
    std::string url = "https://www.youtube.com/get_video_info?video_id=" + id + "&eurl=" + eurl;

    try {
        auto res = getHttpResponse(url);
        std::string responseStr = beast::buffers_to_string(res.body().data());
        
        std::string decodedBody;

        if(percent_decode(decodedBody, responseStr.c_str()) != 0)
        {
            throw YoutubeDownloadException("Error decoding string: " + responseStr);
        }
        
        std::string playerDataStr;

        try {
            playerDataStr = parseURI(decodedBody)["player_response"];
        } catch( ... ) {
            std::cerr << "error parsing playeresponse" << std::endl;
            return "";
        }
        return playerDataStr;
    } catch ( std::exception& e ) {
        throw YoutubeDownloadException(std::string("Unable to download and extract youtube json: ") + e.what());
    }
}

std::map<std::string, std::string>  YoutubeUrlExtractor::parseURI(std::string uri)
{
    auto string_start = uri.begin();
    auto string_end   = uri.end();

    std::map<std::string, std::string> valueMap;

    while(string_start < string_end)
    {
        auto equal_pos = std::find(string_start, string_end, '=');
        if(equal_pos >= string_end -1)
        {
            break;
        }

        std::string key = std::string(string_start, equal_pos);
        string_start = equal_pos + 1;
        auto delimiter_pos = std::find(string_start, string_end, '&');
        std::string value = std::string(string_start, delimiter_pos);

        string_start = delimiter_pos + 1;

        valueMap[key] = value;
    }

    return valueMap;
}
