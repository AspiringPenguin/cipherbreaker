#include "basics.h"
#include "monoalphabetic.h"

namespace monoalphabetic {
	std::string encrypt(std::string text, std::array<char, 26> key)
	{
		std::string encrypted = "";

		int l = text.length();

		for (int i = 0; i < l; i++) {
			encrypted += key[basics::alphabetIndex[text[i]]];
		}

		return encrypted;
	}

	std::array<char, 26> invertKey(std::array<char, 26> key)
	{
		auto newKey = std::array<char, 26>();

		for (int i = 0; i < 26; i++) {
			newKey[basics::alphabetIndex[key[i]]] = basics::alphabet[i];
		}

		return newKey;
	}

	std::string decrypt(std::string text, std::array<char, 26> key, bool encryptionKey)
	{
		if (!encryptionKey) {
			return encrypt(text, key);
		}
		else {
			return encrypt(text, invertKey(key));
		}
	}
}
