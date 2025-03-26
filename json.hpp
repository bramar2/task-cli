#pragma once
#include "task.hpp"

#include <iostream>
#include <map>

namespace taskcli::json {

	constexpr int TYPE_UNSIGNED = 0;
	constexpr int TYPE_SIGNED = 1;
	constexpr int TYPE_STR = 2;
	inline const std::map<std::string, int> KEY_TYPE {
		{"id", TYPE_UNSIGNED},
		{"description", TYPE_STR},
		{"status", TYPE_STR},
		{"created", TYPE_SIGNED},
		{"updated", TYPE_SIGNED}
	};

	bool read(std::istream& in, std::map<uint64_t, taskcli::Task>& tasks);
	void write(std::ostream& out, const std::map<uint64_t, taskcli::Task>& tasks);
}