#include "basics.h"
#include "fitness.h"
#include "polybius.h"
#include "strings.h"
#include <array>
#include <iostream>
#include <random>

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

    void swapElems(polybius& key, std::uniform_int_distribution<>* rd, std::mt19937* gen) {
        int x1 = (*rd)(*gen);
        int y1 = (*rd)(*gen);
        int x2 = (*rd)(*gen);
        int y2 = (*rd)(*gen);

        char _ = key[y1][x1];
        key[y1][x1] = key[y2][x2];
        key[y2][x2] = _;
    }

    void swapRows(polybius& key, std::uniform_int_distribution<>* rd, std::mt19937* gen) {
        int y1 = (*rd)(*gen);
        int y2 = (*rd)(*gen);
        auto _ = key[y1];
        key[y1] = key[y2];
        key[y2] = _;
    }

    void swapCols(polybius& key, std::uniform_int_distribution<>* rd, std::mt19937* gen) {
        int x1 = (*rd)(*gen);
        int x2 = (*rd)(*gen);
        for (int i = 0; i < 5; i++) {
            auto _ = key[i][x1];
            key[i][x1] = key[i][x2];
            key[i][x2] = _;
        }
    }

    void flipDiag(polybius& key) {
        char _;
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < y; x++) {
                _ = key[y][x];
                key[y][x] = key[x][y];
                key[x][y] = _;
            }
        }
    }

    void flipVert(polybius& key) {
        std::array<char, 5> _;
        _ = key[0];
        key[0] = key[4];
        key[4] = _;
        _ = key[1];
        key[1] = key[3];
        key[3] = _;
    }

    void flipHoriz(polybius& key) {
        char _;
        for (int i = 0; i < 5; i++) {
            _ = key[i][0];
            key[i][0] = key[i][4];
            key[i][4] = _;
            _ = key[i][1];
            key[i][1] = key[i][3];
            key[i][3] = _;
            
        }
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

    std::string processPlayfairDecrypt(std::string decrypt) {
        int n = 0;
        while (true) {
            n = decrypt.find('x', n);
            if (n == std::string::npos) {
                return decrypt;
            }
            if (n + 1 == decrypt.length()) {
                return decrypt.substr(0, n);
            }
            if (decrypt[n - 1] == decrypt[n + 1]) {
                decrypt = decrypt.substr(0, n) + decrypt.substr(n + 1);
            }
            else {
                n++;
            }
        }
    }

    std::tuple<polybius, std::string> playfairHillClimber(std::string cipher)
    {
        cipher = basics::formatString(cipher);
        polybius bestKey;
        bestKey[0] = { 'p', 'o', 'l', 'y', 'b' };
        bestKey[1] = { 'i', 'u', 's', 'a', 'c' };
        bestKey[2] = { 'd', 'e', 'f', 'g', 'h' };
        bestKey[3] = { 'k', 'm', 'n', 'q', 'r' };
        bestKey[4] = { 't', 'v', 'w', 'x', 'z' };

        std::string bestDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, bestKey));
        double bestFitness = fitness::tetragramFitness(&bestDecrypt);

        polybius childKey;
        std::string childDecrypt;
        double childFitness;

        int counter = 0;
        int impatience = 0;

        //For pseudo-random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 30);

        while (counter < 1000000 && impatience < 10000000) {
            childKey = bestKey;
            
            //Change the key with the key here
            switch (changeChoice(gen)) {
            case 1:
                flipDiag(childKey);
            case 2:
                flipHoriz(childKey);
            case 3:
                flipDiag(childKey);
            case 4:
                swapRows(childKey, &dist, &gen);
            case 5:
                swapCols(childKey, &dist, &gen);
            default:
                swapElems(childKey, &dist, &gen);
            }


            childDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, childKey));
            childFitness = fitness::tetragramFitness(&childDecrypt);
            if (childFitness > bestFitness) {
                impatience = 0;
            updateBest:
                counter = 0;
                bestKey = childKey;
                bestFitness = childFitness;
                bestDecrypt = childDecrypt;
                std::cout << childFitness << std::endl;
            }
            else if ((childFitness + 0.5) > bestFitness && changeChoice(gen) == 1 && childKey != bestKey) {
                goto updateBest;
            }
            counter++;
            impatience++;
        }

        return { bestKey, bestDecrypt };
    }
}