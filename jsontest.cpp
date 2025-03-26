#include "jsontest.hpp"
#include "json.hpp"
#include "time.hpp"
#include <iostream>
#include <fstream>


namespace jsontest {
	void main() {
		// only test visible characters
		// (includes backslashes, double quotes)
		// (but doesnt include chars like new line)
		std::string test_file;
		std::getline(std::cin, test_file);

		taskcli::Task taskOne(1, "Task 1.\\0", taskcli::Status::TODO, 1093409, (time_t)1e9);
		taskcli::Task taskTwo(2, "2.0 Task\\\\", taskcli::Status::IN_PROGRESS, -1093409, -10901);
		taskcli::Task taskThree(3, "Ta v\\\"3.0 sk\"\\", taskcli::Status::DONE, -1093409, 10901);

		std::map<uint64_t, taskcli::Task> test_map {
			{1, std::move(taskOne)},
			{2, std::move(taskTwo)},
			{3, std::move(taskThree)}
		};
		std::map<uint64_t, taskcli::Task> result_map {};

		{
			std::ofstream out(test_file);
			taskcli::json::write(out, test_map);
		}

		{
			std::ifstream in(test_file);
			bool result = taskcli::json::read(in, result_map);
			std::cout << "Read Result: " << std::boolalpha << result << '\n';
			if (!result) return;
		}

		std::cout << "Equality check: " << std::boolalpha << (bool)(test_map == result_map) << '\n';
		std::cout << "ORIGINAL\n";
		for (auto& [id, task] : test_map) {
			std::cout << "------\n";
			std::cout << "ID (mp): " << id << '\n';
			std::cout << "ID: " << task.id << '\n';
			std::cout << "Description: " << task.description << '\n';
			std::cout << "Status: " << taskcli::to_name(task.status) << '\n';
			std::cout << "Created Time: " << taskcli::formatted_time(task.createdAt) << '\n';
			std::cout << "Updated Time: " << taskcli::formatted_time(task.updatedAt) << '\n';
			std::cout << "------\n";
		}
		std::cout << "AFTER PARSING\n";
		for (auto& [id, task] : result_map) {
			std::cout << "------\n";
			std::cout << "ID (mp): " << id << '\n';
			std::cout << "ID: " << task.id << '\n';
			std::cout << "Description: " << task.description << '\n';
			std::cout << "Status: " << taskcli::to_name(task.status) << '\n';
			std::cout << "Created Time: " << taskcli::formatted_time(task.createdAt) << '\n';
			std::cout << "Updated Time: " << taskcli::formatted_time(task.updatedAt) << '\n';
			std::cout << "------\n";
		}
	}
}