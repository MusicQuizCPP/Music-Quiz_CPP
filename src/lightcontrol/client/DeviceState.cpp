#include "DeviceState.hpp"

#include "common/Log.hpp"

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>


void LightControl::DeviceState::parseMessage(boost::asio::mutable_buffer& buffer)
{
	std::string s(static_cast<char*>(buffer.data()), buffer.size());
	nlohmann::json data = nlohmann::json::parse(s);
	
	/** Info */
	if(data.contains("info")) {
		if(data["info"].contains("name")) {
			name = data["info"]["name"].get<std::string>();
		} else {
			LOG_INFO(data["info"]);
		}
	}	
}