#include "table.hpp"
#include <vector>
#include <ostream>


namespace taskcli::table {

	void print_row_copy(std::ostream& out, const size_t* f_first, const size_t* f_last, const std::string_view* d_begin, const std::string_view* d_end) {
		if (f_first == f_last) return;

		std::vector<std::string_view> data(d_end - d_begin);
		std::copy(d_begin, d_end, data.begin());
		print_row(out, f_first, f_last, &data[0]);
	}

	void print_row(std::ostream& out, const size_t* f_first, const size_t* f_last, std::string_view* data) {
		if (f_first == f_last) return;
		bool nextLine = true;

		while (nextLine) {
			nextLine = false;
			out << '|';
			for (const size_t* ptr = f_first; ptr != f_last; ptr++) {
				size_t sz = data[ptr - f_first].size();

				if (sz <= *ptr) {
					out << ' ' << data[ptr - f_first];
					for (sz++; sz < *ptr + 2; sz++) {
						out << ' ';
					}
					data[ptr - f_first] = std::string_view {};
				} else {
					nextLine = true;
					out << ' ' << data[ptr - f_first].substr(0, *ptr) << ' ';
					data[ptr - f_first] = data[ptr - f_first].substr(*ptr);
				}
				out << '|';
			}
			out << '\n';
		}
		print_div(out, f_first, f_last);
	}

	void print_div(std::ostream& out,
		const size_t* first, const size_t* last) {
		if (first == last) return;

		out << '+';
		size_t size = last - first;
		for (const size_t* ptr = first; ptr != last; ptr++) {
			for (size_t c = *ptr; c > 0; c--) out << '-';
			out << "--+";
		}
	}
}
