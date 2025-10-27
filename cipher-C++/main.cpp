#include <iostream>
#include <string>
#include "fileLoaders.h"

int main() {
	std::string contents = fileLoaders::loadFile("D:/cipher-data/corpus.txt");
	std::cout << contents << std::endl;
	return 0;
}