#include "SetColor.hpp"

#include "common/Log.hpp"
#include <nlohmann/json.hpp>

using namespace LightControl;
using json = nlohmann::json;

std::shared_ptr<std::string> SetColor::compose() const
{
	std::vector<json> segments;
	std::vector<std::vector<uint8_t>> colors;
	colors.push_back(std::vector<uint8_t>({_r, _g, _b}));
	colors.push_back(std::vector<uint8_t>({_r, _g, _b}));
	colors.push_back(std::vector<uint8_t>({_r, _g, _b}));
	segments.push_back({
		{"col", colors},
	});
	json data;
	data["seg"] = segments;

	return std::make_shared<std::string>(data.dump());
}