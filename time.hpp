#pragma once
#include <cstdint>
#include <string>

namespace taskcli {
	time_t current_time();
	std::string formatted_time(time_t epochTime);
};