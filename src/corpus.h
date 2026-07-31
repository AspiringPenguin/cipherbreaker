#pragma once
#include "fileLoaders.h"
#include <array>
#include <string>

namespace corpus {
	#if defined(_WIN64)
		static std::string dataLoc = "D:/cipher-data/"; //Change this to wherever you store the data if you want to build and run the code
	#else
		static std::string dataLoc = std::string(getenv("HOME")) + "/cipher-stuff/cipher-data/"; //Change this to wherever you store the data if you want to build and run the code
	#endif
	

	//Corpus
	static auto corpus = fileLoaders::loadFile(dataLoc + "corpus.txt"); //Load the corpus file

	//Monogram frequencies 
	static auto frequencies = fileLoaders::loadMapDouble(dataLoc + "lettercountsnospaces.txt"); //Letter counts lookup

	//Some helper functions, see implementation for more details
	std::array<float, 26> frequenciesAsArray(std::unordered_map<std::string, float> map);
	std::vector<float> tetragramsAsArray(std::unordered_map<std::string, float> map);
	std::vector<float> precombineTetragrams(std::vector<float>& tetragrams);

	static auto frequencyArray = frequenciesAsArray(frequencies); //Letter counts as an array - see implementation for more details

	//Word data
	static auto wordFrequencies = fileLoaders::loadMapInt(dataLoc + "wordcountsbyfrequency.txt"); //Lookup table for frequency
	static auto wordsByFrequency = fileLoaders::loadVector(dataLoc + "wordcountsbyfrequency.txt"); //List of words by frequency
	static auto wordsAlphabetical = fileLoaders::loadVector(dataLoc + "wordcountsalphabetical.txt"); //List of words alphabetically

	//Tetras
	static auto tetragramLogarithms = fileLoaders::loadMapDouble(dataLoc + "tetras/nospacesalphabeticallogarithms.txt"); //Logarithms of tetragram frequencies

	//The two steps for optimising tetragram fitness, see implementation for more details
	static auto tetragramArrays = tetragramsAsArray(tetragramLogarithms);
	static auto combinedTetragrams = precombineTetragrams(tetragramArrays);
}
