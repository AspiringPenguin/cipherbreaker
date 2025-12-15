#pragma once
#include <string>

namespace stream {
	enum mode {
		add, subtractKey, subtractText
	};

	char combineChars(char p, char s, mode m);
	char decombineChars(char p, char s, mode m);

	std::string autokeyDecrypt(std::string cipher, std::string key, mode m);
}