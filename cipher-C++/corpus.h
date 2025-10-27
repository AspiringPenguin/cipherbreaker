#pragma once
#include "fileLoaders.h"

namespace corpus {
	//Corpus
	static auto corpus = fileLoaders::loadFile("D:/cipher-data/corpus.txt");

	//Monogram frequencies 
	static auto frequencies = fileLoaders::loadMapDouble("D:/cipher-data/lettercountsnospaces.txt");

	//Word data
	static auto wordFrequencies = fileLoaders::loadMapInt("D:/cipher-data/wordcountsbyfrequency.txt");
	static auto wordsByFrequency = fileLoaders::loadVector("D:/cipher-data/wordcountsbyfrequency.txt");
	static auto wordsAlphabetical = fileLoaders::loadVector("D:/cipher-data/wordcountsalphabetical.txt");

	//Tetras
	static auto tetragramLogarithms = fileLoaders::loadMapDouble("D:/cipher-data/tetras/nospacesalphabeticallogarithms.txt");
}