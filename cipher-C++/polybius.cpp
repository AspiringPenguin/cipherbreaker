#include "basics.h"
#include "fitness.h"
#include "polybius.h"
#include "strings.h"
#include <array>
#include <iostream>

namespace polybius {
    std::tuple<int, int> findInPolybius(char c, polybius key)
    {
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                if (key[y][x] == c) {
                    return { y, x };
                }
            }
        }
        return { -1, -1 };
    }

    int removeFive(int n){
        if (n == -1) {
            return 4;
        }
        return n;
    }

    std::string playfairDecrypt(std::string text, polybius key)
    {
        std::string plain = "";
        auto blocks = strings::getBlocks(text, 2);

        std::array<std::tuple<int, int>, 26> lookup;
        //Lookup table for char position
        for (char c = 97; c < 123; c++) {
            lookup[c - 97] = findInPolybius(c, key);
        }

        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        std::tuple<int, int> newpos0;
        std::tuple<int, int> newpos1;
        for (auto block : blocks) {
            pos0 = lookup[block[0] - 97];
            pos1 = lookup[block[1] - 97];
            if (std::get<0>(pos0) == std::get<0>(pos1)) {
                newpos0 = { std::get<0>(pos0), removeFive(std::get<1>(pos0) - 1) };
                newpos1 = { std::get<0>(pos1), removeFive(std::get<1>(pos1) - 1) };
            }
            else if (std::get<1>(pos0) == std::get<1>(pos1)) {
                newpos0 = { removeFive(std::get<0>(pos0) - 1), std::get<1>(pos0) };
                newpos1 = { removeFive(std::get<0>(pos1) - 1), std::get<1>(pos1) };
            }
            else {
                newpos0 = { std::get<0>(pos0) , std::get<1>(pos1) };
                newpos1 = { std::get<0>(pos1) , std::get<1>(pos0) };
            }
            plain += key[std::get<0>(newpos0)][std::get<1>(newpos0)];
            plain += key[std::get<0>(newpos1)][std::get<1>(newpos1)];
        }
        return plain;
    }

    std::string playfairHillClimber(std::string cipher)
    {
        cipher = basics::formatString(cipher);
        polybius bestKey;
        bestKey[0] = { 'p', 'o', 'l', 'y', 'b' };
        bestKey[1] = { 'i', 'u', 's', 'a', 'c' };
        bestKey[2] = { 'd', 'e', 'f', 'g', 'h' };
        bestKey[3] = { 'k', 'm', 'n', 'q', 'r' };
        bestKey[4] = { 't', 'v', 'w', 'x', 'z' };

        std::string bestDecrypt = playfairDecrypt(cipher, bestKey);
        double bestFitness = fitness::tetragramFitness(&bestDecrypt);

        polybius childKey;
        std::string childDecrypt;
        double childFitness;

        int counter = 0;

        while (counter < 10000) {
            childKey = bestKey;
            //Fiddle with the key
            childDecrypt = playfairDecrypt(cipher, childKey);
            childFitness = fitness::tetragramFitness(&childDecrypt);
            if (childFitness > bestFitness) { 
            updateBest:
                bestKey = childKey;
                bestFitness = childFitness;
                bestDecrypt = childDecrypt;
                counter = 0;
                std::cout << childFitness << std::endl;
            }
            else if ((childFitness + 0.5) > bestFitness && false /*Random here*/) {
                goto updateBest;
            }
            counter++;
        }
    }
}