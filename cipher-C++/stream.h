#pragma once

namespace mode {
	enum mode {
		add, subtractKey, subtractText
	};

	char combineChars(char p, char s, mode mode);
	char decombineChars(char p, char s, mode mode);
}