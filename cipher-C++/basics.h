#pragma once
#include <unordered_map>
#include <string>

namespace basics {
	static std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

	std::string formatString(std::string inp);

	std::string removeSpaces(std::string inp);

	std::unordered_map<char, int> createAlphabetMap();

	static std::unordered_map<char, int> alphabetIndex = createAlphabetMap();

	int lcm(int a, int b);

	int hcf(int a, int b);

	bool coprime(int a, int b);

	int multiplicativeInverse(int n, int modulus);
}