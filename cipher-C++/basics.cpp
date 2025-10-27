#include "basics.h"

std::unordered_map<char, int> basics::createAlphabetMap()
{
	auto map = std::unordered_map<char, int>();

	for (int i = 0; i < 26; i++) {
		map[alphabet[i]] = i;
	}
	return map;
}
