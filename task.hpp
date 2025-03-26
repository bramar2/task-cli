#pragma once
#include "status.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <ostream>

namespace taskcli {
	bool valid_description(std::string_view description);

	class Task {
	public:
		uint64_t id;
		std::string description;
		Status status;
		time_t createdAt;
		time_t updatedAt;

		Task(uint64_t c_id, std::string_view c_description, Status c_status, time_t c_createdAt, time_t c_updatedAt);
		Task(const Task& other);
		Task(Task&& other) noexcept;
		void update_description(const std::string& new_description);
		void update_status(Status new_status);
		void serialize(std::ostream& stream) const;
		static std::optional<Task> deserialize(const std::string& data);

		Task& operator=(Task other);
	};
}