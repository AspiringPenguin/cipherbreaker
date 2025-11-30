#include "basics.h"
#include "fitness.h"
#include "interface.h"
#include "polybius.h"
#include "strings.h"
#include <algorithm> //For shuffle
#include <array>
#include <future>
#include <iostream>
#include <random>
#include <thread>

namespace polybius {
    polybius makePolybius(std::string key) {
        polybius result{ -1 };
        int y = 0;
        int x = -1;
        for (char c : key) {
            if (c == 'j') {
                continue;
            }
            x++;
            if (x == 5) {
                x = 0;
                y++;
            }
            result[y][x] = c;
        }
        return result;
    }

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
        plain.reserve(text.size());
        //auto blocks = strings::getBlocks(text, 2);

        std::array<std::tuple<int, int>, 26> lookup;
        //Lookup table for char position
        for (char c = 97; c < 123; c++) {
            lookup[c - 97] = findInPolybius(c, key);
        }

        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        std::tuple<int, int> newpos0;
        std::tuple<int, int> newpos1;
        int l = text.size();
        for (int i = 0; i < l; i+=2) {
            pos0 = lookup[text[i] - 97];
            pos1 = lookup[text[i+1] - 97];
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
            if (n + 2 != decrypt.length()) {
                if (decrypt[n - 1] == decrypt[n + 1]) {
                    decrypt = decrypt.substr(0, n) + decrypt.substr(n + 1);
                }
                else {
                    n++;
                }
            }
            else {
                n++;
            }
        }
    }

    polybius playfairHillClimber(std::string cipher)
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
        polybius lastKey = bestKey;

        //For pseudo-random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 50);

        while (counter < 100000) {
            childKey = bestKey;
            
            //Change the key with the key here
            switch (changeChoice(gen)) {
            case 1:
                flipDiag(childKey);
                break;
            case 2:
                flipHoriz(childKey);
                break;
            case 3:
                flipDiag(childKey);
                break;
            case 4:
                swapRows(childKey, &dist, &gen);
                break;
            case 5:
                swapCols(childKey, &dist, &gen);
                break;
            default:
                swapElems(childKey, &dist, &gen);
                break;
            }

            childDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, childKey));
            childFitness = fitness::tetragramFitness(&childDecrypt);
            if (childFitness > bestFitness || ((childFitness + 1) > bestFitness && changeChoice(gen) < 4 && childKey != bestKey && childKey != lastKey)) {
                counter = 0;
                lastKey = bestKey;
                bestKey = childKey;
                bestFitness = childFitness;
                bestDecrypt = childDecrypt;
                std::cout << childFitness << std::endl;
            }
            counter++;
        }

        return bestKey;
    }

    std::vector<polybius> getAllChildKeys(polybius key) {
        auto keys = std::vector<polybius>();
        polybius childKey;

        //Swap two places
        for (int y1 = 0; y1 < 5; y1++) {
            for (int y2 = 0; y2 < 5; y2++) {
                for (int x1 = 0; x1 < 5; x1++) {
                    for (int x2 = 0; x2 < 5; x2++) {
                        childKey = key;
                        char _ = childKey[y1][x1];
                        childKey[y1][x1] = childKey[y2][x2];
                        childKey[y2][x2] = _;
                        keys.push_back(childKey);
                    }
                }
            }
        }

        //Swap rows
        for (int y1 = 0; y1 < 5; y1++) {
            for (int y2 = 0; y2 < 5; y2++) {
                childKey = key;
                auto _ = childKey[y1];
                childKey[y1] = childKey[y2];
                childKey[y2] = _;
                keys.push_back(childKey);
            }
        }

        //Swap cols
        for (int x1 = 0; x1 < 5; x1++) {
            for (int x2 = 0; x2 < 5; x2++) {
                for (int i = 0; i < 5; i++) {
                    childKey = key;
                    auto _ = childKey[i][x1];
                    childKey[i][x1] = childKey[i][x2];
                    childKey[i][x2] = _;
                    keys.push_back(childKey);
                }
            }
        }

        //Flips
        childKey = key;
        flipHoriz(childKey);
        keys.push_back(childKey);

        childKey = key;
        flipVert(childKey);
        keys.push_back(childKey);

        childKey = key;
        flipDiag(childKey);
        keys.push_back(childKey);

        return keys;
    }

    polybius playfairBacktracking(std::string cipher, polybius startKey, bool ignoreBad) {
        //Get a starting point
        auto decrypt = processPlayfairDecrypt(playfairDecrypt(cipher, startKey));
        double fitness = fitness::tetragramFitness(&decrypt);
        polybius bestKey = startKey;
        double bestFitness = fitness;
        double childFitness;

        std::vector<polybius> children;
        polybius bestChild;

        while (true) {
            children = getAllChildKeys(bestKey);
            fitness = -100;
            bestChild = nullPolybius;
            for (const polybius& child : children) {
                decrypt = processPlayfairDecrypt(playfairDecrypt(cipher, child));
                childFitness = fitness::tetragramFitness(&decrypt);
                if (childFitness > fitness) {
                    fitness = childFitness;
                    bestChild = child;
                }
            }
            if (fitness > bestFitness) {
                bestKey = bestChild;
                bestFitness = fitness;
            }
            else if (bestFitness > -15 || !ignoreBad) {
                return bestKey;
            }
            else {
                return nullPolybius;
            }
        }
        return bestKey;
    }

    polybius playfairBacktracking(std::string cipher) {
        polybius result = nullPolybius;
        polybius key;

        //For pseudo-random numbers
        std::random_device rd;
        std::mt19937 gen(rd());

        //Main loop
        int n = 0;
        while (result == nullPolybius) {
            auto alphabetCopy = basics::alphabet;
            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            key = makePolybius(alphabetCopy);
            result = playfairBacktracking(cipher, key, true);
            n++;
        }
        std::cout << n << std::endl;
        return result;
    }

    polybius playfairBacktracking(std::string cipher, bool* flag) {
        polybius result = nullPolybius;
        polybius key;

        //For pseudo-random numbers
        std::random_device rd;
        std::mt19937 gen(rd());

        //Main loop
        int n = 0;
        while (result == nullPolybius and !*flag) {
            auto alphabetCopy = basics::alphabet;
            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            key = makePolybius(alphabetCopy);
            result = playfairBacktracking(cipher, key, true);
            n++;
        }
        std::cout << n << std::endl;
        return result;
    }

    //polybius playfairBacktracking(std::string cipher, polybius key, double rootFitness, int depth, int maxDepth) {
    //    std::cout << rootFitness << std::endl;
    //    if (depth == maxDepth) {
    //        if (rootFitness > -15) {
    //            return key;
    //        }
    //        return nullPolybius;
    //    }
    //    auto children = getAllChildKeys(key);
    //    std::sort(children.begin(), children.end(), [cipher](polybius a, polybius b) -> bool {
    //        std::string decrypta = processPlayfairDecrypt(playfairDecrypt(cipher, a));
    //        std::string decryptb = processPlayfairDecrypt(playfairDecrypt(cipher, b));
    //        return fitness::tetragramFitness(&decrypta) > fitness::tetragramFitness(&decryptb);
    //    });
    //    std::string decrypt;
    //    double fitness;
    //    polybius result;
    //    for (const polybius& child : children) {
    //        decrypt = processPlayfairDecrypt(playfairDecrypt(cipher, child));
    //        fitness = fitness::tetragramFitness(&decrypt);
    //        if (fitness > rootFitness) {
    //            result = playfairBacktracking(cipher, child, fitness, depth+1, maxDepth);
    //            if (result != nullPolybius) {
    //                return result;
    //            }
    //        }
    //        else {
    //            break; //Pre-sorted
    //        }
    //    }
    //    if (rootFitness > -15) {
    //        return key;
    //    }
    //    return nullPolybius;
    //}


}