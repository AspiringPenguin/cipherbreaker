#pragma once
#include <unordered_map>
#include <string>

namespace basics {
	static std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

	std::unordered_map<char, int> createAlphabetMap();

	static std::unordered_map<char, int> alphabetIndex = createAlphabetMap();
}