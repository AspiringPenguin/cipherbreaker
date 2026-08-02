#include <chrono>
#include <iostream>
#include <string>
#include "basics.h"
#include "corpus.h"
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

//Add test func if not a release build - this is generally used for testing or providing easy debug access to a certain piece of code
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

	//cipher = basics::formatString(cipher);

	return 2; //Complete
}
#endif

int main() {
	std::string cipher = cliInterface::getCipherInput();
	cliInterface::Menu menu;
	menu.setCipher(cipher);

	//Add menu options
	menu.addMenuItem(cliInterface::MenuItem("Analysis", *cliInterface::analysis));

	menu.addMenuItem(cliInterface::MenuItem("Atbash", *monoalphabetic::cliAtbash));
	menu.addMenuItem(cliInterface::MenuItem("Caesar (Brute Force)", *monoalphabetic::cliCaesarBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Affine Shift (Brute Force)", *monoalphabetic::cliAffineBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Monoalphabetic (Hill Climber)", *monoalphabetic::cliHillClimber));

	menu.addMenuItem(cliInterface::MenuItem("Vigenere (Multiple Caesar Shifts)", *periodic::cliVigenereAsCaesarShifts));
	menu.addMenuItem(cliInterface::MenuItem("Polyalphabetic Substitution (Hill Climber)", *periodic::cliHillClimber));

	menu.addMenuItem(cliInterface::MenuItem("Permutation Transposition (Hill Climber)", *transpositions::cliPermutationHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Columnar Transposition (Hill Climber)", *transpositions::cliColumnarHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Twisted Scytale (Brute Force)", *transpositions::cliTwistedScytaleBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Railfence (Brute Force)", *transpositions::cliRailfenceBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Redefence (Brute Force)", *transpositions::cliRedefenceBruteForce));

	menu.addMenuItem(cliInterface::MenuItem("Playfair (Wandering Hill Climber)", *polybius::cliPlayfairHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Playfair 2025 Variation (Wandering Hill Climber)", *polybius::cliPlayfair2025VariationHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Vertical Two Square (Wandering Hill Climber)", *polybius::cliVertTwoSquareHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Horizontal Two Square (Wandering Hill Climber)", *polybius::cliHorizTwoSquareHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Four Square (Wandering Hill Climber)", *polybius::cliFourSquareHillClimber));

	menu.addMenuItem(cliInterface::MenuItem("Autokey (Hill Climber)", *stream::cliAutokeyHillClimber));
	menu.addMenuItem(cliInterface::MenuItem("Progressive Vigenere (Brute Force -> Multiple Caesar Shifts)", *stream::cliProgressiveVigenereBruteForce));

	//Add test func if not a release build
	#ifdef TEST
	menu.addMenuItem(cliInterface::MenuItem("Test stuff", *testStuff));
	#endif

	cliInterface::showMenu(menu);

	return 0;
}