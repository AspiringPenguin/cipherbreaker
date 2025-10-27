#include "fitness.h"
#include "basics.h"

namespace fitness {
	std::array<int, 26> monogramFrequencies(std::string text) {
		std::array<int, 26> array = { 0 };
		int l = text.length();
		for (int i = 0; i < l; i++) {
			array[basics::alphabetIndex[text[i]]]++;
		}
		return array;
	}

	double chi2(std::array<double, 26> actual, std::array<double, 26> expected) {
		double tot = 0.0;
		for (int i = 0; i < 26; i++) {
			tot += (pow(actual[i] - expected[i], 2) / expected[i]);
		}
		return tot;
	}
}
