#pragma once
#include <generator>
#include <vector>

namespace transpositions {
	std::generator<std::vector<int>> heapsPerms(int size);
}