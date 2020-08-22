#include "RunningSectionsMessage.hpp"

#include <cereal/archives/binary.hpp>

#include "common/Log.hpp"


using namespace LightControl;

RunningSectionsMessage::RunningSectionsMessage(boost::asio::mutable_buffer& buffer)
{
    if ( buffer.size() < 12 ) {
        LOG_ERROR("Received Running Sections message with invalid size: " + std::to_string(buffer.size()));
    }

    _updateRate = std::chrono::milliseconds(*static_cast<uint32_t*>(buffer.data()));
    buffer += sizeof(uint32_t);

    _sectionCount = *static_cast<uint32_t*>(buffer.data());
    buffer += sizeof(uint32_t);

    _sectionSize = *static_cast<uint32_t*>(buffer.data());
    buffer += sizeof(uint32_t);
}

std::shared_ptr<std::ostringstream> RunningSectionsMessage::compose() const
{
    std::shared_ptr<std::ostringstream> buf = std::make_shared<std::ostringstream>();
    cereal::BinaryOutputArchive archive(*buf);

    archive(static_cast<uint8_t>(PackageType::RUNNING_SECTIONS),
        static_cast<uint32_t>(_updateRate.count()),
        _sectionCount,
        _sectionSize);

    return buf;
}