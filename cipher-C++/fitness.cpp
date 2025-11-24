#include "basics.h"
#include "corpus.h"
#include "fitness.h"

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

	double dotProduct(std::array<double, 26> vec1, std::array<double, 26> vec2) {
		double tot = 0.0;
		for (int i = 0; i < 26; i++) {
			tot += vec1[i] * vec2[i];
		}
		return tot;
	}

	double angleBetweenVectors(std::array <double, 26 > vec1, std::array<double, 26> vec2) {
		return dotProduct(vec1, vec2) / sqrt(dotProduct(vec1, vec1) * dotProduct(vec2, vec2));
	}

	double angleBetweenVectorsFitness(std::string text) {
		return angleBetweenVectors(monogramFrequencies(text), corpus::frequencyArray);
	}

	double tetragramFitness(std::string* text) {
		double tot = 0;
		int n = text->length() - 3;
		for (int i = 0; i < n; i++) {
			tot += corpus::tetragramArrays[(text->at(i) - 97) * 17576 + (text->at(i+1) - 97) * 676 + (text->at(i+2) - 97) * 26 + (text->at(i+3) - 97)];
		}
		tot /= n;
		return tot;
	}

	double indexOfCoincidence(std::string text, int normalisationFactor) {
		unsigned long long l = text.length();
		auto freqs = monogramFrequencies(text);
		double tot = 0;
		for (int i = 0; i < 26; i++) {
			tot += (freqs[i] * (freqs[i] - 1)) / (l * (l - 1));
		}
		return tot * normalisationFactor;
	}

	double indexOfCoincidence(std::string text) {
		return indexOfCoincidence(text, 26);
	}
}