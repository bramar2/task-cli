#include "json.hpp"
#include <ranges>
#include <unordered_map>


namespace taskcli::json {
	char& next_char(std::istream& in, char& ch) {
		while (in.read(&ch, 1)) {
			if (!std::isspace(ch)) {
				return ch;
			}
		}
		ch = '\0';
		return ch;
	}
	bool read_str(std::istream& in, char buf[], size_t buf_size, size_t& result) {
		result = 0;
		char tmp;
		bool literalBackslash = false;
		while (in.read(&tmp, 1) && result + 5 < buf_size) {
			if (result > 0 && buf[result - 1] == '\\' && !literalBackslash) {
				// yes i added support for escaping \b \t \n, ... for some reason
				// even though program only supports printables
				// (not sure if it even works)
				switch (tmp) {
				case 'b':
					buf[result - 1] = '\b';
					break;
				case 't':
					buf[result - 1] = '\t';
					break;
				case 'n':
					buf[result - 1] = '\n';
					break;
				case 'f':
					buf[result - 1] = '\f';
					break;
				case 'r':
					buf[result - 1] = '\r';
					break;
				case '"':
					buf[result - 1] = '"';
					break;
				case '\\':
					literalBackslash = true;
					break;
				default:
					return false;
				}
			} else if (tmp == '"') {
				buf[result] = 0;
				return true;
			} else {
				if (BAD_JSON[tmp]) return false;
				buf[result] = tmp;
				result++;
				literalBackslash = false;
			}
		}
		return false; // doesn't end / above buf size
	}
	char read_int(std::istream& in, bool sign, char buf[], size_t buf_size, size_t& result) {
		result = 0;
		char tmp;
		while (in.read(&tmp, 1) && result + 5 < buf_size) {
			if (std::isspace(tmp)) {
				next_char(in, tmp);
				if (result != 0 && std::isdigit(tmp)) {
					return '\0';
				}
			}

			if (tmp == ',' || tmp == '}') {
				if (result == 0) {
					return '\0';
				}
				buf[result] = '\0';
				return tmp;
			}
			if ((tmp == '-' && result == 0 && sign) || std::isdigit(tmp)) {
				buf[result] = tmp;
				result++;
			} else {
				break;
			}
		}
		return '\0';
	}

	bool read(std::istream& in, std::map<uint64_t, taskcli::Task>& tasks) {
		char ch;
		if (next_char(in, ch) != '[') {
			return false;
		}
		
		char buf[256];
		size_t size;
		if (next_char(in, ch) == '{') {
			do {
				if (ch == ',' && next_char(in, ch) != '{') {
					return false;
				}

				std::unordered_map<std::string, std::string> mp;
				while (true) {
					if (next_char(in, ch) != '"') {
						return false;
					}

					bool success = read_str(in, buf, std::size(buf), size);
					if (!success) return false;


					std::string key(buf);
					if (mp.count(key) || !KEY_TYPE.count(key)) {
						return false;
					}
					if (next_char(in, ch) != ':') {
						return false;
					}
					switch (KEY_TYPE.at(key)) {
					case TYPE_STR:
						if (next_char(in, ch) != '"') return false;

						success = read_str(in, buf, std::size(buf), size);
						if (!success) return false;
						next_char(in, ch);
						break;
					case TYPE_UNSIGNED:
						ch = read_int(in, false, buf, std::size(buf), size);
						break;
					case TYPE_SIGNED:
						ch = read_int(in, true, buf, std::size(buf), size);
						break;
					default:
						return false;
					}

					if (ch == '\0') return false;
					mp.emplace(key, std::string(buf));

					if (ch == ',') {
						continue;
					} else if (ch == '}') {
						break;
					} else {
						return false;
					}
				}

				for (const std::string& key : KEY_TYPE | std::views::keys) {
					if (!mp.count(key)) {
						return false;
					}
				}
				try {
					uint64_t id = static_cast<uint64_t>(std::stoull(mp.at("id")));
					std::optional<Status> status = from_name(mp.at("status"));
					if (!status.has_value()) return false;

					time_t created = static_cast<time_t>(std::stoll(mp.at("created")));
					time_t updated = static_cast<time_t>(std::stoll(mp.at("updated")));

					if (tasks.count(id)) return false;
					if (updated < created) return false;
					if (!valid_description(mp.at("description"))) return false;
					tasks.emplace(id,
						Task(id, mp.at("description"), status.value(), created, updated));

				} catch (const std::exception& err) {
					return false;
				}


			} while (next_char(in, ch) == ',');
		}

		if (ch == ']') {
			return next_char(in, ch) == (char) 0;
		} else {
			return false;
		}
	}

	void write(std::ostream& out, const std::map<uint64_t, taskcli::Task>& tasks) {
		out << "[\n";
		bool first = true;
		for (const taskcli::Task& task : tasks | std::views::values) {
			if (first) {
				first = false;
			} else {
				out << ",";
			}
			out << "  {\n";
			out << "    \"id\": " << task.id << ",\n";
			out << "    \"description\": \"";

			for (char c : task.description) {
				if (c == '"') out << "\\\"";
				else if (c == '\\') out << "\\\\";
				else out << c;
			}

			out << "\",\n";
			out << "    \"status\": \"" << taskcli::to_name(task.status) << "\",\n";
			out << "    \"created\": " << task.createdAt << ",\n";
			out << "    \"updated\": " << task.updatedAt << "\n";
			out << "  }\n";
		}
		out << "]\n";
	}
}