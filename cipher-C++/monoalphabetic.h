#pragma once
#include <array>
#include <string>

namespace monoalphabetic {
	std::string encrypt(std::string text, std::array<char, 26> key);

	std::array<char, 26> invertKey(std::array<char, 26> key);

	std::string decrypt(std::string text, std::array<char, 26> key, bool encryptionKey);

	std::array<char, 26> stringToKey(std::string key);

	static std::array<char, 26> atbashKey = stringToKey("ZYXWVUTSRQPONMLKJIHGFEDCBA");

	std::string atbash(std::string cipher);

	int cliAtbash(std::string cipher);

	std::string caesarEncrypt(std::string text, int n);

	std::string caesarDecrypt(std::string cipher, int n);

	std::string caesarBruteForce(std::string cipher);

	int cliCaesarBruteForce(std::string cipher);
}