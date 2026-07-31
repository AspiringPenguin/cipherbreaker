#pragma once
#include <string>
#include <tuple>

namespace stream {
	//Some tools and attacks on stream ciphers
	//Tools for the solitaire cipher can be found in solitaire.cpp

	enum mode {
		add, subtractKey, subtractText, solitaire //solitaire generally isn't used here but it is implemented in case of need
	};

	char combineChars(char p, char s, mode m);
	char decombineChars(char p, char s, mode m);

	std::string autokeyDecrypt(std::string cipher, std::string key, mode m);
	std::string autokeyMiniHillClimber(std::string cipher, int l, mode m);
	std::tuple<std::string, mode> autokeyHillClimber(std::string cipher);
	int cliAutokeyHillClimber(std::string cipher);

	std::string progressiveVigenereAsVigenere(std::string cipher, int l, int prog);
	std::string progressiveVigenereSubBruteForce(std::string cipher, int l, int prog);
	std::string progressiveVigenereBruteForce(std::string cipher);
	int cliProgressiveVigenereBruteForce(std::string cipher);
}