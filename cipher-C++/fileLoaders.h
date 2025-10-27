#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

namespace fileLoaders {
	std::string loadFile(std::string fileName);

	std::vector<std::string> loadLines(std::string fileName);
}