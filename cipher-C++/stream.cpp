#include "stream.h"
#include <queue>

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
}