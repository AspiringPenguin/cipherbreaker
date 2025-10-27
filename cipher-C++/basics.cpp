#include "basics.h"

namespace basics {
	std::string formatString(std::string inp) {
		return "";
	}

	std::unordered_map<char, int> createAlphabetMap()
	{
		auto map = std::unordered_map<char, int>();

		for (int i = 0; i < 26; i++) {
			map[alphabet[i]] = i;
		}
		return map;
	}
}
