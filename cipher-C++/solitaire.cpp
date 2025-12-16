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
	}

	void bottomCountCut(std::array<int, 54>& deck) {
	}

	int generateKeyStreamNum(std::array<int, 54>& deck) {
		 return 0;
	}
}