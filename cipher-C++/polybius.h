#pragma once
#include <string>
#include <tuple>
#include <unordered_set>

namespace polybius {
	typedef std::array<std::array<char, 5>, 5> polybius;

	static polybius nullPolybius { -1 };

	polybius makePolybius(std::string key);

	std::tuple<int, int> findInPolybius(char c, polybius key);

	std::string playfairDecrypt(std::string text, polybius key);

	std::tuple<polybius, std::string> playfairHillClimber(std::string cipher);

	polybius playfairBacktracking(std::string cipher, polybius startKey);
	polybius playfairBacktracking(std::string cipher);
	std::string playfairGPUBrute(std::string cipher);
}