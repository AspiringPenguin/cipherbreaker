#include "transpositions.h"

namespace transpositions{
	std::generator<std::vector<int>> heapsPerms(int n)
	{
		auto A = std::vector<int>();
		auto c = std::vector<int>();
		for (int i = 1; i <= n; i++) {
			A.push_back(i);
			c.push_back(0);
		}
		int i = 0;
		int _;
		co_yield A; //Unmodified array

		while (i < n) {
			if (c[i] < i) {
				if (i % 2 == 0) {
					_ = A[0];
					A[0] = A[i];
					A[i] = _;
				}
				else {
					_ = A[c[i]];
					A[c[i]] = A[i];
					A[i] = _;
				}
				co_yield A;
				c[i]++;
				i = 0;
			}
			else {
				c[i] = 0;
				i++;
			}
		}

		co_return;
	}
}
