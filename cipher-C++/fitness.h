#pragma once
#include <array>
#include <string>

namespace fitness {
	std::array<double, 26> monogramFrequencies(std::string text);
	std::array<double, 26> monogramFrequencies(std::string text, int divisor);
	double chi2(std::array<double, 26> expected, std::array<double, 26> actual);
	double chi2Fitness(std::string text);
	double dotProduct(std::array<double, 26> vec1, std::array<double, 26> vec2);
	double angleBetweenVectors(std::array<double, 26> vec1, std::array<double, 26> vec2);
	double angleBetweenVectorsFitness(std::string text);
	double tetragramFitness(std::string* text);
	double indexOfCoincidence(std::string text, int normalisationFactor);
	double indexOfCoincidence(std::string text);
	double indexOfCoincidencePeriodic(std::string text, int n, int normalisationFactor);
	double indexOfCoincidencePeriodic(std::string text, int n);
}