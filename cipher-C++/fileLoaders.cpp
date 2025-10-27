#include "fileLoaders.h"
#include "strings.h"

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
	std::vector<std::string> loadLines(std::string fileName)
	{
		std::string fileContents = loadFile(fileName);
		return strings::split(fileContents, "\n");
	}

	std::unordered_map<std::string, int> loadMapInt(std::string fileName)
	{
		auto map = std::unordered_map<std::string, int>();
		for (std::string line:loadLines())
		return std::unordered_map<std::string, int>();
	}

	std::unordered_map<std::string, double> loadMapDouble(std::string fileName)
	{
		return std::unordered_map<std::string, double>();
	}
}