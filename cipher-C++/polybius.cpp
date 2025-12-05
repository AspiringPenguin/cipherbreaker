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
        std::ios_base::sync_with_stdio(false);

        cipher = basics::formatString(cipher);
        polybius bestKey;
        bestKey[0] = { 'p', 'o', 'l', 'y', 'b' };
        bestKey[1] = { 'i', 'u', 's', 'a', 'c' };
        bestKey[2] = { 'd', 'e', 'f', 'g', 'h' };
        bestKey[3] = { 'k', 'm', 'n', 'q', 'r' };
        bestKey[4] = { 't', 'v', 'w', 'x', 'z' };
        polybius currentKey = bestKey;

        std::string bestDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, bestKey));
        float bestFitness = fitness::tetragramFitness(&bestDecrypt);
        float currentFitness = bestFitness;

        polybius childKey;
        std::string childDecrypt;
        float childFitness;

        int counter = 0;
        int impatience = 0;

        bool improved;

        //For pseudo-random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 50);

        float chance;

        while (counter < 2000000) {
            childKey = currentKey;
            
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

            improved = false;

            playfairEvaluate:
            if (childFitness > bestFitness) {
                childKey = playfairBacktracking(cipher, childKey, false);

                childDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, childKey));
                childFitness = fitness::tetragramFitness(&childDecrypt);

                std::cout << childFitness << std::endl;

                //Because of the recursive improver
                if (childFitness > -13) {
                    return childKey;
                }

                bestKey = childKey;
                bestFitness = childFitness;
                bestDecrypt = childDecrypt;
                currentKey = childKey;
                currentFitness = childFitness;
                counter = 0;
                impatience = 0;
            }

            else if (childFitness == bestFitness) {
                impatience = 0;
                goto playfairChildFitnessGreater;
            }

            else if (childFitness > currentFitness) {
                playfairChildFitnessGreater:
                if (!improved) {
                    improved = true;

                    childKey = playfairBacktracking(cipher, childKey, false);

                    childDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, childKey));
                    childFitness = fitness::tetragramFitness(&childDecrypt);

                    goto playfairEvaluate;
                }

                currentKey = childKey;
                currentFitness = childFitness;
            }

            else if (counter > 5000) {
                chance = -(((-childFitness / 8.0f) - 1.0f) / (childFitness - bestFitness)) * 3.0f + 0.5f;
                if (changeChoice(gen) < chance) {
                    currentKey = childKey;
                    currentFitness = childFitness;
                }
            }

            counter++;
            impatience++;

            if (impatience > 2000) {
                currentKey = bestKey;
                currentFitness = bestFitness;
                impatience = 0;
            }
        }

        std::ios_base::sync_with_stdio(true);

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
        float fitness = fitness::tetragramFitness(&decrypt);
        polybius bestKey = startKey;
        float bestFitness = fitness;
        float childFitness;

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

    int cliPlayfairHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);
        polybius bestKey = playfairHillClimber(cipher);
        for (const auto& row : bestKey) {
            for (const auto& item : row) {
                std::cout << item << " ";
            }
            std::cout << std::endl;
        }
        std::string rawDecrypt = playfairDecrypt(cipher, bestKey);
        std::string decrypt = processPlayfairDecrypt(rawDecrypt);
        if (fitness::tetragramFitness(&decrypt) > -15) {
            if (cliInterface::offerDecryption(decrypt)) {
                std::cout << "Raw Decrypt: " << std::endl;
                std::cout << rawDecrypt << std::endl;
                return 1;
            }
        }
        return 0;
    }

    //Same process to encrypt
    std::string vertTwoSquareDecrypt(std::string cipher, polybius top, polybius bottom, int flips) {
        std::string plain = "";
        plain.reserve(cipher.size());

        //Lookup table for char position
        std::array<std::tuple<int, int>, 26> topLookup;
        for (char c = 97; c < 123; c++) {
            topLookup[c - 97] = findInPolybius(c, top);
        }
        std::array<std::tuple<int, int>, 26> bottomLookup;
        for (char c = 97; c < 123; c++) {
            bottomLookup[c - 97] = findInPolybius(c, bottom);
        }

        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        int newX;
        int newY;
        int l = cipher.size();
        for (int i = 0; i < l; i += 2) {
            pos0 = topLookup[cipher[i] - 97];
            pos1 = bottomLookup[cipher[i + 1] - 97];
            if (std::get<1>(pos0) == std::get<1>(pos1)) { //Same column
                if (flips & 1) {
                    plain += cipher[i + 1];
                    plain += cipher[i];
                }
                else {
                    plain += cipher[i];
                    plain += cipher[i + 1];
                }
            }
            else {
                if (flips & 2) {
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += bottom[newY][newX];
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += top[newY][newX];
                }
                else {
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += top[newY][newX];
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += bottom[newY][newX];
                }
            }
        }
        return plain;
    }

    std::string horizTwoSquareEncrypt(std::string cipher, polybius left, polybius right, int flips) {
        std::string plain = "";
        plain.reserve(cipher.size());

        //Lookup table for char position
        std::array<std::tuple<int, int>, 26> leftLookup;
        for (char c = 97; c < 123; c++) {
            leftLookup[c - 97] = findInPolybius(c, left);
        }
        std::array<std::tuple<int, int>, 26> rightLookup;
        for (char c = 97; c < 123; c++) {
            rightLookup[c - 97] = findInPolybius(c, right);
        }

        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        int newX;
        int newY;
        int l = cipher.size();
        for (int i = 0; i < l; i += 2) {
            pos0 = leftLookup[cipher[i] - 97];
            pos1 = rightLookup[cipher[i + 1] - 97];
            if (std::get<0>(pos0) == std::get<0>(pos1)) { //Same row
                if (flips & 1) {
                    plain += cipher[i + 1];
                    plain += cipher[i];
                }
                else {
                    plain += cipher[i];
                    plain += cipher[i + 1];
                }
            }
            else {
                if (flips & 2) {
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += right[newY][newX];
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += left[newY][newX];
                }
                else {
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += right[newY][newX];
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += left[newY][newX];
                }
            }
        }
        return plain;
    }



    std::string horizTwoSquareDecrypt(std::string cipher, polybius left, polybius right, int flips) {
        std::string plain = "";
        plain.reserve(cipher.size());

        //Lookup table for char position
        std::array<std::tuple<int, int>, 26> leftLookup;
        for (char c = 97; c < 123; c++) {
            leftLookup[c - 97] = findInPolybius(c, left);
        }
        std::array<std::tuple<int, int>, 26> rightLookup;
        for (char c = 97; c < 123; c++) {
            rightLookup[c - 97] = findInPolybius(c, right);
        }

        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        int newX;
        int newY;
        int l = cipher.size();
        for (int i = 0; i < l; i += 2) {
            pos0 = rightLookup[cipher[i] - 97];
            pos1 = leftLookup[cipher[i + 1] - 97];
            if (std::get<0>(pos0) == std::get<0>(pos1)) { //Same row
                if (flips & 1) {
                    plain += cipher[i + 1];
                    plain += cipher[i];
                }
                else {
                    plain += cipher[i];
                    plain += cipher[i + 1];
                }
            }
            else {
                if (flips & 2) {
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += left[newY][newX] ;
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += right[newY][newX];
                }
                else {
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += left[newY][newX];
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += right[newY][newX];
                }
            }
        }
        return plain;
    }
}