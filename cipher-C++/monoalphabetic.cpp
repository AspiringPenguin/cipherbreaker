#include "basics.h"
#include "fitness.h"
#include "interface.h"
#include "monoalphabetic.h"
#include <iostream>

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

	std::array<char, 26> stringToKey(std::string key) {
		key = basics::formatString(key);

		auto arr = std::array<char, 26>();

		for (int i = 0; i < 26; i++) { //Each char in key
			arr[i] = key[i];
		}

		return arr;
	}

	std::string atbash(std::string cipher) { //Reciprocal key
		return encrypt(cipher, atbashKey);
	}

	int cliAtbash(std::string cipher) {
		cipher = basics::formatString(cipher);

		auto decrypted = atbash(cipher);

		if (fitness::tetragramFitness(decrypted) > -18) {
			if (cliInterface::offerDecryption(decrypted)) {
				return 1;
			}
		}
		return 0;
	}
}
