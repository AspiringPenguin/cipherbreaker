#pragma once
#include <generator>
#include <string>
#include <vector>

namespace transpositions {
	std::generator<std::vector<int>> heapsPerms(int size);

	std::string permutationBruteForce(std::string cipher);
	int cliPermutationBruteForce(std::string cipher);
}