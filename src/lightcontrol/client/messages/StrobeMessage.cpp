#include "StrobeMessage.hpp"

#include <cereal/archives/binary.hpp>

#include "common/Log.hpp"


using namespace LightControl;

StrobeMessage::StrobeMessage(boost::asio::mutable_buffer& buffer)
{
    if ( buffer.size() < 8 ) {
        LOG_ERROR("Received Strobe Message with invalid size: " + std::to_string(buffer.size()));
    }

    _onTime = std::chrono::microseconds(*static_cast<uint32_t*>(buffer.data()));
    buffer += sizeof(uint32_t);

    _offTime = std::chrono::microseconds(*static_cast<uint32_t*>(buffer.data()));
    buffer += sizeof(uint32_t);
}

std::shared_ptr<std::ostringstream> StrobeMessage::compose() const
{
    std::shared_ptr<std::ostringstream> buf = std::make_shared<std::ostringstream>();
    cereal::BinaryOutputArchive archive(*buf);

    archive(static_cast<uint8_t>(PackageType::STROBE),
        static_cast<uint32_t>(_onTime.count()),
        static_cast<uint32_t>(_offTime.count()));

    return buf;
}