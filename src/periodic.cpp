#include "basics.h"	
#include "fitness.h"
#include "interface.h"
#include "monoalphabetic.h"
#include "periodic.h"
#include "strings.h"
#include <iostream>
#include <random>

namespace periodic {
	//A method for determining keylength of periodic substitution ciphers
	int getKeySize(std::string cipher) {
		for (int i = 2; i <= 26; i++) { //Tries every period 2 <= i <= 26 to allow for the trithemius cipher
			if (fitness::indexOfCoincidencePeriodic(cipher, i) > 0.06) {
				return i; //if it matches English return i
			}
		}
		return -1;
		//Returns -1 if there is no match
	}

	//Break a vignere by breaking it down into the separate 'columns' with the same caesar shift
	//and breaking those as a caesar with brute force and monogram fitness
	std::string vigenereAsCaesarShifts(std::string cipher, int keyLen) {
		auto columns = strings::getColumns(cipher, keyLen);

		auto decryptedColumns = std::vector<std::string>();

		std::string result;

		for (int i = 0; i < keyLen; i++) {
			result = monoalphabetic::caesarMonogramBruteForce(columns[i]);
			decryptedColumns.push_back(result);
		}

		return strings::columnsToString(decryptedColumns);
	}

	//An overload for the above that determines key length before calling the overload above with the key length
	std::string vigenereAsCaesarShifts(std::string cipher)
	{
		int keySize = getKeySize(cipher);
		if (keySize == -1) {
			return ""; //Return an empty string representing a failed decrypt
		}
		return vigenereAsCaesarShifts(cipher, keySize);
	}

	//CLI interface for the above attack
	int cliVigenereAsCaesarShifts(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto result = vigenereAsCaesarShifts(cipher);
		if (result == "") { //Has the decryption process failed?
			return 0; //Failure
		}
		if (fitness::tetragramFitness(&result) > -15) {
			if (cliInterface::offerDecryption(result)) {
				return 1; //Success
			}
		}
		return 0; //Failure
	}

	//Polyalphabetic decrypt, with a list of monoalphabetic keys, one for each 'column' and a bool for if it is an encryption key which just gets passed through
	std::string polyalphabeticDecrypt(std::string cipher, std::vector<std::array<char, 26>> key, bool encryptionKey) {
		int n = key.size(); 
		auto cols = strings::getColumns(cipher, n);
		auto newCols = std::vector<std::string>(); //To put decrypted columns in
		for (int i = 0; i < n; i++){
			newCols.push_back(monoalphabetic::decrypt(cols[i], key[i], encryptionKey)); //Decrypt each 'column' with the relevant key
		}
		return strings::columnsToString(newCols); //COmbine the decrypted columns into a text
	}

	//Hill climbing attack on periodic substitution ciphers
	//As described in Madness's book at the end of the periodic ciphers section, with just a different limit calculation
	//Uses a custom hill climbing attack, see "periodic substitution hill climber"
	std::string hillClimber(std::string cipher, long keySize) {
		long l = cipher.length();

		//Generate best key as a list of copies of the alphabet
		auto alphabetKey = monoalphabetic::stringToKey(basics::alphabet);
		auto bestKey = std::vector<std::array<char, 26>>();
		for (int i = 0; i < keySize; i++) {
			bestKey.push_back(alphabetKey);
		}

		//Set up the outer variables
		int bigCounter = 0;

		auto bestDecrypt = polyalphabeticDecrypt(cipher, bestKey, false);

		float bestFitness = fitness::tetragramFitness(&bestDecrypt);

		long limit = (keySize * keySize) * (500000000l / l); // A variation on something I found in an academic article to make long texts finish more quickly
														     // without compromising accuracy on short texts

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

		std::ios_base::sync_with_stdio(false); //Makes std::cout faster so it isn't a bottleneck to print status updates

		std::cout << limit << std::endl;

		//Actual loop
		while (bigCounter < limit) {
			std::cout << bigCounter << std::endl;
			for (int i = 0; i < keySize; i++) {
				//Set parent key to a copy of the best key and shuffle the ith subkey
				parentKey = bestKey;
				std::shuffle(std::begin(parentKey[i]), std::end(parentKey[i]), gen);

				//Decrypt and get fitness with parent key
				parentDecrypt = polyalphabeticDecrypt(cipher, parentKey, false);
				parentFitness = fitness::tetragramFitness(&parentDecrypt);

				//Reset littleCounter
				littleCounter = 0;
				while (littleCounter < 1000) {
					//Try swapping two letters in the ith subkey
					childKey = parentKey;
					x = dist(gen);
					y = dist(gen);
					while (x == y) {
						y = dist(gen);
					}
					_ = childKey[i][x];
					#pragma warning( push )
					#pragma warning( disable : 28020 )
					childKey[i][x] = childKey[i][y]; 
					#pragma warning( pop )
					childKey[i][y] = _;

					//Decrypt with childKey and get fitness
					childDecrypt = polyalphabeticDecrypt(cipher, childKey, false);
					childFitness = fitness::tetragramFitness(&childDecrypt);

					//Inner loop and parentKey checks
					if (childFitness > parentFitness) {
						parentKey = childKey;
						parentDecrypt = childDecrypt;
						parentFitness = childFitness;
						littleCounter = 0;
					}

					//Outer loop and bestKey checks
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
					
					//Increment counters
					littleCounter++;
					bigCounter++;
				}
			}
		}

		std::ios_base::sync_with_stdio(true); //Resyncs std::cout to prevent issues elsewhere

		//Print best key
		for (int n = 0; n < keySize; n++) {
			std::cout << monoalphabetic::keyToString(monoalphabetic::invertKey(bestKey[n])) << std::endl;
		}

		return polyalphabeticDecrypt(cipher, bestKey, false);
	}

	//Overload for the above that determines keylength then calls the above
	std::string hillClimber(std::string cipher) {
		//Get key size
		int keySize = getKeySize(cipher);
		if (keySize == -1) {
			return "";
		}
		
		return hillClimber(cipher, keySize);
	}

	//CLI Interface code for the above
	int cliHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);

		std::string decrypt = hillClimber(cipher);

		if (decrypt == "") { //Only occurs if key length couldn't be determined
			return 0; //Failure
		}

		if (fitness::tetragramFitness(&decrypt) > -15) {
			if (cliInterface::offerDecryption(decrypt)) {
				return 1; //Success
			}
		}
		return 0; //Failure
	}
}