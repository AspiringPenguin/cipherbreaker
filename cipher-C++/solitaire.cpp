#include "solitaire.h"
#include <algorithm>

namespace solitaire {
	char combineCharacters(char p, int s) { //P between 97 and 122, S between 1 and 26
		char val = (p - 96) + s;
		if (val > 26) {
			val -= 26;
		}
		return val + 96;
	}

	char decombineCharacters(char p, int s) { //P between 97 and 122, S between 1 and 26
		char val = (p - 96) - s;
		if (val < 1) {
			val += 26;
		}
		return val + 96;
	}

	void moveJokerA(std::array<int, 54>& deck) {
		auto jokerA = std::distance(deck.begin(), std::find(deck.begin(), deck.end(), 53));
		if (jokerA == 53) { //Bottom of the deck -> index 1, need to move alot of cards now
			for (int i = 52; i > 0; i--) {
				deck[i + 1] = deck[i];
			}
			deck[1] = 53;
		}
		else { //Just swap with the card below
			deck[jokerA] = deck[jokerA + 1];
			deck[jokerA + 1] = 53;
		}
	}

	void moveJokerB(std::array<int, 54>& deck) {
		auto jokerB = std::distance(deck.begin(), std::find(deck.begin(), deck.end(), 54));
		if (jokerB == 53) { //Bottom of the deck -> index 2, need to move alot of cards now
			for (int i = 52; i > 1; i--) {
				deck[i + 1] = deck[i];
			}
			deck[2] = 54;
		}
		else if (jokerB == 52) { //1 before bottom of the deck -> index 1, need to move alot of cards now
			for (int i = 51; i > 0; i--) {
				deck[i + 1] = deck[i];
			}
			deck[1] = 54;
		}
		else { //Move down by two cards
			deck[jokerB] = deck[jokerB + 1];
			deck[jokerB + 1] = deck[jokerB + 2];
			deck[jokerB + 2] = 54;
		}

	}

	void jokerTripleCut(std::array<int, 54>& deck) {
		std::array<int, 54> buffer{ -1 };
		auto jokerA = std::distance(deck.begin(), std::find(deck.begin(), deck.end(), 53));
		auto jokerB = std::distance(deck.begin(), std::find(deck.begin(), deck.end(), 54));
		auto first = jokerA > jokerB ? jokerB : jokerA;
		auto firstVal = jokerA > jokerB ? 54 : 53;
		auto second = jokerA > jokerB ? jokerA : jokerB;
		auto secondVal = jokerA > jokerB ? 53 : 54;

		std::copy(deck.begin() + second + 1, deck.end(), buffer.begin()); //Copy after second joker to start of new deck
		std::copy(deck.begin() + first, deck.begin() + second + 1, buffer.begin() + 53 - second); //Copy middle section
		std::copy(deck.begin(), deck.begin() + first, buffer.begin() + 53 - first + 1); //Copy front of old to end of new

		deck = buffer;
	}

	void bottomCountCut(std::array<int, 54>& deck) {
		int val = deck[53];
		if (val > 52) { //A Joker
			return;
		}
		//Else its a normal card 1-52
		std::array<int, 54> buffer{ -1 };

		std::copy(deck.begin(), deck.begin() + val, buffer.end() - val - 1); //Copy the section being moved
		buffer[53] = deck[53]; //Move the bottom card
		std::copy(deck.begin() + val, deck.end() - 1, buffer.begin()); //Move the rest

		deck = buffer;
	}

	int getKeyStreamNum(std::array<int, 54>& deck) {
		int val = deck[deck[0]];
		if (val > 52) { //Joker
			return -1; //So skip
		}
		return val % 26;
	}

	int getNextKeyStreamNum(std::array<int, 54>& deck) {
		int val = -1;
		while (val == -1) {
			moveJokerA(deck);
			moveJokerB(deck);
			jokerTripleCut(deck);
			bottomCountCut(deck);
			val = getKeyStreamNum(deck);
		}
		return val;
	}

	std::string decyrpt(std::string cipher, std::array<int, 54> deck) {
		std::string plain = "";
		for (char c : cipher) {
			plain += decombineCharacters(c, getNextKeyStreamNum(deck));
		}
		return plain;
	}
}