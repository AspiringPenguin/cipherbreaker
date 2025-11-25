#pragma once
#include <string>
#include "strings.h"

namespace periodic {
	std::string vignere(std::string cipher, int keyLen);
	std::string vignere(std::string cipher);

	int cliVignere(std::string cipher);
}