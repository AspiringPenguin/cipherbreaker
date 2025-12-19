#include "homophonic.h"
#include "basics.h"
#include "fitness.h"
#include "monoalphabetic.h"
#include <iostream>
#include <random>

namespace homophonic {
	std::string homophonicDecrypt(std::string cipher, std::unordered_map<char, char> key) {
		std::string plain = "";
		for (char c : cipher) {
			plain += key.at(c);
		}
		return plain;
	}

	std::unordered_map<char, char> homophonicHillClimber(std::string cipher) {
		cipher = basics::removeSpaces(cipher);

		//Random
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 25);
		std::uniform_int_distribution<> coin(0, 1);
		std::uniform_int_distribution<> die(1, 20);

		//Keys
		auto bestKey = std::unordered_map<char, char>();
		auto symbols = std::vector<char>();
		int nSymbols = 0;

		for (char c : cipher) {
			if (bestKey.contains(c)) {
				continue;
			}
			nSymbols++;
			symbols.push_back(c);
			bestKey[c] = dist(gen) + 97; 
		}

		std::uniform_int_distribution<> symbol(0, nSymbols-1);

		std::unordered_map<char, char> currentKey = bestKey;
		std::unordered_map<char, char> childKey;

		//Decrypts and fitness
		std::string bestDecrypt = homophonicDecrypt(cipher, bestKey);
		std::string currentDecrypt = bestDecrypt;
		std::string childDecrypt;

		float bestFitness = fitness::tetragramFitness(&bestDecrypt) * (2 - fitness::angleBetweenVectorsFitness(bestDecrypt));
		float currentFitness = bestFitness;
		float childFitness;

		int counter = 0;
		int impatience = 0;
		bool wandering = false;

		int a, b;
		char _;
		char ac, bc;

		while (counter < 1000000) {
			childKey = currentKey;

			if (coin(gen) == 0) { //Swap two symbols
				a = symbols[symbol(gen)];
				b = symbols[symbol(gen)];
				while (a == b) {
					b = symbols[symbol(gen)];
				}
				_ = childKey.at(a);
				childKey[a] = childKey.at(b);
				childKey[b] = _;
			}
			else { //Swap two letters
				a = dist(gen);
				b = dist(gen);
				while (a == b) {
					b = dist(gen);
				}
				ac = a + 97;
				bc = b + 97;
				for (char c : symbols) {
					if (childKey.at(c) == ac) {
						childKey.insert_or_assign(c, bc);
					}
					else if (childKey.at(c) == bc) {
						childKey.insert_or_assign(c, ac);
					}
				}
			}

			childDecrypt = homophonicDecrypt(cipher, childKey);
			childFitness = fitness::tetragramFitness(&childDecrypt) * (2 - fitness::angleBetweenVectorsFitness(childDecrypt));

			if (childFitness > bestFitness) {
				std::cout << childFitness << " " << counter << std::endl;

				counter = 0;
				impatience = 0;
				wandering = false;

				bestFitness = childFitness;
				currentFitness = childFitness;
				bestKey = childKey;
				currentKey = childKey;
				bestDecrypt = childDecrypt;
				currentDecrypt = childDecrypt;
			}
			else if (childFitness == bestFitness) {
				impatience = 0;
				wandering = false;

				currentFitness = childFitness;
				currentKey = childKey;
				currentDecrypt = childDecrypt;
			}
			else if (childFitness > currentFitness) {
				currentFitness = childFitness;
				currentKey = childKey;
				currentDecrypt = childDecrypt;
			}
			else if (counter > 1000 && coin(gen) == 1 && childFitness > (bestFitness - 15)) {
				wandering = true;

				currentFitness = childFitness;
				currentKey = childKey;
				currentDecrypt = childDecrypt;
			}

			counter++;
			if (wandering) {
				impatience++;
			}
			if (impatience > 2000) {
				impatience = 0;
				currentKey = bestKey;
				currentFitness = bestFitness;
				currentDecrypt = bestDecrypt;
				wandering = false;
			}
		}
		return bestKey;
	}

	std::string dualBetDecrypt(std::string cipher, std::array<char, 26> upper, std::array<char, 26> lower) {
		std::string plain = "";
		for (char c : cipher) {
			if (std::isupper(c)) {
				plain += upper[c - 65];
			}
			else {
				plain += lower[c - 97];
			}
		}
		return plain;
	}

	std::tuple<std::array<char, 26>, std::array<char, 26>> dualBetHillClimber(std::string cipher) {
		cipher = basics::removeSpaces(cipher);

		//Random
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 25);
		std::uniform_int_distribution<> coin(0, 1);
		std::uniform_int_distribution<> die(1, 20);
		
		std::array<char, 26> bestUpperKey = monoalphabetic::stringToKey("abcdefghijklmnopqrstuvwxyz");
		std::array<char, 26> bestLowerKey = monoalphabetic::stringToKey("zyxwvutsrqponmlkjihgfedcba");
		std::array<char, 26> currentUpperKey = bestUpperKey;
		std::array<char, 26> currentLowerKey = bestLowerKey;
		std::array<char, 26> childUpperKey;
		std::array<char, 26> childLowerKey;

		std::string bestDecrypt = dualBetDecrypt(cipher, bestUpperKey, bestLowerKey);
		std::string currentDecrypt = bestDecrypt;
		std::string childDecrypt;

		float bestFitness = fitness::tetragramFitness(&bestDecrypt);
		float currentFitness = bestFitness;
		float childFitness;

		int counter = 0;
		int impatience = 0;
		bool wandering = false;

		char a, b, _;

		while (counter < 1000000) {
			childUpperKey = currentUpperKey;
			childLowerKey = currentLowerKey;

			a = dist(gen);
			b = dist(gen);

			if (coin(gen) == 0) {
				_ = childUpperKey[a];
				childUpperKey[a] = childUpperKey[b];
				childUpperKey[b] = _;
			}
			else {
				_ = childLowerKey[a];
				childLowerKey[a] = childLowerKey[b];
				childLowerKey[b] = _;
			}

			childDecrypt = dualBetDecrypt(cipher, childUpperKey, childLowerKey);
			childFitness = fitness::tetragramFitness(&childDecrypt);

			if (childFitness > bestFitness) {
				std::cout << childFitness << " " << counter << std::endl;

				counter = 0;
				impatience = 0;
				wandering = false;

				bestFitness = childFitness;
				currentFitness = childFitness;
				bestUpperKey = childUpperKey;
				bestLowerKey = childLowerKey;
				currentUpperKey = childUpperKey;
				currentLowerKey = childLowerKey;
				bestDecrypt = childDecrypt;
				currentDecrypt = childDecrypt;
			}
			else if (childFitness == bestFitness) {
				impatience = 0;
				wandering = false;

				currentFitness = childFitness;
				currentUpperKey = childUpperKey;
				currentLowerKey = childLowerKey;
				currentDecrypt = childDecrypt;
			}
			else if (childFitness > currentFitness) {
				currentFitness = childFitness;
				currentUpperKey = childUpperKey;
				currentLowerKey = childLowerKey;
				currentDecrypt = childDecrypt;
			}
			else if (counter > 1000 && die(gen) < 5 && childFitness > (bestFitness - 5)) {
				wandering = true;

				currentFitness = childFitness;
				currentUpperKey = childUpperKey;
				currentLowerKey = childLowerKey;
				currentDecrypt = childDecrypt;
			}

			counter++;
			if (wandering) {
				impatience++;
			}
			if (impatience > 2000) {
				impatience = 0;
				currentUpperKey = bestUpperKey;
				currentLowerKey = bestLowerKey;
				currentFitness = bestFitness;
				currentDecrypt = bestDecrypt;
				wandering = false;
			}

			counter++;
		}
		
		return { bestUpperKey, bestLowerKey };
	}
}