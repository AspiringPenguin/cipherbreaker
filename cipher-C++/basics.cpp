#include "basics.h"
#include <cmath>
#include <iostream>

namespace basics {
	//Turns a string to lowercase and removes all non-alphabetical characters
	std::string formatString(std::string inp) {
		std::string newString = "";
		for (char c : inp) {
			c = tolower(c);
			if (c >= 97 && c < 123) {
				newString += c;
			}
		}
		return newString;
	}

	//Removes spaces from a string
	std::string removeSpaces(std::string inp) {
		std::string newString = "";
		for (char c : inp) {
			if (c != ' ') {
				newString += c;
			}
		}
		return newString;
	}

	int lcm(int a, int b)
	{
		return a * b / hcf(a, b);
	}

	//Euclidean algorithm
	int hcf(int a, int b)
	{
		int _;
		while (b != 0) {
			a = a % b;
			_ = a;
			a = b;
			b = _;
		}

		return a;
	}

	bool coprime(int a, int b)
	{
		return hcf(a, b) == 1;
	}

	//Extended Euclidean algorithm
	int multiplicativeInverse(int n, int modulus) {
		int t = 0;
		int tPrime = 1;
		int r = modulus;
		int rPrime = n;
		int q;
		int _;

		while (rPrime != 0) {
			q = r / rPrime;
			_ = tPrime;
			tPrime = t - (q * tPrime);
			t = _;
			_ = rPrime;
			rPrime = r - (q * rPrime);
			r = _;
		}

		if (r > 1) {
			return -1;
		}
		if (t < 0) {
			t += modulus;
		}
		return t;
	}

	//Get all the factors of a number.
	//Just uses an exhaustive search <= the square root
	std::vector<int> getFactors(int n) {
		auto factors = std::vector<int>();
		int max = sqrt(n);
		for (int i = 1; i <= max; i++) {
			if ((n % i) == 0) {
				if (i == n / i) {
					factors.push_back(i);
				}
				else {
					factors.push_back(i);
					factors.push_back(n / i);
				}
			}
		}
		return factors;
	}
}
