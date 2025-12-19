#pragma once
#include <string>
#include <unordered_map>

namespace homophonic {
	std::string homophonicDecrypt(std::string cipher, std::unordered_map<char, char> key);

	std::unordered_map<char, char> homophonicHillClimber(std::string cipher);

	std::string dualBetDecrypt(std::string cipher, std::array<char, 26> upper, std::array<char, 26> lower);

	std::tuple<std::array<char, 26>, std::array<char, 26>> dualBetHillClimber(std::string cipher);
}