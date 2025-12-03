#pragma once
#include <array>
#include <string>

namespace fitness {
	std::array<float, 26> monogramFrequencies(std::string text);
	std::array<float, 26> monogramFrequencies(std::string text, int divisor);
	float chi2(std::array<float, 26> expected, std::array<float, 26> actual);
	float chi2Fitness(std::string text);
	float dotProduct(std::array<float, 26> vec1, std::array<float, 26> vec2);
	float angleBetweenVectors(std::array<float, 26> vec1, std::array<float, 26> vec2);
	float angleBetweenVectorsFitness(std::string text);
	float tetragramFitness(std::string* text);
	float indexOfCoincidence(std::string text, int normalisationFactor);
	float indexOfCoincidence(std::string text);
	float indexOfCoincidencePeriodic(std::string text, int n, int normalisationFactor);
	float indexOfCoincidencePeriodic(std::string text, int n);
}