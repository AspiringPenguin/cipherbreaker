#include "strings.h"

namespace strings {
	std::vector<std::string> split(std::string string, std::string delimiter)
	{
		auto sections = std::vector<std::string>();

		int pos = 0;
		int dl = delimiter.length();
		int l = string.length() - dl + 1;

		std::string substr;

		for (int i = 0; i < l; i++) {
			substr = string.substr(i, dl);
			if (substr == delimiter) {
				sections.push_back(string.substr(pos, i - pos));
				pos = i + 1;
			}
		}

		sections.push_back(string.substr(pos, string.length() - pos));

		return sections;
	}

	std::vector<std::string> getBlocks(std::string string, int size) {
		auto blocks = std::vector<std::string>();
		int l = string.size();
		blocks.reserve(l/size);
		for (int i = 0; i < l; i += size) {
			blocks.push_back(string.substr(i, size));
		}
		return blocks;
	}

	std::vector<std::string> getColumns(std::string string, int num) {
		auto blocks = getBlocks(string, num);
		auto columns = std::vector<std::string>(num, "");
		for (std::string block : blocks) {
			for (int i = 0; i < block.size(); i++) {
				columns[i] += block[i];
			}
		}
		return columns;
	}

	std::string blocksToString(std::vector<std::string> blocks) {
		std::string result = "";
		result.reserve(blocks.size() * blocks[0].size());
		for (std::string block: blocks) {
			result += block;
		}
		return result;
	}

	std::string columnsToString(std::vector<std::string> columns) {
		std::string result = "";
		int l = columns[0].size() - 1;
		int m = columns.size();
		for (int i = 0; i < l; i++) {
			for (int j = 0; j < m; j++) {
				result += columns[j][i];
			}
		}
		for (int i = 0; i < m; i++) {
			if (columns[i].size() == (l + 1)) {
				result += columns[i][l];
			}
		}
		return result;
	}
}