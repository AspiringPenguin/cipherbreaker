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

	std::vector<double> tetragramsAsArray(std::unordered_map<std::string, double> map) {
		auto arrays = std::vector<double>();
		for (char i = 0; i < 26; i++) {
			for (char j = 0; j < 26; j++) {
				for (char k = 0; k < 26; k++) {
					for (char l = 0; l < 26; l++) {
						arrays.push_back(map[{static_cast<char>(i + 97), static_cast<char>(j + 97), static_cast<char>(k + 97), static_cast<char>(l + 97)}]);
					}
				}
			}
		}
		return arrays;
	}
}