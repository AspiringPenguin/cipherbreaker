#include <iostream>
#include <string>
#include "basics.h"
#include "fileLoaders.h"
#include "interface.h"
#include "monoalphabetic.h"
#include "strings.h"

int testStuff(std::string cipher) {
	return 2;
}

int main() {
	std::string cipher = cliInterface::getCipherInput();
	cliInterface::Menu menu;
	menu.setCipher(cipher);

	//Add menu options
	menu.addMenuItem(cliInterface::MenuItem("Show cipher", *cliInterface::outputCipher));
	menu.addMenuItem(cliInterface::MenuItem("Test stuff", *testStuff));

	return menu.run();
}