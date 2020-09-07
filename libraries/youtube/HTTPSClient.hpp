#pragma once

#include <cstdint>
#include <string>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

namespace youtube {
class HTTPSClient {
public:
    /**
     * @brief Constructor. Conects to supplied host and performs SSL handshake
     * @param[in] host host to connect to.
     *
     */
    HTTPSClient(std::string host);

    /**
     * @brief Sends a http GET request for the supplied url.
     *        The url must be for the same host as supplied in the constructor
     *
     * @param[in] url url to request.
     */
    void sendHttpRequest(std::string url);

    /**
     * @brief Send a HTTP get request to the given url, and returns the response body
     *
     * @param[in] url url to request. Must be from the same host as supplied in constructor
     */

    boost::beast::http::response<boost::beast::http::dynamic_body> getHttpResponse(std::string url);

    /**
     * @brief Sends a http GET request for the supplied url. and reads the response header,
     *        leaving the response body unread. The body must later be read with subsequent calls
     *        to getHttpStreamChunk
     *        The url must be for the same host as supplied in the constructor
     *
     * @param[in] url url to request.
     */
    void startHttpStream(std::string url);

    /**
     * @brief Read the next chunk of data from the current http request
     *
     * @param[out] buf Buffer the data should be copied to
     * @param[in] size size of the buffer
     *
     * @return the amount of data read. returns 0 when stream ends.
     */
    int getHttpStreamChunck(void* buf, int size);

    /**
     * @brief Get the host part of an url
     *
     * @param[in] url url to parse. Must be of format http(s)://<host>/<something>
     * @return hostname
     */
    static std::string getHostFromUrl(std::string url);

private:
    boost::asio::io_context                                              _ioc;
    boost::asio::ssl::context                                            _ctx;
    boost::asio::ip::tcp::resolver                                       _resolver;
    boost::beast::ssl_stream<boost::beast::tcp_stream>                   _stream;
    boost::beast::flat_buffer                                            _readBuffer;
    boost::beast::http::response_parser<boost::beast::http::buffer_body> _resp_parser;
    std::string                                                          _host;
};
} // namespace youtube