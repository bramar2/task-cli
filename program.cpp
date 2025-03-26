#include "task.hpp"
#include "time.hpp"
#include "table.hpp"
#include "json.hpp"
#include "jsontest.hpp"

#include <cstdint>
#include <cstdio>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <vector>

#include <iostream>
#include <fstream>
#include <filesystem>

#include <ranges>

constexpr char HELP[] = 
R"(Task CLI --- A CLI tool to manage all your tasks | github.com/bramar2/task-cli
| task-cli add "Task description"		- Add a task and returns the Task ID.
| task-cli update [ID] "New desc"		- Update task description.
| task-cli delete [ID]				- Delete a task.
| task-cli mark [ID] [todo/in-progress/done]	- Mark task status.
| task-cli list					- Displays a list of all tasks.
| task-cli list [todo/in-progress/done]		- Displays a list of all tasks with the specified status.
| task-cli terminal				- Toggles terminal mode for quick task managing.
| task-cli exit					- Exit.
| task-cli help					- Shows this help menu.
)";

constexpr char DATA_FILE[] = "task-cli.json";

const std::unordered_map<std::string, size_t> COMMAND_ARGC {
	{"add", 2},
	{"update", 3},
	{"delete", 2},
	{"mark", 3},
	{"list", 1},
	{"terminal", 1},
	{"help", 1},
	{"exit", 1}
};
const std::unordered_set<std::string> COMMAND_ID_ARG {
	"update", "delete", "mark"
};
const std::unordered_set<std::string> COMMAND_DESC_ARG {
	"add", "update"
};
const std::unordered_set<std::string> COMMAND_STATUS_ARG {
	"mark", "list"
};

bool load_data(std::map<uint64_t, taskcli::Task>& tasks) {
	if (!std::filesystem::exists(DATA_FILE)) {
		std::ofstream(DATA_FILE) << "[]";
		return true;
	}

	std::ifstream in(DATA_FILE);
	return taskcli::json::read(in, tasks);
}

void save_data(const std::map<uint64_t, taskcli::Task>& tasks) {
	std::ofstream out(DATA_FILE);
	taskcli::json::write(out, tasks);
}

void execute_cmd(const std::vector<std::string>& arguments,
	std::map<uint64_t, taskcli::Task>& tasks, bool& terminalMode) {
	size_t argc = arguments.size();
	if (argc == 0) {
		std::cerr << "error: no command given\n";
		return;
	}
	auto it = COMMAND_ARGC.find(arguments[0]);
	if (it == COMMAND_ARGC.end()) {
		std::cerr << "error: unknown command\n";
		return;
	}

	if (argc < it->second) {
		std::cerr << "error: missing args\n";
		return;
	}
	size_t index_id = 0, index_description;
	uint64_t id;
	std::optional<taskcli::Status> status = std::nullopt;

 	if (COMMAND_ID_ARG.count(arguments[0])) {
		index_id = 1;
		try {
			id = static_cast<uint64_t>(std::stoull(arguments[index_id]));
			if (!tasks.count(id)) {
				std::cerr << "error: no task found with ID " << id << '\n';
				return;
			}
		} catch (const std::exception& ex) {
			std::cerr << "error: invalid id (" << ex.what() << ")\n";
			return;
		}
	}
	if (COMMAND_DESC_ARG.count(arguments[0])) {
		index_description = index_id + 1;
		if (!taskcli::valid_description(arguments[index_description])) {
			std::cerr << "error: invalid description\n";
			return;
		}
	}
	if (COMMAND_STATUS_ARG.count(arguments[0])) {
		if (index_id + 1 < argc) {
			status = taskcli::from_name(arguments[index_id + 1]);
		}
	}
	
	if (arguments[0] == "add") {
		if (tasks.empty()) {
			id = 1;
		} else {
			id = 1 + tasks.rbegin()->first;
		}
		time_t currTime = taskcli::current_time();
		tasks.emplace(id,
			taskcli::Task(id, arguments[index_description], taskcli::Status::TODO, currTime, currTime)
		);
		std::cout << "Added new task (ID " << id << ") with description: " << arguments[index_description] << '\n';
	} else if (arguments[0] == "update") {
		tasks.at(id).update_description(arguments[index_description]);
		std::cout << "Updated task description of ID " << id << " to: " << arguments[2] << '\n';
	} else if (arguments[0] == "delete") {
		tasks.erase(id);
		std::cout << "Deleted task with ID " << id << '\n';
	} else if (arguments[0] == "mark") {
		taskcli::Task& task = tasks.at(id);
		if (status.has_value()) {
			if (task.status == status.value()) {
				std::cout << "Task status of ID " << id << " is already " << taskcli::to_name(task.status) << '\n';
			} else {
				task.update_status(status.value());
				std::cout << "Updated task status of ID " << id << " to: " << taskcli::to_name(task.status) << '\n';
			}
		} else {
			std::cerr << "error: invalid status\n";
		}
	} else if (arguments[0] == "list") {
		taskcli::table::print_div(std::cout,
			std::begin(taskcli::table::LIST_FIELDS),
			std::end(taskcli::table::LIST_FIELDS));
		std::cout << '\n';
		taskcli::table::print_row_copy(std::cout,
			std::begin(taskcli::table::LIST_FIELDS),
			std::end(taskcli::table::LIST_FIELDS),
			std::begin(taskcli::table::LIST_HEAD),
			std::end(taskcli::table::LIST_HEAD));
		std::cout << '\n';

		bool empty = true;
		std::string_view data[ std::size(taskcli::table::LIST_FIELDS) ];
		for (const taskcli::Task& task : tasks | std::views::values) {
			if (status.has_value() && task.status != status.value()) continue;

			empty = false;
			std::string id_str = std::to_string(task.id);
			std::string created_str = taskcli::formatted_time(task.createdAt);
			std::string updated_str = taskcli::formatted_time(task.updatedAt);
			data[0] = id_str;
			data[1] = task.description;
			data[2] = taskcli::to_name(task.status);
			data[3] = created_str;
			data[4] = updated_str;

			taskcli::table::print_row(std::cout,
				std::begin(taskcli::table::LIST_FIELDS),
				std::end(taskcli::table::LIST_FIELDS),
				data);
			std::cout << '\n';
		}

		if (empty) {
			std::fill(std::begin(data), std::end(data), std::string_view {});
			taskcli::table::print_row(std::cout,
				std::begin(taskcli::table::LIST_FIELDS),
				std::end(taskcli::table::LIST_FIELDS),
				data);
			std::cout << '\n';
		}
		
	} else if (arguments[0] == "terminal") {
		terminalMode ^= 1;
		std::cout << "Terminal mode is now " << (terminalMode ? "ON" : "OFF") << '\n';
	} else if (arguments[0] == "help") {
		std::cout << '\n' << HELP << '\n';
	} else if (arguments[0] == "exit") {
		terminalMode = false;
		std::cout << "Exit command received.\n";
	} else {
		std::cerr << "error: unknown command\n";
	}
	return;
}

void parse_input(std::vector<std::string>& arguments, const std::string& input) {
	arguments.push_back("");

	bool inQuote = false;
	for (char ch : input) {
		if (ch == '"') {
			if (!arguments.back().empty() && arguments.back().back() == '\\') {
				arguments.back().back() = '"';
			} else {
				inQuote ^= 1;
			}
		} else if (ch == ' ' && !inQuote) {
			arguments.push_back("");
		} else {
			arguments.back().push_back(ch);
		}
	}
	if (arguments.back().empty()) arguments.pop_back();
}

int main(int argv, char **argc) {
	if (argv <= 1) {
		std::cout << HELP;
		return 0;
	}

	std::vector<std::string> arguments;

	for (int i = 1; i < argv; i++) {
		arguments.emplace_back(argc[i]);
	}


	bool terminalMode = false;
	std::map<uint64_t, taskcli::Task> tasks;
	bool result = load_data(tasks);

	if (!result) {
		std::cerr << "error: failed to load data, possibly invalid json.\n";
		return 1;
	}
	
	std::string input;
	while (true) {
		execute_cmd(arguments, tasks, terminalMode);
		save_data(tasks);

		if (not terminalMode) break;
		std::cout << "task-cli > ";
		std::getline(std::cin, input);
		arguments.clear();
		parse_input(arguments, input);
	}
	return 0;
}