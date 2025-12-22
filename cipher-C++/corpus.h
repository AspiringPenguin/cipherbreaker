#pragma once
#include "fileLoaders.h"
#include <array>
#include <string>

namespace corpus {
	static std::string dataLoc = "D:/cipher-data/";

	//Corpus
	static auto corpus = fileLoaders::loadFile(dataLoc + "corpus.txt");

	//Monogram frequencies 
	static auto frequencies = fileLoaders::loadMapDouble(dataLoc + "lettercountsnospaces.txt");

	std::array<float, 26> frequenciesAsArray(std::unordered_map<std::string, float> map);
	std::vector<float> tetragramsAsArray(std::unordered_map<std::string, float> map);
	std::vector<float> precombineTetragrams(std::vector<float>& tetragrams);

	static auto frequencyArray = frequenciesAsArray(frequencies);

	//Word data
	static auto wordFrequencies = fileLoaders::loadMapInt(dataLoc + "wordcountsbyfrequency.txt");
	static auto wordsByFrequency = fileLoaders::loadVector(dataLoc + "wordcountsbyfrequency.txt");
	static auto wordsAlphabetical = fileLoaders::loadVector(dataLoc + "wordcountsalphabetical.txt");

	//Tetras
	static auto tetragramLogarithms = fileLoaders::loadMapDouble(dataLoc + "tetras/nospacesalphabeticallogarithms.txt");
	static auto tetragramArrays = tetragramsAsArray(tetragramLogarithms);

	static auto combinedTetragrams = precombineTetragrams(tetragramArrays);
}