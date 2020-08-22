#include "LightModeMessage.hpp"

#include <cereal/archives/binary.hpp>

#include "common/Log.hpp"

using namespace LightControl;

LightModeMessage::LightModeMessage(boost::asio::mutable_buffer& buffer)
{
    if ( buffer.size() < 8 ) {
        LOG_ERROR("Received Light Mode Message with invalid size: " + std::to_string(buffer.size()));
    }

    _mode = static_cast<LightMode>(*static_cast<uint8_t*>(buffer.data()));
    buffer += 1;

    _strength = *static_cast<float*>(buffer.data());
    buffer += sizeof(_strength);

    _r = *static_cast<uint8_t*>(buffer.data());
    buffer += sizeof(_r);

    _g = *static_cast<uint8_t*>(buffer.data());
    buffer += sizeof(_g);

    _b = *static_cast<uint8_t*>(buffer.data());
    buffer += sizeof(_b);
}

std::shared_ptr<std::ostringstream> LightModeMessage::compose() const
{
    std::shared_ptr<std::ostringstream> buf = std::make_shared<std::ostringstream>();
    cereal::BinaryOutputArchive archive(*buf);

    archive(static_cast<uint8_t>(PackageType::LIGHT_MODE), static_cast<uint8_t>(_mode), _strength, _r, _g, _b);

    return buf;
}