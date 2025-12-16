#pragma once
#include <array>
#include <string>

namespace solitaire {
	// 53 = Joker A
	// 54 = Joker B
	// For value of card purposes 54 -> 53
	// 0 For an unknown card if needed in an attack

	char combineCharacters(char p, int s);
	char decombineCharacters(char p, int s);

	void moveJokerA(std::array<int, 54>& deck); //Setps 1 and 2
	void moveJokerB(std::array<int, 54>& deck);

	void jokerTripleCut(std::array<int, 54>& deck); //Step 3

	void bottomCountCut(std::array<int, 54>& deck); //Step 4

	int getKeyStreamNum(std::array<int, 54>& deck);

	int getNextKeyStreamNum(std::array<int, 54>& deck);

	std::string decrypt(std::string cipher, std::array<int, 54> deck);

	std::array<int, 54> cliGetDeck();
}