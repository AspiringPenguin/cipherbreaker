#include "stream.h"

#include "basics.h"
#include "fitness.h"
#include "interface.h"
#include <queue>
#include <random>

namespace stream {
	char combineChars(char p, char s, mode m) {
		char val = -1;
		switch (m) {
		case add:
			val = (p + s - 194) % 26;
			if (val >= 26) {
				val -= 26;
			}
			break;
		case subtractKey:
			val = (p - s);
			if (val < 0) {
				val += 26;
			}
			break;
		case subtractText:
			val = (s - p);
			if (val < 0) {
				val += 26;
			}
			break;
		}
		return 97 + val;
	}

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
		}
		return 97 + val;
	}

	std::string autokeyDecrypt(std::string cipher, std::string key, mode m) {
		std::string plain = "";
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

		std::string childKey;
		std::string childDecrypt;
		float childFitness;

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
		std::string result;
		std::string decrypt;
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
		std::string decrypt = autokeyDecrypt(cipher, std::get<0>(res), std::get<1>(res));
		if (fitness::tetragramFitness(&decrypt) > -15) {
			if (cliInterface::offerDecryption(decrypt)) {
				return 1;
			}
		}
		return 0;
	}
}