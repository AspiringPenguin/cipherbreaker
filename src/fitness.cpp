#include "basics.h"
#include "corpus.h"
#include "fitness.h"
#include "strings.h"
#include <cmath>

namespace fitness {
	//Count all the characters a-z in a string and return them as a s6-item array with frequencies in alphabetical order
	std::array<float, 26> monogramFrequencies(std::string text) {
		std::array<float, 26> array = { 0 };
		int l = text.length();
		for (int i = 0; i < l; i++) {
			array[text[i] - 97]++;
		}
		return array;
	}

	//Count all the characters a-z in a string, divide each total by the divisor and return them as a s6-item array with frequencies in alphabetical order
	std::array<float, 26> monogramFrequencies(std::string text, int divisor) {
		std::array<float, 26> array = { 0 };
		int l = text.length();
		for (int i = 0; i < l; i++) {
			array[text[i] - 97]++;
		}
		for (int i = 0; i < 26; i++) {
			array[i] = array[i] / divisor;
		}
		return array;
	}

	//Calculate chi2 for two 26-dimensional vectors
	float chi2(std::array<float, 26> actual, std::array<float, 26> expected) {
		float tot = 0.0;
		for (int i = 0; i < 26; i++) {
			tot += (powf(actual[i] - expected[i], 2) / expected[i]);
		}
		return tot;
	}

	//Monogram fitness using chi2 between the text frequencies and the corpus frequencies.
	float chi2Fitness(std::string text) {
		return chi2(monogramFrequencies(text, text.length()), corpus::frequencyArray);
	}

	//Calculate dot produce of two 26-dimensional vectors
	float dotProduct(std::array<float, 26> vec1, std::array<float, 26> vec2) {
		float tot = 0.0;
		for (int i = 0; i < 26; i++) {
			tot += vec1[i] * vec2[i];
		}
		return tot;
	}
	
	//Using the formula for cosine of the angle between two vectors, find it for two 26d vectors
	float angleBetweenVectors(std::array <float, 26 > vec1, std::array<float, 26> vec2) {
		return dotProduct(vec1, vec2) / sqrt(dotProduct(vec1, vec1) * dotProduct(vec2, vec2));
	}


	//Use cosine of the angle between vectors compare monogram frequencies between the text and the corpus
	float angleBetweenVectorsFitness(std::string text) {
		return angleBetweenVectors(monogramFrequencies(text), corpus::frequencyArray);
	}

	//Tetragram fitness function. This sums the logarithms of the corpus frequencies of the tetragrams found in the input text and divides it by (the length of the text - 3)
	//To reduce random memory accesses, the tetragrams logarithms are stored as precaluclated pairs for quintgrams,
	//while the original table is only used if there are an odd number of tetragrams in the text.
	float tetragramFitness(std::string* text) {
		float tot = 0;
		int n = text->length() - 3;
		int l = n - 1;
		bool flag = (n % 2) != 0; //Needs a final tetragram
		for (int i = 0; i < l; i+=2) {
			tot += corpus::combinedTetragrams[(text->at(i) - 97) * 456976 + (text->at(i + 1) - 97) * 17576 + (text->at(i + 2) - 97) * 676 + (text->at(i + 3) - 97) * 26 + (text->at(i + 4) - 97)];
		}
		if (flag) {
			tot += corpus::tetragramArrays[(text->at(n-1) - 97) * 17576 + (text->at(n) - 97) * 676 + (text->at(n + 1) - 97) * 26 + (text->at(n + 2) - 97)];
		}
		tot /= n;
		return tot;
	}
	
	//Calculate index of coincidence, multiplied by a normalisation factor.
	//Some people like that as 26, but I am used to having it as 1, where IoC of English roughly = 0.066
	float indexOfCoincidence(std::string text, int normalisationFactor) {
		unsigned long long l = text.length();
		auto freqs = monogramFrequencies(text);
		float tot = 0;
		for (int i = 0; i < 26; i++) {
			tot += (freqs[i] * (freqs[i] - 1)) / (l * (l - 1));
		}
		return tot * normalisationFactor;
	}

	//An overload for index of coincidence that defaults the normalisation factor to 1
	float indexOfCoincidence(std::string text) {
		return indexOfCoincidence(text, 1);
	}

	//Index of coincidence by period - splitting the text into blocks of a given length and arranging on top of each other, as in strings::getColumns
	//Then taking the mean index of coincidence of each column
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

	//Same as the above but noramlisationFactor=1
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

	//Calculates the index of coincidence for bigrams
	float indexOfCoincidenceBigrams(std::string text) {
		text = basics::formatString(text);
		float tot = 0;
		int count;
		int len = text.length();
		int subLen = len / 2;

		for (char c = 97; c < 123; c++) {
			for (char d = 97; d < 123; d++) {
				count = 0;
				for (int i = 1; i < len; i+=2) {
					if (text[i - 1] == c && text[i] == d) {
						count++;
					}
				}
				tot += (count * (count - 1.0f)) / (subLen * (subLen - 1.0f));
			}
		}

		return tot;
	}

	//Calculates the index of coincidence for trigrams
	float indexOfCoincidenceTrigrams(std::string text) {
		text = basics::formatString(text);
		float tot = 0;
		int count;
		int len = text.length();
		int subLen = len / 3;

		for (char c = 97; c < 123; c++) {
			for (char d = 97; d < 123; d++) {
				for (char e = 97; e < 123; e++) {
					count = 0;
					for (int i = 2; i < len; i += 3) {
						if (text[i - 2] == c && text[i - 1] == d && text[i] == e) {
							count++;
						}
					}
					tot += (count * (count - 1.0f)) / (subLen * (subLen - 1.0f));
				}
			}
		}

		return tot;
	}
}