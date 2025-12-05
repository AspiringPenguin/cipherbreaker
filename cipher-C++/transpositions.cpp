#include "basics.h"
#include "fitness.h"
#include "strings.h"
#include "interface.h"
#include "transpositions.h"
#include <iostream>

namespace transpositions{
	std::generator<std::vector<int>> heapsPerms(int n)
	{
		auto A = std::vector<int>();
		auto c = std::vector<int>();
		for (int i = 0; i < n; i++) {
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

	std::string permutationDecrypt(std::string cipher, std::vector<int> key) { //Duplicated in permutationBruteForce to reuse objects
		auto cols = strings::getColumns(cipher, key.size());
		auto newCols = std::vector<std::string>();
		for (const auto& i : key) {
			newCols.push_back(cols[i]);
		}
		return strings::columnsToString(newCols);
	}

	std::string permutationBruteForce(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto newCols = std::vector<std::string>();
		std::vector<std::string> cols;
		std::string decrypt;

		for (int n = 2; n < 21; n++) {
			std::cout << n << std::endl;
			cols = strings::getColumns(cipher, n);
			for (const auto& perm : heapsPerms(n)) {
				newCols.clear();
				for (const auto& i : perm) {
					newCols.push_back(cols[i]);
				}
				decrypt = strings::columnsToString(newCols);
				if (fitness::tetragramFitness(&decrypt) > -15) {
					return decrypt;
				}
			}
		}
		return "";
	}

	int cliPermutationBruteForce(std::string cipher) {
		auto decrypt = permutationBruteForce(cipher);
		if (decrypt != "") {
			if (cliInterface::offerDecryption(decrypt)) {
				return 1;
			}
		}
		return 0;
	}
}
