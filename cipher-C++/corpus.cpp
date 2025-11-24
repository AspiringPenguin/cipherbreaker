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

	std::vector<std::vector<std::vector<std::vector<double>>>> tetragramsAsArray(std::unordered_map<std::string, double> map) {
		auto arrays = std::vector<std::vector<std::vector<std::vector<double>>>>();
		auto l4 = std::vector<double>();
		auto l3 = std::vector<std::vector<double>>();
		auto l2 = std::vector<std::vector<std::vector<double>>>();
		for (char i = 0; i < 26; i++) {
			for (char j = 0; j < 26; j++) {
				for (char k = 0; k < 26; k++) {
					for (char l = 0; l < 26; l++) {
						l4.push_back(map[{static_cast<char>(i + 97), static_cast<char>(j + 97), static_cast<char>(k + 97), static_cast<char>(l + 97)}]);
					}
					l3.push_back(l4);
					l4.clear();
				}
				l2.push_back(l3);
				l3.clear();
			}
			arrays.push_back(l2);
			l2.clear();
		}
		return arrays;
	}
}