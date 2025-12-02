#pragma once
#include <string>
#include "strings.h"

namespace periodic {
	std::string vigenere(std::string cipher, int keyLen);
	std::string vigenere(std::string cipher);

	int cliVigenere(std::string cipher);

	std::string polyalphabeticDecrypt(std::string cipher, std::vector<std::array<char, 26>>, bool);

	std::string hillClimber(std::string cipher, int keySize);
	std::string hillClimber(std::string cipher);

	int cliHillClimber(std::string cipher);
}