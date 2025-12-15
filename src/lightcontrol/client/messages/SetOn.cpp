#include "SetOn.hpp"

#include "common/Log.hpp"
#include <nlohmann/json.hpp>


std::shared_ptr<std::string> LightControl::SetOn::compose() const
{
	nlohmann::json data = {
		{"on"  , _on},
		{"bri" , _brightness},
	};

	return std::make_shared<std::string>(data.dump());
}