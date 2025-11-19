#include "basics.h"

namespace basics {
	std::string formatString(std::string inp) {
		std::string newString = "";
		for (char c : inp) {
			c = tolower(c);
			if (std::find(alphabet.begin(), alphabet.end(), c) != alphabet.end()) {
				newString += c;
			}
		}
		return newString;
	}

	std::unordered_map<char, int> createAlphabetMap()
	{
		auto map = std::unordered_map<char, int>();

		for (int i = 0; i < 26; i++) {
			map[alphabet[i]] = i;
		}
		return map;
	}

	int lcm(int a, int b)
	{
		return a * b / hcf(a, b);
	}

	int hcf(int a, int b)
	{
		int _;
		while (b != 0) {
			a = a % b;
			_ = a;
			a = b;
			b = _;
		}

		return a;
	}

	bool coprime(int a, int b)
	{
		return hcf(a, b) == 1;
	}
}
