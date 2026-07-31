#pragma once
#include <unordered_map>
#include <vector>
#include <string>

namespace basics {
	//See implementation file for comments regarding these

	static std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

	std::string formatString(std::string inp);

	std::string removeSpaces(std::string inp);

	int lcm(int a, int b);

	int hcf(int a, int b);

	bool coprime(int a, int b);

	int multiplicativeInverse(int n, int modulus);

	std::vector<int> getFactors(int n);
}