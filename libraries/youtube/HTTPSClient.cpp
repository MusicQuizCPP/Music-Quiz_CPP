#include "HTTPSClient.hpp"

#include "YoutubeDownloadException.hpp"

#include <openssl/ssl.h>
#include <iostream>
#include <thread>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/error.hpp>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using namespace youtube;


HTTPSClient::HTTPSClient(std::string host) :
    _ctx(boost::asio::ssl::context::tlsv12_client), 
    _resolver(_ioc), 
    _stream(_ioc, _ctx),
    _host(host)
{
    _ctx.set_verify_mode(boost::asio::ssl::verify_none);

    // Set SNI Hostname (many hosts need this to handshake successfully)
    #pragma GCC diagnostic ignored "-Wold-style-cast" 
    #pragma GCC diagnostic ignored "-Wcast-qual"
    if(! SSL_set_tlsext_host_name(_stream.native_handle(), _host.c_str()))
    #pragma GCC diagnostic pop
    #pragma GCC diagnostic pop
    {
        throw YoutubeDownloadException("Encountered error while setting SSL_set_tlsext_host_name");
    }

    try {
        // Look up the domain name
        auto const results = _resolver.resolve(_host, "443");

        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(_stream).connect(results);

        // Perform the SSL handshake
        _stream.handshake(ssl::stream_base::client);
    } catch ( std::exception& e ) {
        throw YoutubeDownloadException("Unable to connect to host: " + host + " " + e.what());
    }
}

http::response<http::dynamic_body> HTTPSClient::getHttpResponse(std::string url)
{ 
    try {
        sendHttpRequest(url);
        // This buffer is used for reading and must be persisted
        _readBuffer.clear();

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(_stream, _readBuffer, res);
        return res;
    } catch ( std::exception& e ) {
        throw YoutubeDownloadException("Unable to read HTTP response from " + url + " " + e.what());
    }
} 

void HTTPSClient::sendHttpRequest(std::string url)
{
    try
    {
        // Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::get, url, 11};
        req.set(http::field::host, _host);
        req.set(http::field::user_agent, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.106 Safari/537.36");

        // Send the HTTP request to the remote host
        http::write(_stream, req);
    } catch ( std::exception& e ) {
        throw YoutubeDownloadException("Unable to send HTTP request to " + url + " " + e.what());
    }
}

void HTTPSClient::startHttpStream(std::string url)
{

    sendHttpRequest(url);

    _readBuffer.clear();
    // Declare a container to hold the response
    _resp_parser.body_limit((std::numeric_limits<std::uint64_t>::max)());

    boost::system::error_code ec;
    
    http::read_header(_stream, _readBuffer, _resp_parser, ec);
    if(ec)
    {
        throw YoutubeDownloadException("Unable to start http stream from " + url + " " + ec.message());
    }
}

int HTTPSClient::getHttpStreamChunck(void* buf, int size)
{
    boost::system::error_code ec;

    _resp_parser.get().body().data = buf;
    _resp_parser.get().body().size = size;
    size_t s = http::read_some(_stream, _readBuffer, _resp_parser, ec);
    if(ec == http::error::need_buffer) {
        ec.assign(0, ec.category());
    }
    if(ec)
    {
        throw YoutubeDownloadException(std::string("Encountered error while reading from stream: ") + ec.message());
    }
    return static_cast<int>(s);
}

std::string HTTPSClient::getHostFromUrl(std::string url)
{
    auto host_start = url.find("://") + 3;
    auto host_end = url.find("/", host_start);
    if(host_start == std::string::npos || host_end == std::string::npos)
    {
        throw YoutubeDownloadException("Unable to extract host from " + url);
    }

    return url.substr(host_start, host_end - host_start);
}