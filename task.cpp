#include "task.hpp"
#include "time.hpp"
#include <chrono>

std::optional<std::string_view> next_token(const std::string& data, size_t& start) {
	size_t next = data.find(' ', start);
	if (start == next || next == std::string::npos) return std::nullopt;
	std::string_view res = std::string_view(data).substr(start, next - start);
	start = next + 1;
	return res;
}

std::optional<uint64_t> next_uint64(const std::string& data, size_t& start) {
	std::optional<std::string_view> token = next_token(data, start);
	if (!token.has_value()) return std::nullopt;
	uint64_t x;
#if __cpp_lib_to_chars >= 202306L
    if (std::from_chars(token->data(), token->data() + token->size(), x))
#else
    if (std::from_chars(token->data(), token->data() + token->size(), x).ec == std::errc{})
#endif
    	return x;
    else
    	return std::nullopt;
}
std::optional<time_t> next_time(const std::string& data, size_t& start) {
	std::optional<std::string_view> token = next_token(data, start);
	if (!token.has_value()) return std::nullopt;
	time_t x;
#if __cpp_lib_to_chars >= 202306L
    if (std::from_chars(token->data(), token->data() + token->size(), x))
#else
    if (std::from_chars(token->data(), token->data() + token->size(), x).ec == std::errc{})
#endif
    	return x;
    else
    	return std::nullopt;
}

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

	void Task::serialize(std::ostream& stream) const {
		stream << id << " " << to_name(status) << " " << createdAt << " " << updatedAt << " " << description;
	}

	Task& Task::operator=(Task other) {
		std::swap(this->id, other.id);
		std::swap(this->description, other.description);
		std::swap(this->status, other.status);
		std::swap(this->createdAt, other.createdAt);
		std::swap(this->updatedAt, other.updatedAt);
		return *this;
	}




	std::optional<Task> Task::deserialize(const std::string &data) {
		size_t start = 0;

		
		std::optional<uint64_t> id = next_uint64(data, start);
		if (!id.has_value()) return std::nullopt;

		std::optional<std::string_view> statusName = next_token(data, start);
		if (!statusName.has_value()) return std::nullopt;
		std::optional<taskcli::Status> status = taskcli::from_name(statusName.value());
		if (!status.has_value()) return std::nullopt;


		std::optional<time_t> createdAt = next_time(data, start);
		if (!createdAt.has_value()) return std::nullopt;

		std::optional<time_t> updatedAt = next_time(data, start);
		if (!updatedAt.has_value()) return std::nullopt;
		if (start == std::string::npos) return std::nullopt;

		std::string_view description(data.c_str() + start);
		if (!valid_description(description)) return std::nullopt;

		return std::make_optional<Task>(
			id.value(),
			description,
			status.value(),
			createdAt.value(),
			updatedAt.value());
	}
}