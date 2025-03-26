#include "task.hpp"
#include "time.hpp"

namespace taskcli {
	bool valid_description(std::string_view description) {
		if (description.size() > 200ull) return false;
		bool has_visible = false;
		for (char ch : description) {
			if (!isprint(static_cast<unsigned char>(ch))) return false;
			has_visible |= (ch != ' ');
		}
		return has_visible;
	}
	Task::Task(uint64_t c_id, std::string_view c_description, Status c_status, time_t c_createdAt, time_t c_updatedAt) :
			id(c_id),
			description(c_description),
			status(c_status),
			createdAt(c_createdAt),
			updatedAt(c_updatedAt) {}

	Task::Task(const Task& other) :
		id(other.id),
		description(other.description),
		status(other.status),
		createdAt(other.createdAt),
		updatedAt(other.updatedAt) {}

	Task::Task(Task&& other) noexcept :
		id(other.id),
		description(std::move(other.description)),
		status(other.status),
		createdAt(other.createdAt),
		updatedAt(other.updatedAt) {}

	void Task::update_description(const std::string &new_description) {
		this->description = new_description;
		this->updatedAt = current_time();
	}

	void Task::update_status(Status new_status) {
		this->status = new_status;
		this->updatedAt = current_time();
	}

	Task& Task::operator=(Task other) {
		std::swap(this->id, other.id);
		std::swap(this->description, other.description);
		std::swap(this->status, other.status);
		std::swap(this->createdAt, other.createdAt);
		std::swap(this->updatedAt, other.updatedAt);
		return *this;
	}
	bool Task::operator==(const Task& other) const {
		return this->id == other.id && this->status == other.status && this->description == other.description && this->createdAt == other.createdAt && this->updatedAt == other.updatedAt;
	}
}