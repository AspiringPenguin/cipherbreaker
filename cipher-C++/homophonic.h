#pragma once
#include <string>
#include <unordered_map>

namespace homophonic {
	//I have no clue how much of this works well - I wrote it for 10B as something to try and haven't touched it since
	//See implementation for details about each

	std::string homophonicDecrypt(std::string cipher, std::unordered_map<char, char> key);

	std::unordered_map<char, char> homophonicHillClimber(std::string cipher);

	std::string dualBetDecrypt(std::string cipher, std::array<char, 26> upper, std::array<char, 26> lower);

	std::tuple<std::array<char, 26>, std::array<char, 26>> dualBetHillClimber(std::string cipher);
}