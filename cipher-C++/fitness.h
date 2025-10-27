#pragma once
#include <array>
#include <string>

namespace fitness {
	std::array<int, 26> monogramFrequencies(std::string text);
	double chi2(std::array<double, 26> expected, std::array<double, 26> actual);
}