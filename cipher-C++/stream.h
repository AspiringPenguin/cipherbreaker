#pragma once
#include <string>
#include <tuple>

namespace stream {
	enum mode {
		add, subtractKey, subtractText
	};

	char combineChars(char p, char s, mode m);
	char decombineChars(char p, char s, mode m);

	std::string autokeyDecrypt(std::string cipher, std::string key, mode m);
	std::string autokeyMiniHillClimber(std::string cipher, int l, mode m);
	std::tuple<std::string, mode> autokeyHillClimber(std::string cipher);
	int cliAutokeyHillClimber(std::string cipher);
}