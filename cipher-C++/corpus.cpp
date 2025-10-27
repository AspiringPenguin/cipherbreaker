#include "corpus.h"

namespace corpus {
	std::array<double, 26> frequenciesAsArray(std::unordered_map<std::string, double> map) {
		std::array<double, 26> array;
		char c;
		for (int i = 0; i < 26; i++) {
			c = char(i + 97);
			array[i] = map[std::string(1, c)];
		}
		return array;
	}
}