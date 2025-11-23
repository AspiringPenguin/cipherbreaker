#include <iostream>
#include <string>
#include "basics.h"
#include "fileLoaders.h"
#include "interface.h"
#include "modularNumber.h"
#include "monoalphabetic.h"
#include "strings.h"

//Add test func if not a release build
#ifdef TEST
int testStuff(std::string cipher) {
	std::string text = basics::formatString(cipher);
	std::cout << monoalphabetic::hillClimber(text) << std::endl;
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


	//Add test func if not a release build
	#ifdef TEST
	menu.addMenuItem(cliInterface::MenuItem("Test stuff", *testStuff));
	#endif

	//Finally, the exit code
	menu.addMenuItem(cliInterface::MenuItem("Exit", *cliInterface::exit));

	cliInterface::showMenu(menu);

	return 0;
}