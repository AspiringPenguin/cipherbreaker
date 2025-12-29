#include "solitaire.h"
#include <algorithm>
#include <iostream>

namespace solitaire {
	//Combine a character and a number from the deck solitaire style (A=1, ..., Z=26, range 1-26)
	//P from text between 97 and 122, S between 1 and 26 from deck
	char combineCharacters(char p, int s) { 
		char val = (p - 96) + s;
		if (val > 26) {
			val -= 26;
		}
		return val + 96;
	}

	//Decombine a character and a number from the deck solitaire style (A=1, ..., Z=26, range 1-26)
	//P from text between 97 and 122, S between 1 and 26 from deck
	char decombineCharacters(char p, int s) { 
		char val = (p - 96) - s;
		if (val < 1) {
			val += 26;
		}
		return val + 96;
	}

	//Step 1 of generating a keystream letter
	void moveJokerA(std::array<int, 54>& deck) { //Reference for deck to avoid copies where possible
		auto jokerA = std::distance(deck.begin(), std::find(deck.begin(), deck.end(), 53));
		if (jokerA == 53) { //Bottom of the deck -> index 1
			for (int i = 52; i > 0; i--) { //Move every card from indices 52, 51, ..., 2, 1 down a place
				deck[i + 1] = deck[i];
			}
			deck[1] = 53; //Put joker A at index 1
		}
		else { //Just swap with the card below
			deck[jokerA] = deck[jokerA + 1];
			deck[jokerA + 1] = 53;
		}
	}

	//Step 2 of generating a keystream letter
	void moveJokerB(std::array<int, 54>& deck) {
		auto jokerB = std::distance(deck.begin(), std::find(deck.begin(), deck.end(), 54));
		if (jokerB == 53) { //Bottom of the deck -> index 2, need to move alot of cards now
			for (int i = 52; i > 1; i--) { //Move every card from indices 52, 51, ..., 3, 2 down a place
				deck[i + 1] = deck[i];
			}
			deck[2] = 54; //Put joker B at index 2
		}
		else if (jokerB == 52) { //1 before bottom of the deck -> index 1, need to move alot of cards now
			for (int i = 51; i > 0; i--) { //Move every card from indices 51, 50, ..., 2, 1 down a place
				deck[i + 1] = deck[i]; 
			}
			deck[1] = 54; //Put joker B at index 1
		}
		else { //Move down by two cards
			deck[jokerB] = deck[jokerB + 1];
			deck[jokerB + 1] = deck[jokerB + 2];
			deck[jokerB + 2] = 54;
		}

	}

	//Step 3 of generating a keystream letter
	void jokerTripleCut(std::array<int, 54>& deck) {
		std::array<int, 54> buffer{ -1 }; //To make new deck state in
		auto jokerA = std::distance(deck.begin(), std::find(deck.begin(), deck.end(), 53)); //Find joker A
		auto jokerB = std::distance(deck.begin(), std::find(deck.begin(), deck.end(), 54)); //Find joker B
		auto first = jokerA > jokerB ? jokerB : jokerA; //Get which one is first or last and their positions
		auto firstVal = jokerA > jokerB ? 54 : 53;
		auto second = jokerA > jokerB ? jokerA : jokerB;
		auto secondVal = jokerA > jokerB ? 53 : 54;

		std::copy(deck.begin() + second + 1, deck.end(), buffer.begin()); //Copy after second joker to start of new deck
		std::copy(deck.begin() + first, deck.begin() + second + 1, buffer.begin() + 53 - second); //Copy middle section to the next bit of the new deck
		std::copy(deck.begin(), deck.begin() + first, buffer.begin() + 53 - first + 1); //Copy from front to first joker to the new deck
		
		deck = buffer; //Set the value of deck to the value of buffer
	}

	//Step 4 of generating a keystream letter
	void bottomCountCut(std::array<int, 54>& deck) {
		int val = deck[53];
		if (val > 52) { //A Joker - skip
			return;
		}
		//Else its a normal card 1-52
		std::array<int, 54> buffer{ -1 };

		std::copy(deck.begin(), deck.begin() + val, buffer.end() - val - 1); //Copy the stack taken off the top
		buffer[53] = deck[53]; //Move the bottom card over
		std::copy(deck.begin() + val, deck.end() - 1, buffer.begin()); //Move the rest of the cards

		deck = buffer; //Set the value of deck to the value of buffer
	}

	//A quick method for dealing with the use of 54 as the value for joker B to differentiate them, when value should be 53
	int rem54(int val) { 
		if (val == 54) {
			return 53;
		}
		return val;
	}

	//Steps 5 and 6 - getting the keystream num
	int getKeyStreamNum(std::array<int, 54>& deck) {
		int val = deck[rem54(deck[0])]; //Take the card indexed by the top card
		if (val > 52) { //Joker
			return -1; //So skip
		}
		if (val > 26) {
			val -= 26;
		}
		return val;
	}

	//Combining steps 1 through 6 to a single function that accounts for jokers being skipped still
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

	//Decryption method for the solitaire cipher
	std::string decrypt(std::string cipher, std::array<int, 54> deck) {
		std::string plain = "";
		for (char c : cipher) {
			plain += decombineCharacters(c, getNextKeyStreamNum(deck));
		}
		return plain;
	}

	//Input of deck methods
	//Convert the A23456789TJQK to a number 1-13
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

	//Convert a card to a number
	int stringToCardNum(std::string card) { //JA, JB, JC, JD, JH, JS, Q, K, A etc.
		std::transform(card.begin(), card.end(), card.begin(), ::toupper); //Avoid lowercase cases
		if (card[0] == '1' && card[1] == '0') { //Fix 10H for example
			card = std::string({ 'T', card[2]});
		}
		switch (card[1]) { //Handle jokers and suites
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

	//CLI method to enter a whole deck and return it.
	std::array<int, 54> cliGetDeck() {
		std::array<int, 54> deck{ -1, 54 };
		std::string input;
		int val;
		for (int i = 0; i < 54; i++) {
			std::cout << i + 1 << ". ";
			std::cin >> input; //Get input
			if (input == "?") {
				deck[i] = 0; //Unknown
			}
			else {
				val = stringToCardNum(input); //Convert to a num and put in deck
				deck[i] = val;
			}
		}
		return deck;
	}
}