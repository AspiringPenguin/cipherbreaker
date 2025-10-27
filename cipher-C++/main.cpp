#include <iostream>
#include <string>
#include "fileLoaders.h"
#include "strings.h"

int main() {
	std::vector<std::string> words = fileLoaders::loadList("D:/cipher-data/wordcountsbyfrequency.txt");
	for (std::string word : words) {
		std::cout << word << std::endl;
	}
	return 0;
}