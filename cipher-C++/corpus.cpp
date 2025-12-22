#include "corpus.h"

namespace corpus {
	std::array<float, 26> frequenciesAsArray(std::unordered_map<std::string, float> map) { //Convert a hashmap to an array, used for single characters so a=index 0 etc.
		std::array<float, 26> array;
		char c;
		for (int i = 0; i < 26; i++) {
			c = char(i + 97);
			array[i] = map[std::string(1, c)];
		}
		return array;
	}

	std::vector<float> tetragramsAsArray(std::unordered_map<std::string, float> map) {
		//Specialised case to convert tetragram frequencies/logs to an array
		//This puts each tetragram at the base 26 index specified by its letters e.g. aaaa=0, aaab=1 etc

		auto arrays = std::vector<float>();
		for (char i = 0; i < 26; i++) {
			for (char j = 0; j < 26; j++) {
				for (char k = 0; k < 26; k++) {
					for (char l = 0; l < 26; l++) {
						arrays.push_back(map[{static_cast<char>(i + 97), static_cast<char>(j + 97), static_cast<char>(k + 97), static_cast<char>(l + 97)}]);
					}
				}
			}
		}
		return arrays;
	}

	std::vector<float> precombineTetragrams(std::vector<float>& tetragrams) {
		//This was added to reduce the number of random memory accesses in tetragram fitness
		//By combing the logarithms of each possible pair of two tetragrams into a quintgram, which leads to roughly halving random memory accesses in tetragram fitness
		//The qintgrams are still put at the base 26 index specified by its letters e.g. aaaa=0, aaab=1 etc

		auto vec = std::vector<float>();
		
		float subtot;

		for (int i = 0; i < 26; i++) {
			for (int j = 0; j < 26; j++) {
				for (int k = 0; k < 26; k++) {
					for (int l = 0; l < 26; l++) {
						for (int m = 0; m < 26; m++) {
							subtot = 0;
							subtot += tetragrams[i * 17576 + j * 676 + k * 26 + l];
							subtot += tetragrams[j * 17576 + k * 676 + l * 26 + m];
							vec.push_back(subtot);
						}
					}
				}
			}
		}
		return vec;
	}
}