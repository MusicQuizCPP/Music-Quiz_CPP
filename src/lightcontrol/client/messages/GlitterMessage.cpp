#include "GlitterMessage.hpp"

#include <cereal/archives/binary.hpp>

#include "common/Log.hpp"

using namespace LightControl;

GlitterMessage::GlitterMessage(boost::asio::mutable_buffer& buffer)
{
    if (buffer.size() < 9) {
        LOG_ERROR("Received Glitter Message with invalid size: " + std::to_string(buffer.size()));
    }

    _updateRate = std::chrono::microseconds(*static_cast<uint32_t*>(buffer.data()));
    buffer += sizeof(uint32_t);

    _randomColor = *static_cast<uint8_t*>(buffer.data());
    buffer += sizeof(uint8_t);

    _percentOn = *static_cast<float*>(buffer.data());
    buffer += sizeof(_percentOn);
}

std::shared_ptr<std::ostringstream> GlitterMessage::compose() const
{
    std::shared_ptr<std::ostringstream> buf = std::make_shared<std::ostringstream>();
    cereal::BinaryOutputArchive         archive(*buf);

    archive(static_cast<uint8_t>(PackageType::GLITTER), static_cast<uint32_t>(_updateRate.count()), static_cast<uint8_t>(_randomColor), _percentOn);

    return buf;
}