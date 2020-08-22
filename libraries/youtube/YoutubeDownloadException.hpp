#include <exception>
#include <string>

class YoutubeDownloadException : public std::exception {
public:
    /**
     * @brief Constructor.
     * @param[in] msg message about why the exception occured
     */

    YoutubeDownloadException(std::string msg) : _msg("Download Error: " + msg) {}

    /**
     * @brief Get exception message
     *
     * @return C string with information about the exception
     */
    virtual const char* what() const throw() { return _msg.c_str(); }

private:
    std::string _msg;
};