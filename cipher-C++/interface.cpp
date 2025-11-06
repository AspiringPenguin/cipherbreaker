#include "basics.h"
#include "interface.h"
#include <iostream>

namespace cliInterface {
	std::string getCipherInput() {
		std::cout << "Enter ciphertext:" << std::endl;
		std::string cipher = "";
		std::string inp = "";
		bool first = true;
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

	bool offerDecryption(std::string text) {
		std::cout << "Possible decryption:" << std::endl;
		std::cout << text << std::endl;
		std::cout << "Accept? (y/n) > ";
		std::string inp;
		getline(std::cin, inp);
		if (basics::formatString(inp) == "y") {
			return true;
		}
		return false;
	}

	//MenuItem
	MenuItem::MenuItem(std::string s, int(*_func)(std::string))
	{
		name = s;
		func = _func;
	}

	int MenuItem::run(std::string s)
	{
		return func(s);
	}

	
	//Menu
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
		int pos = std::stoi(inp) - 1;
		return menuItems[pos].run(cipher);
	}

	//Option to print the cipher
	int outputCipher(std::string cipher) {
		std::cout << cipher << std::endl;
		return 2;
	}

	int exit(std::string cipher) {
		return -2;
	}

	void showMenu(Menu menu) {
		int exitCode = menu.run();
		if (exitCode == -2) {
			return; //Exit here
		}
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
	}
}