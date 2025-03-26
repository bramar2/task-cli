#include "status.hpp"
#include <utility>

namespace taskcli {
	std::optional<Status> from_name(std::string_view s) {
		if(s == STATUS_NAME_TODO) return taskcli::Status::TODO;
		else if(s == STATUS_NAME_IN_PROGRESS) return taskcli::Status::IN_PROGRESS;
		else if(s == STATUS_NAME_DONE) return taskcli::Status::DONE;
		else return std::nullopt;
	}
	std::string_view to_name(Status s) {
		switch(s) {
		case taskcli::Status::TODO:
			return STATUS_NAME_TODO;
		case taskcli::Status::IN_PROGRESS:
			return STATUS_NAME_IN_PROGRESS;
		case taskcli::Status::DONE:
			return STATUS_NAME_DONE;
		}
		std::unreachable();
	}
}