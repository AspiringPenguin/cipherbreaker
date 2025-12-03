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

	std::vector<std::string> loadLines(std::string fileName)
	{
		std::string fileContents = loadFile(fileName);
		return strings::split(fileContents, "\n");
	}

	std::vector<std::string> loadVector (std::string fileName){ //Throws away values attached to the key
		auto items = std::vector<std::string>();
		auto lines = loadLines(fileName);
		items.reserve(lines.size());
		for (std::string line : lines) {
			items.push_back(strings::split(line, ",")[0]);
		}
		return items;
	}

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

	std::unordered_map<std::string, float> loadMapDouble(std::string fileName)
	{
		auto map = std::unordered_map<std::string, float>();
		auto lines = loadLines(fileName);
		map.reserve(lines.size());
		for (std::string line : lines) {
			auto items = strings::split(line, ",");
			map[items[0]] = std::stod(items[1]);
		}
		return map;
	}
}