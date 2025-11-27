#include "polybius.h"
#include "basics.h"
#include "strings.h"
#include <array>
#include <iostream>

namespace polybius {
    std::tuple<int, int> findInPolybius(char c, polybius key)
    {
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                if (key[y][x] == c) {
                    return { x, y };
                }
            }
        }
        return { -1, -1 };
    }

    std::string playfairDecrypt(std::string text, polybius key)
    {
        std::string plain = "";
        auto blocks = strings::getBlocks(text, 2);

        //Lookup table for char position
        for (char c = 97; c < 114; c++) {
            std::cout << c << std::endl;
        }

        int pos0;
        int pos1;
        for (auto block : blocks) {
        }
        return plain;
    }

    std::string playfairHillClimber(std::string cipher)
    {
        return std::string();
    }
}