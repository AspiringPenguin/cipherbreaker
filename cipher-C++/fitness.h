#pragma once
#include <array>

namespace fitness {
	double chi2(std::array<double, 26> expected, std::array<double, 26> actual);
}