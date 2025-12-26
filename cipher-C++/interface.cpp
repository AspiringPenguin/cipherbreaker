#include "basics.h"
#include "fitness.h"
#include "interface.h"
#include <iostream>

namespace cliInterface {
	//Multiline cipher input, conacatenates all input up to a line cotaining just 'end' and returns it.
	std::string getCipherInput() {
		std::cout << "Enter ciphertext:" << std::endl;
		std::string cipher = "";
		std::string inp = "";
		bool first = true; //Controls the addition of '\n', not sure why this is needed
		while (!(inp == "end")) {
			cipher += inp;
			if (!first) {
				cipher += "\n";
			}
			else {
				first = false;
			}
			getline(std::cin, inp);
		}
		return cipher;
	}

	//A function for offering a decryption with a y/n prompt for whether to accept it.
	bool offerDecryption(std::string text) {
		std::cout << "Possible decryption:" << std::endl;
		std::cout << text << std::endl;
		std::cout << "Accept? (y/n) > ";
		std::string inp;
		getline(std::cin, inp);
		if (basics::formatString(inp) == "y") { //formatString used here to remove non-alphabetic characters and strip out any spaces
			return true;
		}
		return false;
	}

	//MenuItem to add to menu, with descriptor to show and a pointer to the function
	MenuItem::MenuItem(std::string s, int(*_func)(std::string))
	{
		name = s;
		func = _func;
	}

	int MenuItem::run(std::string s)
	{
		return func(s);
	}

	
	//Menu itself
	std::string Menu::getCipher()
	{ 
		return cipher;
	}

	void Menu::setCipher(std::string _cipher)
	{
		cipher = _cipher;
	}

	void Menu::addMenuItem(MenuItem i)
	{
		menuItems.push_back(i);
	}

	//This handles the output of menu item descriptors, choosing a menu item and running it before returning the exit code
	//The rest is handled in showMenu
	int Menu::run()
	{
		std::cout << "----------MENU----------" << std::endl;
		int l = menuItems.size();
		for (int i = 0; i < l; i++) {
			std::cout << i + 1 << ". " << menuItems[i].name << std::endl;
		}
		std::cout << "> ";
		std::string inp;
		getline(std::cin, inp);
		int pos = std::stoi(inp) - 1; //Index 0 is printed as 1. etc.
		return menuItems[pos].run(cipher);
	}

	//Option to print the cipher
	int outputCipher(std::string cipher) {
		std::cout << cipher << std::endl;
		return 2; //Complete
	}

	//Option to exit
	int exit(std::string cipher) {
		return -2; //Exit
	}

	//Significant analysis ouput function, with various stats
	int analysis(std::string cipher) {
		auto formatted = basics::formatString(cipher);

		std::cout << "----------Statistics----------" << std::endl;
		std::cout << "Number of characters: " << cipher.length() << std::endl;
		std::cout << "Number of characters - formatted: " << formatted.length() << std::endl;

		if (formatted.length() > 0) {
			std::cout << "Monogram angle: " << fitness::angleBetweenVectorsFitness(formatted) << std::endl;
			std::cout << "Tetragram Fitness: " << fitness::tetragramFitness(&formatted) << std::endl;
			std::cout << "Index of Coincidence: " << fitness::indexOfCoincidence(formatted) << std::endl;
			std::cout << "Index of Coincidence for bigrams (Corpus 0.0072): " << fitness::indexOfCoincidenceBigrams(formatted) << std::endl;
			std::cout << "Index of Coincidence for trigrams (Corpus 0.0013): " << fitness::indexOfCoincidenceTrigrams(formatted) << std::endl << std::endl;

			std::cout << "Periodic IOC:" << std::endl;
			for (int n = 2; n < 27; n++) { //Goes up to 26 to cover for trithemius cipher
				std::cout << "  Period " << n << ": " << fitness::indexOfCoincidencePeriodic(formatted, n) << std::endl;
			}
		}

		return 2; //Complete
	}

	//This handles the exit code logic of the menu and the repeated display of the menu
	void showMenu(Menu menu) {
		int exitCode = menu.run();
		if (exitCode == -2) {
			return; //Exit here
		}
		std::cout << std::endl;
		if (exitCode == 0) {
			std::cout << "FAILURE" << std::endl << std::endl;
			showMenu(menu);
		}
		else if (exitCode == 1) {
			std::cout << "SUCCESS" << std::endl << std::endl;
			showMenu(menu);
		}
		else if (exitCode == 2) {
			std::cout << "COMPLETE" << std::endl << std::endl;
			showMenu(menu);
		}
		else if (exitCode == 3) {
			std::cout << "FUNCTION IS WIP\nThe function has finished with no other relevant outcome." << std::endl << std::endl;
			showMenu(menu);
		}
		//Any other codes just fall through and cause the program to exit in the end
	}
}