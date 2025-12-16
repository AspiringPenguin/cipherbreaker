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
#include "solitaire.h"
#include "stream.h"
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

	std::cout << solitaire::decombineCharacters('y', 25) << std::endl;

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
	menu.addMenuItem(cliInterface::MenuItem("Caesar (Brute Force)", *monoalphabetic::cliCaesarBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Affine Shift (Brute Force)", *monoalphabetic::cliAffineBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Monoalphabetic (Hill Climber)", *monoalphabetic::cliHillClimber));

	menu.addMenuItem(cliInterface::MenuItem("Vigenere (Multiple Caesar Shifts)", *periodic::cliVigenere));
	menu.addMenuItem(cliInterface::MenuItem("Polyalphabetic Substitution (Hill Climber)", *periodic::cliHillClimber));

	menu.addMenuItem(cliInterface::MenuItem("Permutation (Brute Force)", *transpositions::cliPermutationBruteForce));

	menu.addMenuItem(cliInterface::MenuItem("Playfair (Wandering Hill Climber)", *polybius::cliPlayfairHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Playfair 2025 Variation (Wandering Hill Climber)", *polybius::cliPlayfair2025VariationHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Vertical Two Square (Wandering Hill Climber)", *polybius::cliVertTwoSquareHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Horizontal Two Square (Wandering Hill Climber)", *polybius::cliHorizTwoSquareHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Four Square (Wandering Hill Climber)", *polybius::cliFourSquareHillClimber));

	menu.addMenuItem(cliInterface::MenuItem("Autokey (Hill Climber)", *stream::cliAutokeyHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Progressive Vigenere (Brute Force -> Multiple Caesar Shifts)", *stream::cliProgressiveVigenereBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Solitaire (Hill Climber for part-known key)", nullptr));

	//Add test func if not a release build
	#ifdef TEST
	menu.addMenuItem(cliInterface::MenuItem("Test stuff", *testStuff));
	#endif

	//Finally, the exit code
	menu.addMenuItem(cliInterface::MenuItem("Exit", *cliInterface::exit));

	cliInterface::showMenu(menu);

	return 0;
}

//Priorities:
//1. Solitaire - WIP
//2. Double playfair
//3a. Hill, as per forum post attack?
//3b. More tranpositions
//3c. More polybius-derived stuff
//3d. IOCn stats