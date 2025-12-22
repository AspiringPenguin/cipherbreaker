#include "strings.h"

namespace strings {
	//Equivalent to python str.split(delimitet) in functionality
	//Splits the input string by the delimiter and returns an std::vector<std::string> containing the split parts
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

	//Turns the string into length size blocks
	std::vector<std::string> getBlocks(std::string string, int size) {
		auto blocks = std::vector<std::string>();
		int l = string.size();
		blocks.reserve(l/size);
		for (int i = 0; i < l; i += size) {
			blocks.push_back(string.substr(i, size));
		}
		return blocks;
	}

	//If the blocks from the above are laid out vertically, this returns what would be the columns:
	//AAA
	//BCD
	//EFG
	//from string AAABCDEFG, giving columns {ABD, ACF, ADG}
	//Which is useful for periodic ciphers and some transposition ciphers
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

	//Rejoins blocks together into a single string
	std::string blocksToString(std::vector<std::string> blocks) {
		std::string result = "";
		result.reserve(blocks.size() * blocks[0].size());
		for (std::string block: blocks) {
			result += block;
		}
		return result;
	}

	//Rejoins columns together correctly into a string
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