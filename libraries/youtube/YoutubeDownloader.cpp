#include "YoutubeDownloader.hpp"

#include "HTTPSClient.hpp"
#include "YoutubeDownloadException.hpp"
#include "YoutubeUrlExtractor.hpp"

using namespace youtube;

YoutubeDownloader::YoutubeDownloader(std::string url, std::vector<std::string> preferedFormats) : _url(url), _preferedFormats(preferedFormats)
{
    _urlExtractor   = new YoutubeUrlExtractor(_url);
    _streamInfo     = _urlExtractor->getVideoStream(_preferedFormats);
    _downloadClient = new HTTPSClient(_urlExtractor->getHostFromUrl(_streamInfo.url));
    _downloadClient->startHttpStream(_streamInfo.url);
}

int YoutubeDownloader::getChunk(void* buf, int size)
{
    int tmp = _downloadClient->getHttpStreamChunck(buf, size);
    _downloadedBytes += tmp;
    if (tmp == 0 && _downloadedBytes == 0) 
    {
        if (_retryCount < _maxRetries) {
            tmp = retry(buf, size);
        }
        else {
            throw YoutubeDownloadException("Read zero bytes from stream");
        }
    }
    return tmp;
}

int YoutubeDownloader::retry(void* buf, int size)
{
    delete _urlExtractor;
    delete _downloadClient;
    _urlExtractor   = new YoutubeUrlExtractor(_url);
    _streamInfo     = _urlExtractor->getVideoStream(_preferedFormats);
    _downloadClient = new HTTPSClient(_urlExtractor->getHostFromUrl(_streamInfo.url));
    _downloadClient->startHttpStream(_streamInfo.url);
    _downloadedBytes = 0;
    _retryCount++;
    return getChunk(buf, size);
}

size_t YoutubeDownloader::getDownloadedBytes() { return _downloadedBytes; }

YoutubeDownloader::~YoutubeDownloader()
{
    delete _urlExtractor;
    delete _downloadClient;
}