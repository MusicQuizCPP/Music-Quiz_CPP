#include "youtube/YoutubeDownloader.hpp"
#include <exception>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Not enough arguments" << std::endl;
        return -1;
    }
    try {
        youtube::YoutubeDownloader downloader(argv[1]);
        std::cout << "Video:\n" << downloader.getStreamInfo() << std::endl;

        char          buf[1024];
        int           bytes_received       = 0;
        size_t        last_progress_report = 0;
        std::ofstream outfile(argv[2], std::ios::binary);
        std::cout << "Download start" << std::endl;
        do {
            bytes_received = downloader.getChunk(buf, sizeof(buf));
            outfile.write(buf, bytes_received);
            if (downloader.getDownloadedBytes() > last_progress_report + 1000000) {
                last_progress_report = downloader.getDownloadedBytes();
                std::cout << "Downloaded " << downloader.getDownloadedBytes() << " bytes" << std::endl;
            }
        } while (bytes_received > 0);
        outfile.close();
    }
    catch (std::exception& e) {
        std::cout << "Encountered error while downloading video: " << e.what() << std::endl;
    }
}