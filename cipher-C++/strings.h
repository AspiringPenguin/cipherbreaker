#pragma once
#include <string>
#include <vector>

namespace strings {
	//A set of tools for handling strings - see implementation for details

	std::vector<std::string> split(std::string string, std::string delimiter);

	std::vector<std::string> getBlocks(std::string string, int size);
	std::vector<std::string> getColumns(std::string string, int num);
	std::string blocksToString(std::vector<std::string> blocks);
	std::string columnsToString(std::vector<std::string> blocks);
}