#pragma once
#include <string>
#include <tuple>

namespace polybius {
	typedef std::array<std::array<char, 5>, 5> polybius;

	std::tuple<int, int> findInPolybius(char c, polybius key);

	std::string playfairDecrypt(std::string text, polybius key);

	std::tuple<polybius, std::string> playfairHillClimber(std::string cipher);
}