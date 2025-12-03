#include "basics.h"
#include "corpus.h"
#include "fitness.h"
#include "strings.h"

namespace fitness {
	std::array<float, 26> monogramFrequencies(std::string text) {
		std::array<float, 26> array = { 0 };
		int l = text.length();
		for (int i = 0; i < l; i++) {
			array[basics::alphabetIndex[text[i]]]++;
		}
		return array;
	}

	std::array<float, 26> monogramFrequencies(std::string text, int divisor) {
		std::array<float, 26> array = { 0 };
		int l = text.length();
		for (int i = 0; i < l; i++) {
			array[basics::alphabetIndex[text[i]]]++;
		}
		for (int i = 0; i < 26; i++) {
			array[i] = array[i] / divisor;
		}
		return array;
	}

	float chi2(std::array<float, 26> actual, std::array<float, 26> expected) {
		float tot = 0.0;
		for (int i = 0; i < 26; i++) {
			tot += (pow(actual[i] - expected[i], 2) / expected[i]);
		}
		return tot;
	}

	float chi2Fitness(std::string text) {
		return chi2(monogramFrequencies(text, text.length()), corpus::frequencyArray);
	}

	float dotProduct(std::array<float, 26> vec1, std::array<float, 26> vec2) {
		float tot = 0.0;
		for (int i = 0; i < 26; i++) {
			tot += vec1[i] * vec2[i];
		}
		return tot;
	}

	float angleBetweenVectors(std::array <float, 26 > vec1, std::array<float, 26> vec2) {
		return dotProduct(vec1, vec2) / sqrt(dotProduct(vec1, vec1) * dotProduct(vec2, vec2));
	}

	float angleBetweenVectorsFitness(std::string text) {
		return angleBetweenVectors(monogramFrequencies(text), corpus::frequencyArray);
	}

	float tetragramFitness(std::string* text) {
		float tot = 0;
		int n = text->length() - 3;
		for (int i = 0; i < n; i++) {
			tot += corpus::tetragramArrays[(text->at(i) - 97) * 17576 + (text->at(i+1) - 97) * 676 + (text->at(i+2) - 97) * 26 + (text->at(i+3) - 97)];
		}
		tot /= n;
		return tot;
	}

	float indexOfCoincidence(std::string text, int normalisationFactor) {
		unsigned long long l = text.length();
		auto freqs = monogramFrequencies(text);
		float tot = 0;
		for (int i = 0; i < 26; i++) {
			tot += (freqs[i] * (freqs[i] - 1)) / (l * (l - 1));
		}
		return tot * normalisationFactor;
	}

	float indexOfCoincidence(std::string text) {
		return indexOfCoincidence(text, 1);
	}

	float indexOfCoincidencePeriodic(std::string text, int n, int normalisationFactor) {
		auto cols = strings::getColumns(text, n);
		float avg = 0;
		for (std::string col : cols) {
			if (col.length() != 0) {
				avg += indexOfCoincidence(col, normalisationFactor);
			}
		}
		avg /= n;
		return avg;
	}

	float indexOfCoincidencePeriodic(std::string text, int n) {
		auto cols = strings::getColumns(text, n);
		float avg = 0;
		for (std::string col : cols) {
			if (col.length() != 0) {
				avg += indexOfCoincidence(col);
			}
		}
		avg /= n;
		return avg;
	}
}