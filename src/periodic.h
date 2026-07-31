#pragma once
#include <string>
#include "strings.h"

namespace periodic {
	//A set of tools for periodic substitution ciphers

	std::string vigenereAsCaesarShifts(std::string cipher, int keyLen);
	std::string vigenereAsCaesarShifts(std::string cipher);

	int cliVigenereAsCaesarShifts(std::string cipher);

	std::string polyalphabeticDecrypt(std::string cipher, std::vector<std::array<char, 26>>, bool);

	std::string hillClimber(std::string cipher, long keySize);
	std::string hillClimber(std::string cipher);

	int cliHillClimber(std::string cipher);
}