#include "SetOn.hpp"

#include "common/Log.hpp"
#include <nlohmann/json.hpp>

using namespace LightControl;
using json = nlohmann::json;

std::shared_ptr<std::string> SetOn::compose() const
{
	json data = {
		{"on"  , _on},
		{"bri" , _brightness},
	};
	return std::make_shared<std::string>(data.dump());
}