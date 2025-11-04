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
		return 0;
	}
}