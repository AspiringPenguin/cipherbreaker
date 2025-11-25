#include "basics.h"	
#include "fitness.h"
#include "interface.h"
#include "monoalphabetic.h"
#include "periodic.h"
#include "strings.h"

namespace periodic {
	std::string vignere(std::string cipher, int keyLen) {
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

	std::string vignere(std::string cipher)
	{
		for (int i = 2; i < 21; i++) {
			if (fitness::indexOfCoincidencePeriodic(cipher, i) > 0.06) {
				return vignere(cipher, i);
			}
		}
		return "";
	}

	int cliVignere(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto result = vignere(cipher);
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
}