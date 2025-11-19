#pragma once
#include <iostream>

class modularNumber {
	int value;
	int modulus;

	void update();

public:
	modularNumber();
	modularNumber(int n);
	modularNumber(int n, int m);
	void operator=(int& n);

	modularNumber operator+(int& n);
	modularNumber operator+(modularNumber& n);

	void operator+=(int n);
	void operator+=(modularNumber n);

	modularNumber operator - ();
	modularNumber operator - (int& n);
	modularNumber operator - (modularNumber& n);

	modularNumber operator*(int& n);
	modularNumber operator*(modularNumber& n);

	operator int() {
		return value;
	}

	//For output, weird, I don't fully get how this works (friend means it can get private values)
	friend std::ostream& operator<< (std::ostream& stream, const modularNumber& mNum);
};