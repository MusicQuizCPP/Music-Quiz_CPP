#include "PulseMessage.hpp"

#include <cereal/archives/binary.hpp>

#include "common/Log.hpp"

using namespace LightControl;

PulseMessage::PulseMessage(boost::asio::mutable_buffer& buffer)
{
    if (buffer.size() < 6) {
        LOG_ERROR("Received Pulse Message with invalid size: " + std::to_string(buffer.size()));
    }

    _pulseTime = std::chrono::milliseconds(*static_cast<uint32_t*>(buffer.data()));
    buffer += sizeof(uint32_t);

    _direction = static_cast<PulseDirection>(*static_cast<uint8_t*>(buffer.data()));
    buffer += sizeof(uint8_t);

    _singleShot = *static_cast<uint8_t*>(buffer.data());
    buffer += sizeof(uint8_t);
}

std::shared_ptr<std::ostringstream> PulseMessage::compose() const
{
    std::shared_ptr<std::ostringstream> buf = std::make_shared<std::ostringstream>();
    cereal::BinaryOutputArchive         archive(*buf);

    archive(static_cast<uint8_t>(PackageType::PULSE_SETTINGS), static_cast<uint32_t>(_pulseTime.count()), static_cast<int8_t>(_direction), static_cast<uint8_t>(_singleShot));

    return buf;
}