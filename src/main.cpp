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

// Todo:
// Update polybius support to deal with z into y as well as j into i. We can't just botch it as each letter isn't independent.
// Perhaps we need an extra parameter for which character to ignore in the starting key for attacks. 
// Also add more decriptive error messages so I know why things have failed to start without checking the code.
// 
// Interpret morse?!
// 
// Improve tools for polyalphabetic substitutions
// - Beaufort
// - Variant Beaufort
// - Porta
// - Bellaso 1552
// - Periodic Affine
// - Quagmires?
// - Create a menu option that tries each of these in turn
// - perhaps improve the hill-climber (probably shotgun hill climbing would work best here, could be a good case for multithreading)
//   Some texts work well (like Madness's example for that attack) but it would be good to make it faster and/or more reliable for shorter texts
//   and for larger keys - it seems to struggle with most other things I threw at it - but this is probable mainly a result of the massive key-space
//   and the consequent ease of overfitting to the text, giving gibberish that maximised tetragram fitness
//
// More grid ciphers - whole section?
// 
// Matrices implementation
// Hill cipher - decrypt, brute force with less brute force, using cribs
// Affine hill cipher?
// 
// More transposition ciphers, and maybe have a combined script that checks them all
// 
// Finish solitaire cipher attack

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