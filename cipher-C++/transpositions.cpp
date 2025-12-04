#include "transpositions.h"

namespace transpositions{
	std::generator<std::vector<int>> heapsPerms(int size)
	{
		co_yield {1, 2, 3, 4, 5};
		co_yield{ 2, 1, 3, 4, 5 };
		co_return;
	}
}
