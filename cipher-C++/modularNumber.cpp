#include "modularNumber.h"

modularNumber::modularNumber(int n) {
	value = n;
	modulus = 26;
}

modularNumber::modularNumber(int n, int m) {
	value = n;
	modulus = m;
}

modularNumber modularNumber::operator=(int& n) {
	value = n;
}

std::ostream& operator<<(std::ostream& stream, const modularNumber& mNum) {
	stream << mNum.value;
	return stream;
}