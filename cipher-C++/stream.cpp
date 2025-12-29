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
			val = (p + s - 194); //Add together and subtract the ascii offset (97, as we want a=0)*2 = 194 before doing mod 26
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
			val = (p - s);
			if (val < 0) {
				val += 26;
			}
			break;
		case subtractKey:
			val = (p + s - 194);
			if (val >= 26) {
				val -= 26;
			}
			break;
		case subtractText:
			val = (s - p);
			if (val < 0) {
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

	std::string autokeyDecrypt(std::string cipher, std::string key, mode m) {
		std::string plain = "";
		plain.reserve(cipher.length());
		std::queue<char> stream;
		for (char c : key) {
			stream.push(c);
		}

		char pChar;

		for (char c : cipher) {
			pChar = decombineChars(c, stream.front(), m);
			stream.pop(); //Have to read then remove from the queue
			stream.push(pChar);
			plain += pChar;
		}

		return plain;
	}

	std::string autokeyMiniHillClimber(std::string cipher, int l, mode m) {
		cipher = basics::formatString(cipher);

		std::string bestKey = "";

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 25);

		for (int i = 0; i < l; i++) {
			bestKey += basics::alphabet[dist(gen)];
		}

		std::string bestDecrypt = autokeyDecrypt(cipher, bestKey, m);
		float bestFitness = fitness::tetragramFitness(&bestDecrypt);

		std::string childKey = "";
		std::string childDecrypt = "";
		float childFitness = 0.0f;

		bool flag = false;
		
		while (!flag) {
			flag = true;
			for (int i = 0; i < l; i++) {
				childKey = bestKey;
				for (char x = 97; x < 123; x++) {
					childKey[i] = x;
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

	std::tuple<std::string, mode> autokeyHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		std::string result = "";
		std::string decrypt =  "";
		for (int l = 1; l < 21; l++) {
			for (mode m : {add, subtractKey, subtractText}) {
				result = autokeyMiniHillClimber(cipher, l, m);
				decrypt = autokeyDecrypt(cipher, result, m);
				if (fitness::tetragramFitness(&decrypt) > -15) {
					return { result, m };
				}
			}
		}
		return { "", add };
	}

	int cliAutokeyHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto res = autokeyHillClimber(cipher);
		if (std::get<0>(res) == "") {
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

	std::string progressiveVigenereAsVigenere(std::string cipher, int l, int prog) {
		std::string vigenere = "";
		for (int i = 0; i < cipher.length(); i++) {
			vigenere += decombineChars(cipher[i], (((i/l)*prog)%26)+97, add);
		}
		return vigenere;
	}

	std::string progressiveVigenereSubBruteForce(std::string cipher, int l, int prog) {
		return periodic::vigenereAsCaesarShifts(progressiveVigenereAsVigenere(cipher, l, prog), l);
	}

	std::string progressiveVigenereBruteForce(std::string cipher) {
		cipher = basics::formatString(cipher);
		for (int l = 2; l < 21; l++) {
			for (int prog = 1;prog < 26;prog++) {
				std::string text = progressiveVigenereSubBruteForce(cipher, l, prog);
				if (fitness::tetragramFitness(&text) > -15) {
					return text;
				}
			}
		}
		return "";
	}

	int cliProgressiveVigenereBruteForce(std::string cipher) {
		std::string decrypt = progressiveVigenereBruteForce(cipher);
		if (cliInterface::offerDecryption(decrypt)) {
			return 1; //Success
		}
		return 0; //Failure
	}
}