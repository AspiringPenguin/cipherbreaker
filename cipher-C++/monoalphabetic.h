#pragma once
#include <array>
#include <string>

namespace monoalphabetic {
	std::string encrypt(std::string text, std::array<char, 26> key);

	std::array<char, 26> invertKey(std::array<char, 26> key);

	std::string decrypt(std::string text, std::array<char, 26> key, bool encryptionKey);

	std::array<char, 26> stringToKey(std::string key);

	std::string keyToString(std::array<char, 26> key);

	static std::array<char, 26> atbashKey = stringToKey("ZYXWVUTSRQPONMLKJIHGFEDCBA");

	std::string atbash(std::string cipher);

	int cliAtbash(std::string cipher);

	std::string caesarEncrypt(std::string text, int n);

	std::string caesarDecrypt(std::string cipher, int n);

	std::string caesarBruteForce(std::string cipher);

	int cliCaesarBruteForce(std::string cipher);

	static int affineKeys[] = { 1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25 };

	std::string affineEncrypt(std::string text, int a, int b);

	std::string affineDecrypt(std::string text, int a, int b);

	std::string affineBruteForce(std::string cipher);

	int cliAffineBruteForce(std::string cipher);
}