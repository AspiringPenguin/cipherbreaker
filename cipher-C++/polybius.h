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

	polybius playfairHillClimber(std::string cipher);

	polybius playfairBacktracking(std::string cipher, polybius startKey, bool ignoreBad);
	polybius playfairBacktracking(std::string cipher);
}