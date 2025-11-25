#include "basics.h"	
#include "fitness.h"
#include "interface.h"
#include "monoalphabetic.h"
#include "periodic.h"
#include "strings.h"
#include <iostream>

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
		cipher = basics::formatString(cipher);
		return "";
	}

	std::string hillClimber(std::string cipher) {
		int keySize = getKeySize(cipher);
		if (keySize == -1) {
			return "";
		}
		return hillClimber(cipher, keySize);
	}
}