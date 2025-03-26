#include "time.hpp"
#include <chrono>

namespace taskcli {
	time_t current_time() {
		return static_cast<time_t>(
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
			.count());
	}
	std::string formatted_time(time_t epochTime) {
		std::chrono::system_clock::time_point tp { std::chrono::milliseconds { epochTime }};
		return std::format("{0:%F}T{0:%R%z}", tp);
	}
}