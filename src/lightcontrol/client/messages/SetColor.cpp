#include "SetColor.hpp"

#include <vector>

#include "common/Log.hpp"
#include <nlohmann/json.hpp>


std::shared_ptr<std::string> LightControl::SetColor::compose() const
{
	std::vector<nlohmann::json> segments;
	std::vector<std::vector<uint8_t>> colors;
	colors.push_back(std::vector<uint8_t>({_r, _g, _b}));
	colors.push_back(std::vector<uint8_t>({_r, _g, _b}));
	colors.push_back(std::vector<uint8_t>({_r, _g, _b}));
	segments.push_back({
		{"col", colors},
	});

	nlohmann::json data;
	data["seg"] = segments;

	return std::make_shared<std::string>(data.dump());
}