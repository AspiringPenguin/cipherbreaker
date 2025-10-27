#include <iostream>
#include <string>
#include "fileLoaders.h"
#include "strings.h"

int main() {
	std::string toSplit = "this,isnot,a,test,arewesure?";
	for (std::string sub : strings::split(toSplit, ",")) {
		std::cout << sub << std::endl;
	}
	return 0;
}