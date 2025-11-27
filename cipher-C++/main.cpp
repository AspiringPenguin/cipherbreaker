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

//Add test func if not a release build
#ifdef TEST
int testStuff(std::string cipher) {
	//std::string text = basics::formatString(cipher);
	//for (int i = 0; i < 20; i++) {
	//	auto start = std::chrono::high_resolution_clock::now();
	//	//Something slow
	//	//std::cout << monoalphabetic::hillClimber(text) << std::endl;
	//	auto end = std::chrono::high_resolution_clock::now();
	//	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//	std::cout << ms_int.count() << std::endl;
	//}
	/*cipher = basics::formatString(cipher);
	std::cout << periodic::hillClimber(cipher) << std::endl;*/
	
	polybius::polybius key;
	key[0] = { 'a', 'b', 'c', 'd', 'e' };
	key[1] = { 'f', 'g', 'h', 'i', 'j' };
	key[2] = { 'k', 'l', 'm', 'n', 'o' };
	key[3] = { 'p', 'q', 'r', 's', 't' };
	key[4] = { 'u', 'v', 'w', 'y', 'z' };

	polybius::playfairDecrypt(cipher, key);

	return 2;
}
#endif

int main() {
	std::string cipher = cliInterface::getCipherInput();
	cliInterface::Menu menu;
	menu.setCipher(cipher);

	//Add menu options
	menu.addMenuItem(cliInterface::MenuItem("Show cipher", *cliInterface::outputCipher));

	menu.addMenuItem(cliInterface::MenuItem("Atbash", *monoalphabetic::cliAtbash));
	menu.addMenuItem(cliInterface::MenuItem("Caesar", *monoalphabetic::cliCaesarBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Affine Shift", *monoalphabetic::cliAffineBruteForce));
	menu.addMenuItem(cliInterface::MenuItem("Monoalphabetic Hill Climber", *monoalphabetic::cliHillClimber));

	menu.addMenuItem(cliInterface::MenuItem("Vigenere", *periodic::cliVigenere));


	//Add test func if not a release build
	#ifdef TEST
	menu.addMenuItem(cliInterface::MenuItem("Test stuff", *testStuff));
	#endif

	//Finally, the exit code
	menu.addMenuItem(cliInterface::MenuItem("Exit", *cliInterface::exit));

	cliInterface::showMenu(menu);

	return 0;
}