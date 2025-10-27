#include "basics.h"
#include "corpus.h"
#include "fitness.h"
#include <iostream>

namespace fitness {
	std::array<double, 26> monogramFrequencies(std::string text) {
		std::array<double, 26> array = { 0 };
		int l = text.length();
		for (int i = 0; i < l; i++) {
			array[basics::alphabetIndex[text[i]]]++;
		}
		return array;
	}

	std::array<double, 26> monogramFrequencies(std::string text, int divisor) {
		std::array<double, 26> array = { 0 };
		int l = text.length();
		for (int i = 0; i < l; i++) {
			array[basics::alphabetIndex[text[i]]]++;
		}
		for (int i = 0; i < 26; i++) {
			array[i] = array[i] / divisor;
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

	double chi2Fitness(std::string text) {
		return chi2(monogramFrequencies(text, text.length()), corpus::frequencyArray);
	}
}
