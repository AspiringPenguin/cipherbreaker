#include "solitaire.h"

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
}