#pragma once
#include <string>
#include <tuple>
#include <unordered_set>

namespace polybius {
	//A large grouping of tools for polybius-based ciphers
	
	typedef std::array<std::array<char, 5>, 5> polybius; //Define the polybius type as a list of rows, so its indexed [y][x]

	static polybius nullPolybius{ -1 }; //For returning 'null' results from attacks

	polybius makePolybius(std::string key);

	static polybius alphabetPolybius = makePolybius("abcdefghijklmnopqrstuvwxyz");

	std::tuple<int, int> findInPolybius(char c, polybius key);

	//Playfair
	std::string playfairDecrypt(std::string text, polybius key);
	std::string playfair2025VariationDecrypt(std::string text, polybius key);

	polybius playfairHillClimber(std::string cipher);
	polybius playfair2025VariationHillClimber(std::string cipher);
	//These is used in the above
	polybius playfairImprover(std::string cipher, polybius startKey, bool ignoreBad);
	polybius playfair2025VariationImprover(std::string cipher, polybius startKey, bool ignoreBad);

	int cliPlayfairHillClimber(std::string cipher);
	int cliPlayfair2025VariationHillClimber(std::string cipher);

	//Two-square ciphers
	std::string vertTwoSquareDecrypt(std::string cipher, polybius top, polybius bottom, int flips = 0);
	std::string horizTwoSquareEncrypt(std::string cipher, polybius left, polybius right, int flips);
	std::string horizTwoSquareDecrypt(std::string cipher, polybius left, polybius right, int flips = 1);

	std::tuple<polybius, polybius> vertTwoSquareHillClimber(std::string cipher);
	//Used in the above
	std::tuple<polybius, polybius> vertTwoSquareImprover(std::string cipher, polybius startTop, polybius startBottom, bool ignoreBad);
	int cliVertTwoSquareHillClimber(std::string cipher);

	std::tuple<polybius, polybius> horizTwoSquareHillClimber(std::string cipher);
	//Used in the above
	std::tuple<polybius, polybius> horizTwoSquareImprover(std::string cipher, polybius startTop, polybius startBottom, bool ignoreBad);
	int cliHorizTwoSquareHillClimber(std::string cipher);

	//Four-square
	std::string fourSquareDecrypt(std::string text, polybius topRight, polybius bottomLeft);
	std::tuple<polybius, polybius> fourSquareHillClimber(std::string cipher);
	int cliFourSquareHillClimber(std::string cipher);
}