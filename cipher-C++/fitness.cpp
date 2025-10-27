#include "fitness.h"

namespace fitness {
	double chi2(std::array<double, 26> actual, std::array<double, 26> expected) {
		double tot = 0.0;
		for (int i = 0; i < 26; i++) {
			tot += (pow(actual[i] - expected[i], 2) / expected[i]);
		}
		return tot;
	}
}
