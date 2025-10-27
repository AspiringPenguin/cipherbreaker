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

	std::vector<std::string> loadVector (std::string fileName){ //Throws away values attached to the key
		auto items = std::vector<std::string>();
		for (std::string line : loadLines(fileName)) {
			items.push_back(strings::split(line, ",")[0]);
		}
		return items;
	}

	std::unordered_map<std::string, int> loadMapInt(std::string fileName)
	{
		auto map = std::unordered_map<std::string, int>();
		for (std::string line : loadLines(fileName)) {
			if (line == "") {
				continue;
			}
			auto items = strings::split(line, ",");
			map[items[0]] = std::stoi(items[1]);
		}
		return map;
	}

	std::unordered_map<std::string, double> loadMapDouble(std::string fileName)
	{
		auto map = std::unordered_map<std::string, double>();
		for (std::string line : loadLines(fileName)) {
			auto items = strings::split(line, ",");
			map[items[0]] = std::stod(items[1]);
		}
		return map;
	}
}