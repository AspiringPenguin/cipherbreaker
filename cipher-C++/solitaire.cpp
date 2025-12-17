#include "solitaire.h"
#include <algorithm>
#include <iostream>

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

	int rem54(int val) {
		if (val == 54) {
			return 53;
		}
		return val;
	}

	int getKeyStreamNum(std::array<int, 54>& deck) {
		int val = deck[rem54(deck[0])];
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

	std::string decrypt(std::string cipher, std::array<int, 54> deck) {
		std::string plain = "";
		for (char c : cipher) {
			plain += decombineCharacters(c, getNextKeyStreamNum(deck));
		}
		return plain;
	}

	int charToNum(char c) {
		if (std::isdigit(c)) {
			return c - 48;
		}
		switch (c) {
		case 'A':
			return 1;
		case 'T':
			return 10;
		case 'J':
			return 11;
		case 'Q':
			return 12;
		case 'K':
			return 13;
		}
	}

	int stringToCardNum(std::string card) { //J1, J2, JC, JD, JH, JS, Q, K, A etc.
		std::transform(card.begin(), card.end(), card.begin(), ::toupper); //Avoid lowercase cases
		if (card[0] == '1' && card[1] == '0') { //Fix 10H for example
			card = std::string({ 'T', card[2]});
		}
		switch (card[1]) {
		case 'A': //Joker
			return 53;
		case 'B': //Also joker
			return 54;
		case 'C': //Clubs
			return charToNum(card[0]);
		case 'D': //Diamonds
			return charToNum(card[0]) + 13;
		case 'H': //Hearts
			return charToNum(card[0]) + 26;
		case 'S': //Spades
			return charToNum(card[0]) + 39;
		}
	}

	std::array<int, 54> cliGetDeck() {
		std::array<int, 54> deck{ -1, 54 };
		std::string input;
		int val;
		for (int i = 0; i < 54; i++) {
			std::cout << i << ". ";
			std::cin >> input;
			if (input == "?") {
				deck[i] = 0; //Unknown
			}
			else {
				val = stringToCardNum(input);
				deck[i] = val;
				std::cout << val << std::endl;
			}
		}
		return deck;
	}
}