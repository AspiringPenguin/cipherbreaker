#pragma once
#include <string>
#include <vector>

namespace cliInterface {
	std::string getCipherInput();
	int outputCipher(std::string cipher);

	class MenuItem {
		int (*func) (std::string);
	public:
		std::string name;
		MenuItem(std::string s, int (*func) (std::string));
		int run(std::string);
	};

	class Menu {
		std::vector<MenuItem> menuItems;
		std::string cipher;

	public:
		std::string getCipher();
		void setCipher(std::string cipher);
		void addMenuItem(MenuItem i);
		int run();
	};


	void showMenu(Menu menu);
}