#include "stream.h"

namespace stream {
	char combineChars(char p, char s, mode m) {
		char val = -1;
		switch (m) {
		case add:
			val = (p + s - 194) % 26;
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
}