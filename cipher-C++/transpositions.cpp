#include "basics.h"
#include "fitness.h"
#include "strings.h"
#include "interface.h"
#include "transpositions.h"
#include <algorithm>
#include <cmath>
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

	std::string columnarDecrypt(std::string cipher, std::vector<int> key) { //Doesn't work
		//Get keyLen and number of columns that are shorter
		int keyLen = key.size();
		int cipherLen = cipher.length();
		int numGaps = keyLen - (cipherLen % keyLen);
		if (numGaps == keyLen) {
			numGaps = 0;
		}

		//Get column lengths
		float fColumnLen = static_cast<float>(cipherLen) / keyLen;
		#pragma warning(push)
		#pragma warning(disable:4244) //Intentional behaviour to narrow cast
		int columnLen = fColumnLen;
		#pragma warning(pop)
		if (std::fmod(fColumnLen, 1) != 0) {
			columnLen++;
		}

		//Find blocks which need extra spaces
		std::vector<int> needsGaps = std::vector<int>();
		for (int i = 0; i < keyLen; i++) {
			if (key[i] >= (keyLen - numGaps)) {
				needsGaps.push_back(i);
			}
		}
		std::sort(needsGaps.begin(), needsGaps.end());

		for (int i = 0; i < numGaps; i++) {
			cipher = cipher.substr(0, columnLen * (needsGaps[i] + 1) - 1) + " " + cipher.substr(columnLen * (needsGaps[i] + 1) - 1);
		}

		std::vector<std::string> blocks = strings::getBlocks(cipher, columnLen);
		auto newBlocks = std::vector<std::string>();
		for (const int& index : key) {
			newBlocks.push_back(blocks[index]);
		}

		std::string plain = "";
		for (int x = 0; x < columnLen; x++) {
			for (int y = 0; y < keyLen; y++) {
				plain += newBlocks[y][x];
			}
		}

		return basics::removeSpaces(plain);
	}

	std::string columnarBruteForce(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto newCols = std::vector<std::string>();
		std::vector<std::string> cols;
		std::string decrypt;

		for (int n = 2; n < 21; n++) {
			std::cout << n << std::endl;
			for (const auto& perm : heapsPerms(n)) {
				decrypt = columnarDecrypt(cipher, perm);
				if (fitness::tetragramFitness(&decrypt) > -15) {
					return decrypt;
				}
			}
		}
		return "";
	}

	int cliColumnarBruteForce(std::string cipher) {
		auto decrypt = columnarBruteForce(cipher);
		if (decrypt != "") {
			if (cliInterface::offerDecryption(decrypt)) {
				return 1;
			}
		}
		return 0;
	}
}
