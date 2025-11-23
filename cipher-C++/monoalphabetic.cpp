#include "basics.h"
#include "fitness.h"
#include "interface.h"
#include "modularNumber.h"
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

	std::string keyToString(std::array<char, 26> key) {
		std::string strKey = "";

		for (char c : key) {
			strKey += c;
		}

		return strKey;
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

	std::string caesarEncrypt(std::string text, int shift) {
		return caesarDecrypt(text, 26-shift);
	}

	std::string caesarDecrypt(std::string cipher, int shift) {
		std::string text = "";
		modularNumber n;
		for (char c : cipher) {
			n = basics::alphabetIndex[c];
			n += shift;
			text += basics::alphabet[n];
		}
		return text;
	}

	std::string caesarBruteForce(std::string cipher) {
		std::string decrypt;
		for (int i = 1; i < 26; i++) {
			decrypt = caesarDecrypt(cipher, i);
			if (fitness::tetragramFitness(decrypt) > -15) {
				return decrypt;
			}
		}
		return "";
	}

	int cliCaesarBruteForce(std::string cipher) {
		cipher = basics::formatString(cipher);
		std::string decrypt = caesarBruteForce(cipher);
		if (decrypt != "") {
			if (cliInterface::offerDecryption(decrypt)) {
				return 1;
			}
		}
		return 0;
	}

	std::string affineEncrypt(std::string text, int a, int b) {
		std::string result = "";
		modularNumber n;
		for (char c : text) {
			n = basics::alphabetIndex[c];
			n = n * a;
			n += b;
			result += basics::alphabet[n.getValue()];
		}
		return result;
	}

	std::string affineDecrypt(std::string text, int a, int b) {
		a = basics::multiplicativeInverse(a, 26);
		std::string result = "";
		modularNumber n;
		for (char c : text) {
			n = basics::alphabetIndex[c];
			n = n * a;
			n = n - b;
			result += basics::alphabet[n.getValue()];
		}
		return result;
	}

	std::string affineBruteForce(std::string cipher) {
		std::string decryptedText;
		for (int a : affineKeys) {
			for (int b = 0; b < 26; b++) {
				decryptedText = affineDecrypt(cipher, a, b);
				if (fitness::tetragramFitness(decryptedText) > -15) {
					return decryptedText;
				}
			}
		}
		return "";
	}

	int cliAffineBruteForce(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto decryption = affineBruteForce(cipher);
		if (decryption != "") {
			if (cliInterface::offerDecryption(decryption)) {
				return 1;
			}
		}
		return 0;
	}
}
