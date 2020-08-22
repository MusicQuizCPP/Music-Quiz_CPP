#include "RainbowMessage.hpp"

#include <cereal/archives/binary.hpp>

#include "common/Log.hpp"


using namespace LightControl;

RainbowMessage::RainbowMessage(boost::asio::mutable_buffer& buffer)
{
    if ( buffer.size() < sizeof(uint32_t) ) {
        LOG_ERROR("Received Rainbow message with invalid size: " + std::to_string(buffer.size()));
    }

    _updateRate = std::chrono::milliseconds(*static_cast<uint32_t*>(buffer.data()));
}

std::shared_ptr<std::ostringstream> RainbowMessage::compose() const
{
    std::shared_ptr<std::ostringstream> buf = std::make_shared<std::ostringstream>();
    cereal::BinaryOutputArchive archive(*buf);

    archive(static_cast<uint8_t>(PackageType::RAINBOW),
        static_cast<uint32_t>(_updateRate.count()));

    return buf;
}