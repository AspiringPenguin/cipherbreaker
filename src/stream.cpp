#include "stream.h"

#include "basics.h"
#include "fitness.h"
#include "interface.h"
#include "periodic.h"
#include <iostream>
#include <queue>
#include <random>

namespace stream {
	//Combine two chars 97-122 using a given mode (a=97, z=122)
	char combineChars(char p, char s, mode m) {
		char val = -1;
		switch (m) {
		case add:
			val = (p + s - 194); //Add together and subtract the ascii offset (97, as we want a=0)*2 = 194
			if (val >= 26) { //Deal with values >= 26
				val -= 26;
			}
			break;
		case subtractKey:
			val = (p - s); //Subtract, which deals with the ascii offset as they cancel out
			if (val < 0) { //Deal with negatives
				val += 26;
			}
			break;
		case subtractText:
			val = (s - p); //Subtract, which deals with the ascii offset as they cancel out
			if (val < 0) { //Deal with negatives
				val += 26;
			}
			break;
		case solitaire:
			char pConv = p - 96; //Convert to soliaire form A=1, Z=26 etc.
			char sConv = s - 96;
			val = pConv + sConv; //Add together
			if (val > 26) { //Subtract 26 if greater than 26
				val -= 26;
			}
			val--; //Convert to a form that can have 97 added to be correct
			break;
		}
		return 97 + val;
	}

	//Decombine two chars 97-122 using a given mode (a=97, z=122)
	char decombineChars(char p, char s, mode m) {
		char val = -1;
		switch (m) {
		case add:
			val = (p - s); //Do the reverse and substract the stream character, which deals with the ascii offset as they cancel out
			if (val < 0) { //Deal with negatives
				val += 26;
			}
			break;
		case subtractKey:
			val = (p + s - 194); //Do the inverse and back the stream character, subtract the ascii offset (97, as we want a=0)*2 = 194
			if (val >= 26) { //Deal with values >= 26
				val -= 26;
			}
			break;
		case subtractText: 
			val = (s - p); //Do the inverse and subtract the stream character, which works because we are dealing with mod 26, which deals with the ascii offset as they cancel out
			if (val < 0) { //Deal with negative values
				val += 26;
			}
			break;
		case solitaire:
			char pConv = p - 96; //Convert to soliaire form A=1, Z=26 etc.
			char sConv = s - 96;
			val = pConv - sConv;
			if (val < 1) { //Add 26 to remove negative results
				val += 26;
			}
			val--; //Convert to a form that can have 97 added to be correct
			break;
		}
		return 97 + val;
	}

	//Decryption routine for the autokey cipher for a given key and mode of operation
	std::string autokeyDecrypt(std::string cipher, std::string key, mode m) {
		std::string plain = "";
		plain.reserve(cipher.length()); //Make the plain big enough to store the decrypt with allocating more memory on the fly
		std::queue<char> stream; //A queue for the stream so characters can easily be added at the back and removed at the from without any complex operations
		for (char c : key) { //Put the key into the stream queue
			stream.push(c);
		}

		char pChar;

		for (char c : cipher) {
			pChar = decombineChars(c, stream.front(), m);
			stream.pop(); //Have to read then remove from the queue
			stream.push(pChar); //Add the plaintext char to the queue
			plain += pChar;
		}

		return plain;
	}

	//Mini hillclimber for autokey cipher for a given keyLength and mode of operation
	//Uses the "check all key changes" setup
	std::string autokeyMiniHillClimber(std::string cipher, int l, mode m) {
		cipher = basics::formatString(cipher);


		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 25);

		//Generate a random bestKey
		std::string bestKey = "";
		for (int i = 0; i < l; i++) {
			bestKey += (dist(gen) + 97);
		}

		//Decrypt and get fitness using the bestKey
		std::string bestDecrypt = autokeyDecrypt(cipher, bestKey, m);
		float bestFitness = fitness::tetragramFitness(&bestDecrypt);

		//Child values
		std::string childKey = "";
		std::string childDecrypt = "";
		float childFitness = 0.0f;

		//Loop control
		bool flag = false;
		
		while (!flag) {
			flag = true;
			for (int i = 0; i < l; i++) { //For pos in key
				childKey = bestKey; //Copy bestKey
				for (char x = 97; x < 123; x++) { //For char a=97 -> z=122
					childKey[i] = x; //Change key

					//Redecrypt and get fitness
					childDecrypt = autokeyDecrypt(cipher, childKey, m);
					childFitness = fitness::tetragramFitness(&childDecrypt);

					if (childFitness > bestFitness) {
						bestFitness = childFitness;
						bestDecrypt = childDecrypt;
						bestKey = childKey;
						flag = false;
					}
				}
			}
		}
		return bestKey;
	}

	//The outer brute force element for the above, trying every mode and key length
	std::tuple<std::string, mode> autokeyHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		std::string result = "";
		std::string decrypt = "";
		for (int l = 1; l < 21; l++) { //For key length 1->20 inclusive
			for (mode m : {add, subtractKey, subtractText}) { //For each possible mode
				result = autokeyMiniHillClimber(cipher, l, m); //Run the hillclimber
				decrypt = autokeyDecrypt(cipher, result, m);
				if (fitness::tetragramFitness(&decrypt) > -15) {
					return { result, m };
				}
			}
		}
		return { "", add }; //Null result
	}

	int cliAutokeyHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto res = autokeyHillClimber(cipher);
		if (std::get<0>(res) == "") { //Skip null results
			return 0; //Failure
		}
		std::string decrypt = autokeyDecrypt(cipher, std::get<0>(res), std::get<1>(res));
		if (fitness::tetragramFitness(&decrypt) > -15) {
			if (cliInterface::offerDecryption(decrypt)) {
				return 1; //Success
			}
		}
		return 0; //Failure
	}

	//Convert a progressive vigenere cipher to a vigenere cipher for a given length and progression
	std::string progressiveVigenereAsVigenere(std::string cipher, int l, int prog) {
		std::string vigenere = "";
		for (int i = 0; i < cipher.length(); i++) {
			vigenere += decombineChars(cipher[i], (((i/l)*prog)%26)+97, add); // (i/l) is integer division, so always rounds down. 
			//Multiply by progression and take mod 26 before adding the ascii offset = 97 (a=97) to generate the char to 'decombine' from the text using addition mode
		}
		return vigenere;
	}

	//A routine to break a progressive vigenere cipher using the above code and the existing attack for a given progression index and progression length
	std::string progressiveVigenereSubBruteForce(std::string cipher, int l, int prog) {
		return periodic::vigenereAsCaesarShifts(progressiveVigenereAsVigenere(cipher, l, prog), l);
	}

	//A brute force attack trying every combination for the above
	std::string progressiveVigenereBruteForce(std::string cipher) {
		cipher = basics::formatString(cipher);
		for (int l = 2; l < 21; l++) { //For all progression lengths 2->20 inclusive
			for (int prog = 1; prog < 26; prog++) { //For all progression indexes 1->25
				std::string text = progressiveVigenereSubBruteForce(cipher, l, prog); //Try the length and indexes
				if (fitness::tetragramFitness(&text) > -15) {
					return text;
				}
			}
		}
		return ""; //Return a null value
	}

	int cliProgressiveVigenereBruteForce(std::string cipher) {
		std::string decrypt = progressiveVigenereBruteForce(cipher);
		if (decrypt != "") { //Skip null results
			if (cliInterface::offerDecryption(decrypt)) {
				return 1; //Success
			}
		}
		return 0; //Failure
	}
}