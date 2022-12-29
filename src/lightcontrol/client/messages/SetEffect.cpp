#include "SetEffect.hpp"

#include "common/Log.hpp"
#include <nlohmann/json.hpp>

using namespace LightControl;
using json = nlohmann::json;

std::shared_ptr<std::string> SetEffect::compose() const
{
	std::vector<json> segments;
	segments.push_back({
		{"fx", _effect},
		{"sx", _speed},
		{"ix", _intensity},
	});
	json data;
	data["seg"] = segments;

	return std::make_shared<std::string>(data.dump());
}