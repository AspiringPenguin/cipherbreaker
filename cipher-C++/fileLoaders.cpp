#include "fileLoaders.h"
#include "strings.h"

namespace fileLoaders {
	//Literally just loads the contents as a string
	std::string loadFile(std::string fileName) {
		std::ifstream reader(fileName);

		if (!reader) {
			std::cerr << "Error opening file " << fileName << std::endl;
			return "";
		}
		else {
			std::string line; //For reading the file

			std::string contents = ""; //To return

			bool skip = true;

			while (!reader.eof()) {
				getline(reader, line);
				if (!skip) {
					contents += "\n";
				}
				else {
					skip = false;
				}
				contents += line;
			}
			reader.close();
			return contents;
		}
	}

	//Loads the file, then returns a std::vector<std::string> containing the lines in order
	std::vector<std::string> loadLines(std::string fileName) {
		std::string fileContents = loadFile(fileName);
		return strings::split(fileContents, "\n");
	}


	//These all expect data in "key, value" format on each line

	//Throws away values attached to the key, and just returns the keys in an std::vector<std::string>
	std::vector<std::string> loadVector (std::string fileName){
		auto items = std::vector<std::string>();
		auto lines = loadLines(fileName);
		items.reserve(lines.size());
		for (std::string line : lines) {
			items.push_back(strings::split(line, ",")[0]);
		}
		return items;
	}

	//Loads the data to a map, storing the values as integers
	std::unordered_map<std::string, int> loadMapInt(std::string fileName)
	{
		auto map = std::unordered_map<std::string, int>();
		auto lines = loadLines(fileName);
		map.reserve(lines.size());
		for (std::string line : lines) {
			if (line == "") {
				continue;
			}
			auto items = strings::split(line, ",");
			map[items[0]] = std::stoi(items[1]);
		}
		return map;
	}

	//Loads the data to a map, storing the values as doubles
	std::unordered_map<std::string, float> loadMapDouble(std::string fileName)
	{
		auto map = std::unordered_map<std::string, float>();
		auto lines = loadLines(fileName);
		map.reserve(lines.size());
		for (std::string line : lines) {
			auto items = strings::split(line, ",");
			map[items[0]] = std::stof(items[1]);
		}
		return map;
	}
}