#pragma once
#include <ostream>

namespace taskcli::table {

	constexpr size_t LIST_FIELDS[] {6, 30, 11, 22, 22};
	constexpr std::string_view LIST_HEAD[] {"ID", "Description", "Status", "Created", "Updated"};

	void print_row_copy(std::ostream& out, const size_t* f_first, const size_t* f_last, const std::string_view* d_begin, const std::string_view* d_end);
	void print_row(std::ostream& out, const size_t* f_first, const size_t* f_last, std::string_view* data);
	void print_div(std::ostream& out, const size_t* first, const size_t* last);
}