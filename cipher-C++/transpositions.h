#pragma once
#include <generator>
#include <string>
#include <vector>

namespace transpositions {
	int integerDivisionRoundUp(int a, int b);

	std::generator<std::vector<int>> heapsPerms(int size);

	std::vector<int> rollKey(std::vector<int>& key, int roll);

	std::string rollString(std::string toRoll, int roll);

	std::string permutationDecrypt(std::string cipher, std::vector<int> key);

	std::string permutationBruteForce(std::string cipher);
	int cliPermutationBruteForce(std::string cipher);

	std::vector<int> permutationSubHillClimber(std::string cipher, int keyLen);
	std::vector<int> permutationHillClimber(std::string cipher);
	int cliPermutationHillClimber(std::string cipher);

	std::string columnarDecrypt(std::string cipher, std::vector<int> key);
	std::string columnarBruteForce(std::string cipher);
	int cliColumnarBruteForce(std::string cipher);

	std::vector<int> columnarSubHillClimber(std::string cipher, int keyLen);
	std::vector<int> columnarHillClimber(std::string cipher);
	int cliColumnarHillClimber(std::string cipher);

	std::string twistedScytaleDecrypt(std::string cipher, int width, int twist);
}