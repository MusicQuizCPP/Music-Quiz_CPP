#include "SetEffect.hpp"

#include <vector>

#include "common/Log.hpp"
#include <nlohmann/json.hpp>


std::shared_ptr<std::string> LightControl::SetEffect::compose() const
{
	std::vector<nlohmann::json> segments;
	segments.push_back({
		{"fx", _effect},
		{"sx", _speed},
		{"ix", _intensity},
	});
	nlohmann::json data;
	data["seg"] = segments;

	return std::make_shared<std::string>(data.dump());
}