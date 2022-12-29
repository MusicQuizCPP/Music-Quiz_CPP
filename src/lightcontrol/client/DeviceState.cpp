#include "DeviceState.hpp"

#include "common/Log.hpp"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

using namespace LightControl;
using namespace boost::asio;
using json = nlohmann::json;

void DeviceState::parseMessage(boost::asio::mutable_buffer& buffer)
{
	std::string s(static_cast<char*>(buffer.data()), buffer.size());
	json data = json::parse(s);
	
	// info
	if(data.contains("info")) {
		if(data["info"].contains("name")) {
			name = data["info"]["name"].get<std::string>();
		} else {
			LOG_INFO(data["info"]);
		}
	}	
}