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
}