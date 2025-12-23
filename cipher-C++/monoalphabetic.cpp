#include "basics.h"
#include "fitness.h"
#include "interface.h"
#include "modularNumber.h"
#include "monoalphabetic.h"
#include <iostream>
#include <random>

namespace monoalphabetic {
	//Encrypt the text using the key. This also works with the inverse key for decryption
	std::string encrypt(std::string text, std::array<char, 26> key)
	{
		std::string encrypted = "";

		int l = text.length();

		for (int i = 0; i < l; i++) {
			encrypted += key[key[i] - 97];
		}

		return encrypted;
	}

	//Invert the key e.g. for alphabet ABCD, key ACDB -> inverse ADBC
	std::array<char, 26> invertKey(std::array<char, 26> key)
	{
		auto newKey = std::array<char, 26>();

		for (int i = 0; i < 26; i++) {
			newKey[key[i] - 97] = basics::alphabet[i];
		}

		return newKey;
	}

	//Decrypt the text. If encryptionKey is set the key is inverted before being used to 'encrypt' the text to produce the original
	std::string decrypt(std::string text, std::array<char, 26> key, bool encryptionKey)
	{
		if (!encryptionKey) {
			return encrypt(text, key);
		}
		else {
			return encrypt(text, invertKey(key));
		}
	}

	//Convert a string key to an array key
	std::array<char, 26> stringToKey(std::string key) {
		key = basics::formatString(key);

		auto arr = std::array<char, 26>();

		for (int i = 0; i < 26; i++) { //Each char in key
			arr[i] = key[i];
		}

		return arr;
	}

	//Convert an array key to a string key
	std::string keyToString(std::array<char, 26> key) {
		std::string strKey = "";

		for (char c : key) {
			strKey += c;
		}

		return strKey;
	}

	//Atbash decrypt
	std::string atbash(std::string cipher) { //Reciprocal key
		return encrypt(cipher, atbashKey);
	}

	//Interface code for atbash
	int cliAtbash(std::string cipher) {
		cipher = basics::formatString(cipher);

		auto decrypted = atbash(cipher);

		if (fitness::tetragramFitness(&decrypted) > -18) {
			if (cliInterface::offerDecryption(decrypted)) {
				return 1;
			}
		}
		return 0;
	}

	//Caesar encrypt - decrypt with inverse
	std::string caesarEncrypt(std::string text, int shift) {
		return caesarDecrypt(text, 26-shift);
	}

	//Caesar decrypt
	std::string caesarDecrypt(std::string cipher, int shift) {
		std::string text = "";
		modularNumber n;
		for (char c : cipher) {
			n = (c - 97); //a = 97 in ascii & unicode
			n += shift;
			text += basics::alphabet[n];
		}
		return text;
	}

	//Brute force attack on the caesar shift cipher using tetragram fitness. Excludes shift = 0 as this does nothing, and this isn't used for vigenere
	std::string caesarBruteForce(std::string cipher) {
		std::string decrypt;
		for (int i = 1; i < 26; i++) {
			decrypt = caesarDecrypt(cipher, i);
			if (fitness::tetragramFitness(&decrypt) > -15) {
				return decrypt;
			}
		}
		return "";
	}

	//Brute force attack on the caesar shift cipher using monogram fitness. Includes shift=0 as this is used for vigenere ciphers, and there could be an 'a' in the key
	std::string caesarMonogramBruteForce(std::string cipher) {
		std::string decrypt;
		std::string bestDecrypt;
		float fitness;
		float bestFitness = -100;
		for (int i = 0; i < 26; i++) {
			decrypt = caesarDecrypt(cipher, i);
			fitness = fitness::angleBetweenVectorsFitness(decrypt);
			if (fitness > bestFitness) {
				bestFitness = fitness;
				bestDecrypt = decrypt;
			}
		}
		return bestDecrypt;
	}

	//Cli interface code for the caesar cipher.
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

	//Affine encryption code
	std::string affineEncrypt(std::string text, int a, int b) {
		std::string result = "";
		modularNumber n;
		for (char c : text) {
			n = (c - 97); //a = 97 in ascii & unicode
			n = n * a;
			n += b;
			result += basics::alphabet[n.getValue()];
		}
		return result;
	}

	//Affine decryption, which is effectively identical to encryption but with multiplicative and additive inverses
	std::string affineDecrypt(std::string text, int a, int b) {
		a = basics::multiplicativeInverse(a, 26);
		std::string result = "";
		modularNumber n;
		for (char c : text) {
			n = (c - 97); //a = 97 in ascii & unicode
			n = n * a;
			n = n - b;
			result += basics::alphabet[n.getValue()];
		}
		return result;
	}

	//Brute force attack on the affine shift cipher with tetragram fitness
	std::string affineBruteForce(std::string cipher) {
		std::string decryptedText;
		for (int a : affineKeys) {
			for (int b = 0; b < 26; b++) {
				decryptedText = affineDecrypt(cipher, a, b);
				if (fitness::tetragramFitness(&decryptedText) > -15) {
					return decryptedText;
				}
			}
		}
		return "";
	}

	//CLI code for affine shift cipher brute force attack
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

	//Hill climber for monoalphabetic substitution 
	//This uses the "basic hill-climber" setup, as explained in hillClimberTypes.txt
	std::string hillClimber(std::string cipher, int limit) {

		//Set up parent and child key, decrypt and fitness
		auto parentKey = stringToKey("abcdefghijklmnopqrstuvwxyz"); //An arbitrary starting key
		auto parentPlain = decrypt(cipher, parentKey, true);
		float parentFitness = fitness::tetragramFitness(&parentPlain);
		std::array<char, 26> childKey;
		std::string childPlain;
		float childFitness;

		//Set up for random numbers
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 25); //For an index in the key

		//Loop control variables
		int counter = 0;

		//Temp storage for key changes
		int a, b, _;

		while (counter < limit) {
			childKey = parentKey;
			childPlain = parentPlain;

			//Swap two characters in the key
			a = dist(gen);
			b = dist(gen);
			_ = childKey[a];
			childKey[a] = childKey[b];
			childKey[b] = _;

			//Incrementally update the bestDecypt by iterating through and swapping the letters concerned
			for (int i = 0; i < childPlain.size(); i++) {
				if (childPlain[i] == childKey[a]) {
					childPlain[i] = childKey[b];
				}
				else if (childPlain[i] == childKey[b]) {
					childPlain[i] = childKey[a];
				}
			}

			//Get child fitness
			childFitness = fitness::tetragramFitness(&childPlain);

			if (childFitness > parentFitness) {
				parentFitness = childFitness;
				parentKey = childKey;
				parentPlain = childPlain;
				counter = 0;
			}

			counter++;
		}
		std::cout << parentFitness << std::endl;
		return parentPlain;
	}

	//CLi interface for monoalphabetic hillclimbing attack
	int cliHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto result = hillClimber(cipher);
		if (fitness::tetragramFitness(&result) > -15) {
			if (cliInterface::offerDecryption(result)) {
				return 1;
			}
		}
		return 0;
	}
}
