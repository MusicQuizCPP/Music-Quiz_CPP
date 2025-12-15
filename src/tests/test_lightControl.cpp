#include "lightcontrol/discover/LightControlDiscover.hpp"
#include "lightcontrol/client/LightControlClient.hpp"
#include "lightcontrol/client/messages/LightModeMessage.hpp"
#include "lightcontrol/client/messages/PulseMessage.hpp"
#include "lightcontrol/client/messages/StrobeMessage.hpp"
#include "lightcontrol/client/messages/GlitterMessage.hpp"

#include <iostream>
#include <chrono>
#include <thread>

using namespace LightControl;


int main()
{
	LightControlDiscover discover;
	while ( discover.getDevices().size() == 0 ) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::cout << "Waiting for devices" << std::endl;
	}

	auto devices = discover.getDevices();
	std::string device = devices.begin()->second;
	std::cout << "Found " << device << std::endl;

	auto client = std::make_shared<LightControl::LightControlClient>(device, 80);
	client->start();
	while ( !client->isConnected() ) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	client->sendMessage(LightModeMessage(LightMode::OFF, 1.f, 0, 10, 0));
	client->sendMessage(LightModeMessage(LightMode::PULSE, 1.f, 0, 10, 0));
	client->sendMessage(GlitterMessage(std::chrono::milliseconds(50), true, 10));
	client->sendMessage(PulseMessage(std::chrono::milliseconds(5000), PulseMessage::PulseDirection::PULSE_IN, true));

	// while(true)
	// {
	//     client->sendMessage(LightModeMessage(LIGHT_MODE_ON, 0.99f, 0, 255, 0));
	//     std::this_thread::sleep_for(std::chrono::milliseconds(10));
	// }
}