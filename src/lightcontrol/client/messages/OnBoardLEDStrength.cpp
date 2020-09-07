#include "OnBoardLEDStrength.hpp"

#include <cereal/archives/binary.hpp>

#include "common/Log.hpp"

using namespace LightControl;

OnBoardLEDStrength::OnBoardLEDStrength(boost::asio::mutable_buffer& buffer)
{
    if (buffer.size() < sizeof(float)) {
        LOG_ERROR("Received Onboard LED message with invalid size: " + std::to_string(buffer.size()));
    }

    _strength = *static_cast<uint8_t*>(buffer.data());
}

std::shared_ptr<std::ostringstream> OnBoardLEDStrength::compose() const
{
    std::shared_ptr<std::ostringstream> buf = std::make_shared<std::ostringstream>();
    cereal::BinaryOutputArchive         archive(*buf);

    archive(static_cast<uint8_t>(PackageType::ONBOARD_LED_STRENGTH), _strength);

    return buf;
}