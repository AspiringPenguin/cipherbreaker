#include "basics.h"	
#include "fitness.h"
#include "interface.h"
#include "monoalphabetic.h"
#include "periodic.h"
#include "strings.h"
#include <iostream>
#include <random>

namespace periodic {
	int getKeySize(std::string cipher) {
		for (int i = 2; i < 21; i++) {
			if (fitness::indexOfCoincidencePeriodic(cipher, i) > 0.06) {
				return i;
			}
		}
		return -1;
	}

	std::string vigenere(std::string cipher, int keyLen) {
		auto columns = strings::getColumns(cipher, keyLen);

		auto decryptedColumns = std::vector<std::string>();

		std::string result;

		for (int i = 0; i < keyLen; i++) {
			result = monoalphabetic::caesarMonogramBruteForce(columns[i]);
			if (result == "") {
				return "";
			}
			decryptedColumns.push_back(result);
		}

		return strings::columnsToString(decryptedColumns);
	}

	std::string vigenere(std::string cipher)
	{
		int keySize = getKeySize(cipher);
		if (keySize == -1) {
			return "";
		}
		return vigenere(cipher, keySize);
	}

	int cliVigenere(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto result = vigenere(cipher);
		if (result == "") {
			return 0;
		}
		if (fitness::tetragramFitness(&result) > -15) {
			if (cliInterface::offerDecryption(result)) {
				return 1;
			}
		}
		return 0;
	}

	std::string polyalphabeticDecrypt(std::string cipher, std::vector<std::array<char, 26>> key, bool encryptionKey) {
		int n = key.size();
		auto cols = strings::getColumns(cipher, n);
		auto newCols = std::vector<std::string>();
		for (int i = 0; i < n; i++){
			newCols.push_back(monoalphabetic::decrypt(cols[i], key[i], encryptionKey));
		}
		return strings::columnsToString(newCols);
	}

	std::string hillClimber(std::string cipher, int keySize) {
		int l = cipher.length();

		//Generate best key
		auto alphabetKey = monoalphabetic::stringToKey(basics::alphabet);
		auto bestKey = std::vector<std::array<char, 26>>();
		for (int i = 0; i < keySize; i++) {
			bestKey.push_back(alphabetKey);
		}

		//Set up the outer variables
		int bigCounter = 0;

		auto bestDecrypt = polyalphabeticDecrypt(cipher, bestKey, false);

		float bestFitness = fitness::tetragramFitness(&bestDecrypt);

		int limit = (keySize * keySize) * 100000000 / l;

		//Set up for vars in loops
		int littleCounter;
		std::vector<std::array<char, 26>> parentKey;
		std::vector<std::array<char, 26>> childKey;
		float parentFitness;
		std::string parentDecrypt;
		float childFitness;
		std::string childDecrypt;
		int x, y;
		char _;

		//Set up for random
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 25);

		std::ios_base::sync_with_stdio(false);

		std::cout << limit << std::endl;

		//Actual loop
		while (bigCounter < limit) {
			std::cout << bigCounter << std::endl;
			for (int i = 0; i < keySize; i++) {
				parentKey = bestKey;
				std::shuffle(std::begin(parentKey[i]), std::end(parentKey[i]), gen);
				parentDecrypt = polyalphabeticDecrypt(cipher, parentKey, false);
				parentFitness = fitness::tetragramFitness(&parentDecrypt);
				littleCounter = 0;
				while (littleCounter < 1000) {
					childKey = parentKey;
					x = dist(gen);
					y = dist(gen);
					while (x == y) {
						y = dist(gen);
					}
					_ = childKey[i][x];
					#pragma warning( push )
					#pragma warning( disable : 28020 )
					childKey[i][x] = childKey[i][y]; //warning: ignore
					#pragma warning( pop )
					childKey[i][y] = _;
					childDecrypt = polyalphabeticDecrypt(cipher, childKey, false);
					childFitness = fitness::tetragramFitness(&childDecrypt);
					if (childFitness > parentFitness) {
						parentKey = childKey;
						parentDecrypt = childDecrypt;
						parentFitness = childFitness;
						littleCounter = 0;
					}
					littleCounter++;
					if (childFitness > bestFitness) {
						std::cout << childFitness << std::endl;
						bestDecrypt = childDecrypt;
						bestKey = childKey;
						bestFitness = childFitness;
						bigCounter = 0;
						if (bestFitness > -15) {
							for (int n = 0; n < keySize; n++) {
								std::cout << monoalphabetic::keyToString(monoalphabetic::invertKey(bestKey[n])) << std::endl;
							}
						}
					}
					bigCounter++;
				}
			}
		}

		std::ios_base::sync_with_stdio(true);

		for (int n = 0; n < keySize; n++) {
			std::cout << monoalphabetic::keyToString(monoalphabetic::invertKey(bestKey[n])) << std::endl;
		}

		return polyalphabeticDecrypt(cipher, bestKey, false);
	}

	std::string hillClimber(std::string cipher) {
		//Get key size
		int keySize = getKeySize(cipher);
		if (keySize == -1) {
			return "";
		}
		
		return hillClimber(cipher, keySize);
	}

	int cliHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		std::string decrypt = hillClimber(cipher);
		if (fitness::tetragramFitness(&decrypt) > -15) {
			if (cliInterface::offerDecryption(decrypt)) {
				return 1;
			}
		}
		return 0;
	}
}