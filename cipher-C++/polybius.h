#pragma once
#include <string>
#include <tuple>
#include <unordered_set>

namespace polybius {
	typedef std::array<std::array<char, 5>, 5> polybius;

	static polybius nullPolybius{ -1 };

	polybius makePolybius(std::string key);

	static polybius alphabetPolybius = makePolybius("abcdefghijklmnopqrstuvwxyz");

	std::tuple<int, int> findInPolybius(char c, polybius key);

	//Playfair
	std::string playfairDecrypt(std::string text, polybius key);

	polybius playfairHillClimber(std::string cipher);
	//This is used in the above
	polybius playfairBacktracking(std::string cipher, polybius startKey, bool ignoreBad);

	int cliPlayfairHillClimber(std::string cipher);

	//Two-square ciphers
	std::string vertTwoSquareDecrypt(std::string cipher, polybius top, polybius bottom, int flips = 0);
	std::string horizTwoSquareEncrypt(std::string cipher, polybius left, polybius right, int flips);
	std::string horizTwoSquareDecrypt(std::string cipher, polybius left, polybius right, int flips = 1);

	std::tuple<polybius, polybius> vertHillClimber(std::string cipher);
}