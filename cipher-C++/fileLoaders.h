#pragma once
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>

namespace fileLoaders {
	//These are various file loading helpers for loading different files
	//See implementation for more details

	std::string loadFile(std::string fileName);

	std::vector<std::string> loadLines(std::string fileName);

	std::vector<std::string> loadVector(std::string fileName);

	std::unordered_map<std::string, int> loadMapInt(std::string fileName);

	std::unordered_map<std::string, float> loadMapDouble(std::string fileName);
}