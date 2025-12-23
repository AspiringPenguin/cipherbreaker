#pragma once
#include <string>
#include <vector>

namespace cliInterface {
	//This is a load of standard functions for cli interactions with the user, along with a menu system.

	std::string getCipherInput();
	bool offerDecryption(std::string text);
	int outputCipher(std::string cipher);
	int exit(std::string cipher);

	int analysis(std::string cipher);

	class MenuItem {
		int (*func) (std::string);
	public:
		std::string name;
		MenuItem(std::string s, int (*func) (std::string));
		int run(std::string);
	};

	class Menu {
		std::vector<MenuItem> menuItems; //List of all menuItems in order
		std::string cipher; //The cipher to pass as the parameter to any chosen options

	public:
		std::string getCipher();
		void setCipher(std::string cipher);
		void addMenuItem(MenuItem i);
		int run();
	};


	void showMenu(Menu menu);
}