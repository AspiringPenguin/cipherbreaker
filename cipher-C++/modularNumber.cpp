#include "modularNumber.h"

void modularNumber::update() {
	value = value % modulus;
}

modularNumber::modularNumber(int n) {
	value = n;
	modulus = 26;
	update();
}

modularNumber::modularNumber(int n, int m) {
	value = n;
	modulus = m;
	update();
}

modularNumber modularNumber::operator=(int& n) {
	value = n;
	update();
}

modularNumber modularNumber::operator+(int& n) {
	return modularNumber(value + n);
}

modularNumber modularNumber::operator+(modularNumber& n) {
	return modularNumber(value + n.value);
}

modularNumber modularNumber::operator -(int& n)
{
	return modularNumber(value - n);
}

modularNumber modularNumber::operator -(modularNumber& n)
{
	return modularNumber(value - n.value);
}

std::ostream& operator<<(std::ostream& stream, const modularNumber& mNum) {
	stream << mNum.value;
	return stream;
}