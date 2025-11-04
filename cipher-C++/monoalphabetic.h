#pragma once
#include <array>
#include <string>

namespace monoalphabetic {
	std::string encrypt(std::string text, std::array<char, 26> key);

	std::array<char, 26> invertKey(std::array<char, 26> key);

	std::string decrypt(std::string text, std::array<char, 26> key, bool encryptionKey);

	std::array<char, 26> stringToKey(std::string key);

	std::string atbash(std::string cipher);

	static std::array<char, 26> atbashKey = stringToKey("ZYXWVUTSRQPONMLKJIHGFEDCBA");
}