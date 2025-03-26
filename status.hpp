#pragma once
#include <optional>
#include <string_view>

namespace taskcli {
	inline constexpr std::string_view STATUS_NAME_TODO = "todo";
	inline constexpr std::string_view STATUS_NAME_IN_PROGRESS = "in-progress";
	inline constexpr std::string_view STATUS_NAME_DONE = "done";
	enum class Status { TODO, IN_PROGRESS, DONE };
	
	std::optional<Status> from_name(std::string_view s);
	std::string_view to_name(Status s);
}