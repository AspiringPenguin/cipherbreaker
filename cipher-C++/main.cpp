#include <chrono>
#include <iostream>
#include <string>
#include "basics.h"
#include "fileLoaders.h"
#include "fitness.h"
#include "interface.h"
#include "modularNumber.h"
#include "monoalphabetic.h"
#include "periodic.h"
#include "polybius.h"
#include "strings.h"
#include "transpositions.h"

//Add test func if not a release build
#ifdef TEST
int testStuff(std::string cipher) {
	//std::string text = basics::formatString(cipher);
	//for (int i = 0; i < 20; i++) {
	//	auto start = std::chrono::high_resolution_clock::now();
	//	//Something slow
	//	std::cout << monoalphabetic::hillClimber(text) << std::endl;
	//	auto end = std::chrono::high_resolution_clock::now();
	//	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//	std::cout << ms_int.count() << std::endl;
	//}

	cipher = basics::formatString(cipher);

	auto result = polybius::fourSquareHillClimber(cipher);

	std::cout << polybius::fourSquareDecrypt(cipher, std::get<0>(result), std::get<1>(result)) << std::endl;

	return 2;
}
#endif

int main() {
	std::string cipher = cliInterface::getCipherInput();
	cliInterface::Menu menu;
	menu.setCipher(cipher);

	//Add menu options
	menu.addMenuItem(cliInterface::MenuItem("Show cipher", *cliInterface::outputCipher));
	menu.addMenuItem(cliInterface::MenuItem("Analysis", *cliInterface::analysis));

	menu.addMenuItem(cliInterface::MenuItem("Atbash", *monoalphabetic::cliAtbash));
	menu.addMenuItem(cliInterface::MenuItem("Caesar", *monoalphabetic::cliCaesarBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Affine Shift", *monoalphabetic::cliAffineBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Monoalphabetic Hill Climber", *monoalphabetic::cliHillClimber));

	menu.addMenuItem(cliInterface::MenuItem("Vigenere", *periodic::cliVigenere));
	menu.addMenuItem(cliInterface::MenuItem("Polyalphabetic Substitution Cipher", *periodic::cliHillClimber));

	menu.addMenuItem(cliInterface::MenuItem("Permutation Cipher", *transpositions::cliPermutationBruteForce));

	menu.addMenuItem(cliInterface::MenuItem("Playfair", *polybius::cliPlayfairHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Playfair 2025 Variation", *polybius::cliPlayfair2025VariationHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Vertical Two Square", *polybius::cliVertTwoSquareHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Horizontal Two Square", *polybius::cliHorizTwoSquareHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Four Square", *polybius::cliFourSquareHillClimber));

	//Add test func if not a release build
	#ifdef TEST
	menu.addMenuItem(cliInterface::MenuItem("Test stuff", *testStuff));
	#endif

	//Finally, the exit code
	menu.addMenuItem(cliInterface::MenuItem("Exit", *cliInterface::exit));

	cliInterface::showMenu(menu);

	return 0;
}