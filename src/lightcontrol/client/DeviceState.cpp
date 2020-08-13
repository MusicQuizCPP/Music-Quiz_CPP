#include "DeviceState.hpp"

#include "messages/PackageTypes.hpp"

#include "common/Log.hpp"


using namespace LightControl;

void DeviceState::parseMessage(boost::asio::mutable_buffer& buffer)
{
	PackageType packageType = static_cast<PackageType>(*static_cast<uint8_t*>(buffer.data()));
	buffer += sizeof(uint8_t);

	switch ( packageType ) {
	case PackageType::NODE_INFO:
		this->nodeInfo = NodeInfoMessage(buffer);
		break;

	case PackageType::LIGHT_MODE:
		this->lightMode = LightModeMessage(buffer);
		break;

	case PackageType::STROBE:
		this->strobe = StrobeMessage(buffer);
		break;

	case PackageType::GLITTER:
		this->glitter = GlitterMessage(buffer);
		break;

	case PackageType::PULSE_SETTINGS:
		this->pulse = PulseMessage(buffer);
		break;

	case PackageType::ONBOARD_LED_STRENGTH:
		this->onboardLed = OnBoardLEDStrength(buffer);
		break;

	case PackageType::RAINBOW:
		this->rainbow = RainbowMessage(buffer);
		break;

	case PackageType::RUNNING_SECTIONS:
		this->runningSections = RunningSectionsMessage(buffer);
		break;

	case PackageType::FREQUENCY_INFO:
	case PackageType::SAMPLES:
	case PackageType::TETRIS_MOVE:
	case PackageType::WIFI_SETTINGS:
	case PackageType::SCAN_WIFI:
	case PackageType::OTA_DATA:
	case PackageType::PING:
	case PackageType::GAME_OF_LIFE_SETTINGS:
	case PackageType::FREQUENCY_SETTINGS:
		break;
	default:
		LOG_DEBUG("Received unhandled package with type: " + std::to_string(packageType));
		break;
	}
}