#include "fileLoaders.h"

namespace fileLoaders {
	std::string loadFile(std::string fileName) {
		std::ifstream reader(fileName);

		if (!reader) {
			std::cerr << "Error opening file " << fileName << std::endl;
			return "";
		}
		else {
			char letter; //For reading the file

			std::string contents = ""; //To return

			while (!reader.eof()) {
				reader.get(letter);
				contents += letter;
			}
			reader.close();
			return contents;
		}
	}
	std::vector<std::string> loadList(std::string fileName)
	{
		auto wordList = std::vector<std::string>();

		std::string fileContents = loadFile(fileName);

		return wordList;
	}
}