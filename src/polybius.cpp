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
    //Lay out a string key containing all of a-z into a polybius square
    polybius makePolybius(std::string key) {
        polybius result{ -1 };
        int y = 0;
        int x = -1;
        for (char c : key) {
            if (c == 'j') { //Ignore j
                continue;
            }
            x++; //Go along the row
            if (x == 5) { //If have spilled over, go back to the start of the next row
                x = 0;
                y++;
            }
            result[y][x] = c; //Put the character in the polybius at the coordinates
        }
        return result;
    }

    //Linear search to find c in the key. Returns y, x not x, y as polybiuses are indexed [y][x]
    std::tuple<int, int> findInPolybius(char c, polybius key) {
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                if (key[y][x] == c) {
                    return { y, x };
                }
            }
        }
        return { -1, -1 };
    }

    //A helper function to swap two elements in a key. Return type is void as the key is passed by reference
    //Pointers to a distribution and random number generator are passed in to avoid cost of instantiating them every time this is called
    void swapElems(polybius& key, std::uniform_int_distribution<>* rd, std::mt19937* gen) {
        int x1 = (*rd)(*gen);
        int y1 = (*rd)(*gen);
        int x2 = (*rd)(*gen);
        int y2 = (*rd)(*gen);

        char _ = key[y1][x1];
        key[y1][x1] = key[y2][x2];
        key[y2][x2] = _;
    }

    //A helper function to swap two rows in a key. Return type is void as the key is passed by reference
    //Pointers to a distribution and random number generator are passed in to avoid cost of instantiating them every time this is called
    void swapRows(polybius& key, std::uniform_int_distribution<>* rd, std::mt19937* gen) {
        int y1 = (*rd)(*gen);
        int y2 = (*rd)(*gen);
        auto _ = key[y1];
        key[y1] = key[y2];
        key[y2] = _;
    }

    //A helper function to swap two columns in a key. Return type is void as the key is passed by reference
    //Pointers to a distribution and random number generator are passed in to avoid cost of instantiating them every time this is called
    void swapCols(polybius& key, std::uniform_int_distribution<>* rd, std::mt19937* gen) {
        int x1 = (*rd)(*gen);
        int x2 = (*rd)(*gen);
        for (int i = 0; i < 5; i++) {
            auto _ = key[i][x1];
            key[i][x1] = key[i][x2];
            key[i][x2] = _;
        }
    }

    //Flips a polybius along the y=x diagonal (given top-left (0, 0) right-right (4, 4))
    // Return type is void as the key is passed by reference
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

    //Flips a polybius vertically (i.e. in y=2)
    //Return type is void as the key is passed by reference
    void flipVert(polybius& key) {
        std::array<char, 5> _;
        _ = key[0];
        key[0] = key[4];
        key[4] = _;
        _ = key[1];
        key[1] = key[3];
        key[3] = _;
    }

    //Flips a polybius horizontally (i.e. in x=2)
    //Return type is void as the key is passed by reference
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
    
    //A helper function for modular arithmetic base 5 without instantiating lots of modularNumbers
    int removeFive(int n){
        n = n % 5;
        if (n < 0) {
            return n + 5;
        }
        return n;
    }

    //Decrypt function for standard playfair
    std::string playfairDecrypt(std::string text, polybius key)
    {
        std::string plain = "";
        plain.reserve(text.size());

        std::array<std::tuple<int, int>, 26> lookup;
        //Lookup table for char position, to reduce number of memory lookups by avoiding linear search
        for (char c = 97; c < 123; c++) {
            lookup[c - 97] = findInPolybius(c, key); //Stored in {y, x} form
        }

        //Tuples for position of chars in the ciphertext bigram and the plaintext bigram
        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        std::tuple<int, int> newpos0;
        std::tuple<int, int> newpos1;
        int l = text.size();
        for (int i = 0; i < l; i+=2) {
            pos0 = lookup[text[i] - 97]; //get the positions, subtract 97 as a=97 in ascii & unicode
            pos1 = lookup[text[i+1] - 97]; 
            if (std::get<0>(pos0) == std::get<0>(pos1)) { //Same row - y is equal - shift up
                newpos0 = { std::get<0>(pos0), removeFive(std::get<1>(pos0) - 1) };
                newpos1 = { std::get<0>(pos1), removeFive(std::get<1>(pos1) - 1) };
            }
            else if (std::get<1>(pos0) == std::get<1>(pos1)) { //Same column - x is equal - shift to the left
                newpos0 = { removeFive(std::get<0>(pos0) - 1), std::get<1>(pos0) };
                newpos1 = { removeFive(std::get<0>(pos1) - 1), std::get<1>(pos1) };
            }
            else { //Main case - corners of a rectangle
                newpos0 = { std::get<0>(pos0) , std::get<1>(pos1) };
                newpos1 = { std::get<0>(pos1) , std::get<1>(pos0) };
            }
            //Add characters to plain based on plain char positions
            plain += key[std::get<0>(newpos0)][std::get<1>(newpos0)];
            plain += key[std::get<0>(newpos1)][std::get<1>(newpos1)];
        }
        return plain;
    }


    //Decrypt function for the playfair variation used in 9B this year, effectively an exact copy of the above with a few minor tweaks
    std::string playfair2025VariationDecrypt(std::string text, polybius key)
    {
        std::string plain = "";
        plain.reserve(text.size());

        std::array<std::tuple<int, int>, 26> lookup;
        //Lookup table for char position, to reduce number of memory lookups by avoiding linear search
        for (char c = 97; c < 123; c++) {
            lookup[c - 97] = findInPolybius(c, key); //Stored in {y, x} form
        }
        
        //Tuples for position of chars in the ciphertext bigram and the plaintext bigram
        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        std::tuple<int, int> newpos0;
        std::tuple<int, int> newpos1;
        int l = text.size();
        for (int i = 0; i < l; i += 2) {
            pos0 = lookup[text[i] - 97]; //get the positions, subtract 97 as a=97 in ascii & unicode
            pos1 = lookup[text[i + 1] - 97];
            if (std::get<0>(pos0) == std::get<0>(pos1)) { //Same row - y is equal - shift to the right instead, with wrapping
                newpos0 = { std::get<0>(pos0), removeFive(std::get<1>(pos0) + 1) };
                newpos1 = { std::get<0>(pos1), removeFive(std::get<1>(pos1) + 1) };
            }
            else if (std::get<1>(pos0) == std::get<1>(pos1)) { //Same column - x is equal - shift to the right instead, with wrapping 
                newpos0 = { std::get<0>(pos0), removeFive(std::get<1>(pos0) + 1) };
                newpos1 = { std::get<0>(pos1), removeFive(std::get<1>(pos1) + 1) };
            }
            else { //Normal case, unchanged - corners of a rectangle
                newpos0 = { std::get<0>(pos0) , std::get<1>(pos1) };
                newpos1 = { std::get<0>(pos1) , std::get<1>(pos0) };
            }
            //Add characters to plain based on plain char positions
            plain += key[std::get<0>(newpos0)][std::get<1>(newpos0)];
            plain += key[std::get<0>(newpos1)][std::get<1>(newpos1)];
        }
        return plain;
    }

    //A helper function to remove the excess 'x's from a playfair decryption to make tetragram fitness more accurate
    std::string processPlayfairDecrypt(std::string decrypt) {
        int n = 0; //Start search from the beginning
        while (true) {
            n = decrypt.find('x', n);
            if (n == std::string::npos) { //No more 'x's from position n onwards, so return
                return decrypt;
            }
            if (n + 1 == decrypt.length()) { //Only 'x' is the last letter, probably a null so remove it and return
                return decrypt.substr(0, n);
            }
            if (decrypt[n - 1] == decrypt[n + 1]) { //is it a double letter
                decrypt = decrypt.substr(0, n) + decrypt.substr(n + 1); //remove the x
            }
            else { //Increment n as we can ignore this 'x'
                n++;
            }
        }
    }

    //Hill climbing attack on the playfair cipher
    //This uses the "best-current-child key with improver" setup
    //This is based on a mixture of Madness's attack the book, a load of articles I read online and my own experimentation
    polybius playfairHillClimber(std::string cipher)
    {
        std::ios_base::sync_with_stdio(false); //Makes std::cout faster so it isn't a bottleneck to print status updates

        cipher = basics::formatString(cipher);

        //Best and current vars
        polybius bestKey; //An arbitrary starting key
        bestKey[0] = { 'p', 'o', 'l', 'y', 'b' };
        bestKey[1] = { 'i', 'u', 's', 'a', 'c' };
        bestKey[2] = { 'd', 'e', 'f', 'g', 'h' };
        bestKey[3] = { 'k', 'm', 'n', 'q', 'r' };
        bestKey[4] = { 't', 'v', 'w', 'x', 'z' };
        polybius currentKey = bestKey;

        std::string bestDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, bestKey));

        float bestFitness = fitness::tetragramFitness(&bestDecrypt);

        float currentFitness = bestFitness;

        //Child vars
        polybius childKey;
        std::string childDecrypt;
        float childFitness;

        //Loop control vars
        int counter = 0;
        int impatience = 0;
        bool wandering = false;

        //For pseudo-random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 50);

        float chance;

        while (counter < 2000000) {
            childKey = currentKey;
            
            //Change the key
            switch (changeChoice(gen)) { //Random num 1 - 50
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

            //Decrypt using child key and get fitness
            childDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, childKey));
            childFitness = fitness::tetragramFitness(&childDecrypt);

            if (childFitness > currentFitness) {  // If it's at all promising recursively improve it
                childKey = playfairImprover(cipher, childKey, false);
                childDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, childKey));
                childFitness = fitness::tetragramFitness(&childDecrypt);
            }

            if (childFitness > bestFitness) {
                std::cout << childFitness << std::endl;

                //Because of the improver I can exit confidently here
                //Experiments in python showed that any key ~6 steps or less from the best key can be improved to the best key without stepping back
                //Hence if the improved key gives very high fitness, it must be best
                if (childFitness > -13) {
                    return childKey;
                }

                //Update best and current vars
                bestKey = childKey;
                bestFitness = childFitness;
                bestDecrypt = childDecrypt;
                currentKey = childKey;
                currentFitness = childFitness;

                //Update loop control
                counter = 0;
                impatience = 0;
                wandering = false;
            }

            else if (childFitness >= currentFitness) {
                if (childFitness == bestFitness) {
                    impatience = 0;
                    wandering = false;
                }

                currentKey = childKey;
                currentFitness = childFitness;
            }

            else {
                //'Magic' formula I came up with a while ago based on bestFitness and childFitness
                //Provides a nice gradient - small steps are very likely, while larger steps are increasingly less so as bestFitness increases
                chance = -(((-childFitness / 8.0f) - 1.0f) / (childFitness - bestFitness)) * 3.0f + 0.5f; 
                if (changeChoice(gen) < chance) {
                    currentKey = childKey;
                    currentFitness = childFitness;
                    wandering = true;
                }
            }

            counter++;

            if (wandering) {
                impatience++;
            }

            if (impatience > 2000) { //Dewander the key if its been wandering for a long time
                currentKey = bestKey;
                currentFitness = bestFitness;
                impatience = 0;
                wandering = false;
            }
        }

        std::ios_base::sync_with_stdio(true); //Resyncs std::cout to prevent issues elsewhere

        return bestKey;
    }

    //A copy of the above but using 2025Variation decryption and improver
    polybius playfair2025VariationHillClimber(std::string cipher)
    {
        std::ios_base::sync_with_stdio(false); //Makes std::cout faster so it isn't a bottleneck to print status updates

        cipher = basics::formatString(cipher);

        //Best and current vars
        polybius bestKey; //An arbitrary starting key
        bestKey[0] = { 'p', 'o', 'l', 'y', 'b' };
        bestKey[1] = { 'i', 'u', 's', 'a', 'c' };
        bestKey[2] = { 'd', 'e', 'f', 'g', 'h' };
        bestKey[3] = { 'k', 'm', 'n', 'q', 'r' };
        bestKey[4] = { 't', 'v', 'w', 'x', 'z' };
        polybius currentKey = bestKey;

        std::string bestDecrypt = processPlayfairDecrypt(playfair2025VariationDecrypt(cipher, bestKey));
        float bestFitness = fitness::tetragramFitness(&bestDecrypt);
        float currentFitness = bestFitness;

        //Child vars
        polybius childKey;
        std::string childDecrypt;
        float childFitness;

        //Loop control vars
        int counter = 0;
        int impatience = 0;
        bool wandering = false;

        float chance;

        //For pseudo-random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 50);
        
        while (counter < 2000000) {
            childKey = currentKey;

            //Change the key with the key here
            switch (changeChoice(gen)) { //Random num 1-50
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

            //Decrypt
            childDecrypt = processPlayfairDecrypt(playfair2025VariationDecrypt(cipher, childKey));
            childFitness = fitness::tetragramFitness(&childDecrypt);

            if (childFitness > currentFitness) { // If it's at all promising recursively improve it
                childKey = playfair2025VariationImprover(cipher, childKey, false);
                childDecrypt = processPlayfairDecrypt(playfair2025VariationDecrypt(cipher, childKey));
                childFitness = fitness::tetragramFitness(&childDecrypt);
            }

            if (childFitness > bestFitness) {
                std::cout << childFitness << std::endl;

                //Because of the recursive improver I can exit confidently here
                //Experiments in python showed that any key ~6 steps or less from the best key can be recursively improved to the best key without stepping back
                //Hence if the improved key gives very high fitness, it must be best
                if (childFitness > -13) {
                    return childKey;
                }

                //Update best and current vars
                bestKey = childKey;
                bestFitness = childFitness;
                bestDecrypt = childDecrypt;
                currentKey = childKey;
                currentFitness = childFitness;

                //Update loop control
                counter = 0;
                impatience = 0;
                wandering = false;
            }

            else if (childFitness > currentFitness) {
                if (childFitness == bestFitness) {
                    impatience = 0;
                    wandering = false;
                }
                currentKey = childKey;
                currentFitness = childFitness;
            }

            else {
                //'Magic' formula I came up with a while ago based on bestFitness and childFitness
                //Provides a nice gradient - small steps are very likely, while larger steps are increasingly less so as bestFitness increases
                chance = -(((-childFitness / 8.0f) - 1.0f) / (childFitness - bestFitness)) * 3.0f + 0.5f;
                if (changeChoice(gen) < chance) {
                    currentKey = childKey;
                    currentFitness = childFitness;
                    wandering = true;
                }
            }

            counter++;
            if (wandering) {
                impatience++;
            }

            if (impatience > 2000) { //Dewander the key if its been wandering for a long time
                currentKey = bestKey;
                currentFitness = bestFitness;
                impatience = 0;
                wandering = false;
            }
        }

        std::ios_base::sync_with_stdio(true); //Resyncs std::cout to prevent issues elsewhere

        return bestKey;
    }

    //A helper function to get every possible child key that the options to change a playfair key could produce
    std::vector<polybius> getAllChildKeysPlayfair(polybius key) {
        auto keys = std::vector<polybius>();
        polybius childKey;

        //Swap two places
        for (int y1 = 0; y1 < 5; y1++) {
            for (int y2 = 0; y2 < 5; y2++) {
                for (int x1 = 0; x1 < 5; x1++) {
                    for (int x2 = 0; x2 < 5; x2++) {
                        if (x1 == x2 && y1 == y2) { //Don't return a copy of the original key
                            continue;
                        }
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

    //A helper function for the hillclimbing attack that repeatedly takes the best improvement available until there are no more and it returns the key.
    //This is a flattened recursive algorithm, so its a bit weird-looking
    //ignoreBad is a flag for if a result with fitness below -15 should be ignored and a null result given instead of returning it anyway
    polybius playfairImprover(std::string cipher, polybius startKey, bool ignoreBad) {
        //Get a starting point
        auto decrypt = processPlayfairDecrypt(playfairDecrypt(cipher, startKey));
        float fitness = fitness::tetragramFitness(&decrypt);
        polybius bestKey = startKey;
        float bestFitness = fitness;
        float childFitness;

        //Empty variables to store stuff
        std::vector<polybius> children;
        polybius bestChild;

        while (true) {
            children = getAllChildKeysPlayfair(bestKey); //Get all children of current best
            fitness = -100; //Anything that looks at all like English will be better as I defined log 0 as -100 for tetragram fitness
            bestChild = nullPolybius;
            for (const polybius& child : children) { //Find the best child
                decrypt = processPlayfairDecrypt(playfairDecrypt(cipher, child));
                childFitness = fitness::tetragramFitness(&decrypt);
                if (childFitness > fitness) {
                    fitness = childFitness;
                    bestChild = child;
                }
            }
            if (fitness > bestFitness) { //If the best this depth is better than the best in all prior searches, update it
                bestKey = bestChild;
                bestFitness = fitness;
            }
            else if (bestFitness > -15 || !ignoreBad) { //Else return bestKey, unless we are ignoring 'bad' keys and fitness < -15
                return bestKey;
            }
            else { //Return a nullPolybius, only happens if the above was false, so fitness < -15 and we are ignoring 'bad' keys
                return nullPolybius;
            }
        }
    }

    //A version of the above for the 2025 9B variation
    polybius playfair2025VariationImprover(std::string cipher, polybius startKey, bool ignoreBad) {
        //Get a starting point
        auto decrypt = processPlayfairDecrypt(playfair2025VariationDecrypt(cipher, startKey));
        float fitness = fitness::tetragramFitness(&decrypt);
        polybius bestKey = startKey;
        float bestFitness = fitness;
        float childFitness;

        //Empty variables to store stuff
        std::vector<polybius> children;
        polybius bestChild;

        while (true) {
            children = getAllChildKeysPlayfair(bestKey); //Get all children of current best
            fitness = -100; //Anything that looks at all like English will be better as I defined log 0 as -100 for tetragram fitness
            bestChild = nullPolybius;
            for (const polybius& child : children) { //Check if any of the children are better than the others in this depth
                decrypt = processPlayfairDecrypt(playfair2025VariationDecrypt(cipher, child));
                childFitness = fitness::tetragramFitness(&decrypt);
                if (childFitness > fitness) {
                    fitness = childFitness;
                    bestChild = child;
                }
            }
            if (fitness > bestFitness) { //If the best this depth is better than the best in all prior searches, update it
                bestKey = bestChild;
                bestFitness = fitness;
            }
            else if (bestFitness > -15 || !ignoreBad) { //Else return bestKey, unless we are ignoring 'bad' keys and fitness < -15
                return bestKey;
            }
            else { //Return a nullPolybius, only happens if the above was false, so fitness < -15 and we are ignoring 'bad' keys
                return nullPolybius;
            }
        }
    }

    //CLI interface code for playfair hill climber
    int cliPlayfairHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

        polybius bestKey = playfairHillClimber(cipher); //Run attack
        //Print key
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
                std::cout << rawDecrypt << std::endl; //Print raw decrypt so excess 'x's can be included in a decrypt
                return 1; //Success
            }
        }
        return 0; //Failure
    }

    //CLI interface code for playfair 2025 9B variation hill climber
    int cliPlayfair2025VariationHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

        polybius bestKey = playfair2025VariationHillClimber(cipher); //Run attack 

        //Print key
        for (const auto& row : bestKey) {
            for (const auto& item : row) {
                std::cout << item << " ";
            }
            std::cout << std::endl;
        }

        std::string rawDecrypt = playfair2025VariationDecrypt(cipher, bestKey);
        std::string decrypt = processPlayfairDecrypt(rawDecrypt);

        if (fitness::tetragramFitness(&decrypt) > -15) {
            if (cliInterface::offerDecryption(decrypt)) {
                std::cout << "Raw Decrypt: " << std::endl;
                std::cout << rawDecrypt << std::endl; //Print raw decrypt so excess 'x's can be included in a decrypt
                return 1; //Success
            }
        }
        return 0; //Failure
    }

    //Decryption code for the vertical two square cipher
    //Can also be used to encrypt as they are the same process
    //Flips is a binary flag that is 0 by default. 
    // The '1' bit controls if characters in the same column have their order reversed.
    // The '2' bit controls if characters in the normal case have their order reversed.
    std::string vertTwoSquareDecrypt(std::string cipher, polybius top, polybius right, int flips) {
        std::string plain = "";
        plain.reserve(cipher.size());

        //Lookup table for char positions
        std::array<std::tuple<int, int>, 26> topLookup;
        for (char c = 97; c < 123; c++) { //a=97, z=122
            topLookup[c - 97] = findInPolybius(c, top);
        }
        std::array<std::tuple<int, int>, 26> rightLookup;
        for (char c = 97; c < 123; c++) { //a=97, z=122
            rightLookup[c - 97] = findInPolybius(c, right);
        }

        //Variables for temp storage in the loop
        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        int newX;
        int newY;
        int l = cipher.size(); //Limit on loop
        for (int i = 0; i < l; i += 2) {
            pos0 = topLookup[cipher[i] - 97];
            pos1 = rightLookup[cipher[i + 1] - 97];
            if (std::get<1>(pos0) == std::get<1>(pos1)) { //Same column
                if (flips & 1) { //If flipped, using bitwise operations to detect if that bit is 'on'
                    plain += cipher[i + 1];
                    plain += cipher[i];
                }
                else { //Else
                    plain += cipher[i];
                    plain += cipher[i + 1];
                }
            }
            else {
                if (flips & 2) { //If flipped, using bitwise operations to detect if that bit is 'on'
                    //Get other corners of the rectangle
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += right[newY][newX];
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += top[newY][newX];
                }
                else { //Else
                    //Get other corners of the rectangle
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += top[newY][newX];
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += right[newY][newX];
                }
            }
        }
        return plain;
    }

    //Encryption code for the horizontal two square cipher. This isn't actually used anywhere, but it exists if ever needed
    // Flips is a binary flag that is 1 by default. 
    // The '1' bit controls if characters in the same column have their order reversed.
    // The '2' bit controls if characters in the normal case have their order reversed.
    std::string horizTwoSquareEncrypt(std::string cipher, polybius left, polybius right, int flips) {
        std::string plain = "";
        plain.reserve(cipher.size());

        //Lookup table for char positions
        std::array<std::tuple<int, int>, 26> leftLookup;
        for (char c = 97; c < 123; c++) {
            leftLookup[c - 97] = findInPolybius(c, left);
        }
        std::array<std::tuple<int, int>, 26> rightLookup;
        for (char c = 97; c < 123; c++) {
            rightLookup[c - 97] = findInPolybius(c, right);
        }

        //Temp storage for the loop
        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        int newX;
        int newY;

        //Limit
        int l = cipher.size();
        for (int i = 0; i < l; i += 2) {
            pos0 = leftLookup[cipher[i] - 97]; //Get positions
            pos1 = rightLookup[cipher[i + 1] - 97];
            if (std::get<0>(pos0) == std::get<0>(pos1)) { //Same row
                if (flips & 1) { //If flipped
                    plain += cipher[i + 1];
                    plain += cipher[i];
                }
                else {
                    plain += cipher[i];
                    plain += cipher[i + 1];
                }
            }
            else { 
                if (flips & 2) { //If flipped
                    //Get other corners of the rectangle
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += right[newY][newX];
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += left[newY][newX];
                }
                else {
                    //Get other corners of the rectangle
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

    //Decryption code for the horizontal two square cipher
    //Flips is a binary flag that is 1 by default. 
    // The '1' bit controls if characters in the same column have their order reversed.
    // The '2' bit controls if characters in the normal case have their order reversed.
    std::string horizTwoSquareDecrypt(std::string cipher, polybius left, polybius right, int flips) {
        std::string plain = "";
        plain.reserve(cipher.size());

        //Lookup table for char positions
        std::array<std::tuple<int, int>, 26> leftLookup;
        for (char c = 97; c < 123; c++) {
            leftLookup[c - 97] = findInPolybius(c, left);
        }
        std::array<std::tuple<int, int>, 26> rightLookup;
        for (char c = 97; c < 123; c++) {
            rightLookup[c - 97] = findInPolybius(c, right);
        }

        //Temp storage inside loop
        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;
        int newX;
        int newY;

        //Limit
        int l = cipher.size();
        for (int i = 0; i < l; i += 2) {
            pos0 = rightLookup[cipher[i] - 97]; //Get coords in grids
            pos1 = leftLookup[cipher[i + 1] - 97];
            if (std::get<0>(pos0) == std::get<0>(pos1)) { //Same row
                if (flips & 1) { //If flipped
                    plain += cipher[i + 1];
                    plain += cipher[i];
                }
                else {
                    plain += cipher[i];
                    plain += cipher[i + 1];
                }
            }
            else {
                if (flips & 2) { //If flipped
                    //Get other corners of the rectangle
                    newX = std::get<1>(pos0);
                    newY = std::get<0>(pos1);
                    plain += left[newY][newX] ;
                    newX = std::get<1>(pos1);
                    newY = std::get<0>(pos0);
                    plain += right[newY][newX];
                }
                else {
                    //Get other corners of the rectangle
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

    //Get all child keys for a given two-square key.
    //This returns every possible swap on the first key with the second key unchanged and every possible swap on the second key with the first key unchanged
    std::vector<std::tuple<polybius, polybius>> getAllChildKeysTwoSquare(polybius key1, polybius key2) {
        auto keys = std::vector<std::tuple<polybius, polybius>>();
        polybius childKey;
        char _;

        //Swap two places in first square
        for (int y1 = 0; y1 < 5; y1++) {
            for (int y2 = 0; y2 < 5; y2++) {
                for (int x1 = 0; x1 < 5; x1++) {
                    for (int x2 = 0; x2 < 5; x2++) {
                        if (x1 == x2 && y1 == y2) { //Don't return a copy of the original key
                            continue;
                        }
                        childKey = key1;
                        _ = childKey[y1][x1];
                        childKey[y1][x1] = childKey[y2][x2];
                        childKey[y2][x2] = _;
                        keys.push_back({ childKey, key2 });
                    }
                }
            }
        }

        //Swap two places in second square
        for (int y1 = 0; y1 < 5; y1++) {
            for (int y2 = 0; y2 < 5; y2++) {
                for (int x1 = 0; x1 < 5; x1++) {
                    for (int x2 = 0; x2 < 5; x2++) {
                        if (x1 == x2 && y1 == y2) { //Don't return a copy of the original key
                            continue;
                        }
                        childKey = key2;
                        _ = childKey[y1][x1];
                        childKey[y1][x1] = childKey[y2][x2];
                        childKey[y2][x2] = _;
                        keys.push_back({ key1, childKey });
                    }
                }
            }
        }

        return keys;
    }
    
    //A hill-climbing attack on the vertical two-square cipher.
    //It uses the "best-current-child key with improver" setup, but as a shotgun hill-climber which restarts regularly if stuck
    //Not the best, but it works eventually, and quite quickly for longer (at the very least cipher challenge type length) texts
    std::tuple<polybius, polybius> vertTwoSquareHillClimber(std::string cipher) { 
        cipher = basics::formatString(cipher);

        std::string alphabetCopy = basics::alphabet;

        //Set up a lot of memory allocations here
        //Best
        polybius bestTopKey;
        polybius bestBottomKey;
        std::string bestDecrypt;
        float bestFitness;
        
        //Current
        polybius currentTopKey;
        polybius currentBottomKey;
        std::string currentDecrypt;
        float currentFitness;

        //Child
        polybius childTopKey;
        polybius childBottomKey;
        std::string childDecrypt;
        float childFitness;

        //For improving
        std::tuple<polybius, polybius> improvedKey; //As the improver uses a tuple, which can't be transformed directly

        //Random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> squareChoice(0, 1);
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 5);
        
        //Loop control
        int counter = 0;
        int impatience = 0;

        bool wandering;
        
        int N = 0; //Counter for shotgun loop
        while (N < 1000) {
            std::cout << std::endl << "Restart " << N << std::endl;

            //Generate a random starting key pair
            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen); 
            bestTopKey = makePolybius(alphabetCopy);

            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestBottomKey = makePolybius(alphabetCopy);

            //Generate bestDecrypt and bestFitness from random key
            bestDecrypt = vertTwoSquareDecrypt(cipher, bestTopKey, bestBottomKey);
            bestFitness = fitness::tetragramFitness(&bestDecrypt);

            //Copy to current
            currentTopKey = bestTopKey;
            currentBottomKey = bestBottomKey;
            currentDecrypt = bestDecrypt;
            currentFitness = bestFitness;

            //Set loop controls to 0 / off
            counter = 0;
            impatience = 0;
            wandering = false;

            while (counter < 20000 || (bestFitness > -37 && counter < (20000 + ((37 + bestFitness) * 10000)))) { //Allows more iterations as bestFitness gets above -37
                childTopKey = currentTopKey; //Copy to childKey
                childBottomKey = currentBottomKey;

                if (squareChoice(gen) == 0) { //Flip a coin
                    swapElems(childTopKey, &dist, &gen); //If heads flip in the top square
                }
                else {
                    swapElems(childBottomKey, &dist, &gen); //If tails flip in the bottom square
                }

                //Decrypt and get new fitness
                childDecrypt = vertTwoSquareDecrypt(cipher, childTopKey, childBottomKey); 
                childFitness = fitness::tetragramFitness(&childDecrypt);

                if (childFitness > currentFitness) { //If the key shows any form of promise
                    //Improve it
                    improvedKey = vertTwoSquareImprover(cipher, childTopKey, childBottomKey, false);
                    childTopKey = std::get<0>(improvedKey);
                    childBottomKey = std::get<1>(improvedKey);

                    //Decrypt again and get new fitness
                    childDecrypt = vertTwoSquareDecrypt(cipher, childTopKey, childBottomKey);
                    childFitness = fitness::tetragramFitness(&childDecrypt);
                }

                if (childFitness > bestFitness) {
                    wandering = false;

                    bestFitness = childFitness;
                    currentFitness = childFitness;

                    bestDecrypt = childDecrypt;
                    currentDecrypt = childDecrypt;

                    bestTopKey = childTopKey;
                    currentTopKey = childTopKey;
                    bestBottomKey = childBottomKey;
                    currentBottomKey = childBottomKey;

                    std::cout << bestFitness << " " << counter << std::endl;

                    counter = 0;
                    impatience = 0;
                }
                else if (childFitness > currentFitness) {
                    if (childFitness == bestFitness) {
                        impatience = 0;
                        wandering = false;
                    }
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentTopKey = childTopKey;
                    currentBottomKey = childBottomKey;
                }
                else if (counter > 100 && childFitness > (bestFitness + (bestFitness/6)) && changeChoice(gen) == 1) { //Randomly step back, with permissible range set to be 7/6 of the bestFitness and a 20% chance
                    currentTopKey = childTopKey;
                    currentBottomKey = childBottomKey;
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    wandering = true;
                }

                if (impatience > 2000) { //De-wander the key if it has wandered too far without improvement
                    currentTopKey = bestTopKey;
                    currentBottomKey = bestBottomKey;
                    impatience = 0;
                    wandering = false;
                }

                counter++;
                if (wandering) {
                    impatience++;
                }

                if (bestFitness > -12 && counter > 10000) { //Break if its clearly a good key
                    break;
                }
            }

            if (fitness::tetragramFitness(&bestDecrypt) > -15) { //If its a good key, return it (and break as a result)
                return { bestTopKey, bestBottomKey };
            }

            N++; //Increment the outer counter
        }

        return { nullPolybius, nullPolybius }; //Return a null result
    }

    //A helper function for the vertical two-square hill-climbing attack that repeatedly takes the best improvement until there are no more and returns the resultant key
    //This is very similar to playfairImprover
    //ignoreBad is a flag for if a result with fitness below -15 should be ignored and a null result given instead of returning it anyway
    std::tuple<polybius, polybius> vertTwoSquareImprover(std::string cipher, polybius startTop, polybius startRight, bool ignoreBad) {
        //Get a starting point
        auto decrypt = vertTwoSquareDecrypt(cipher, startTop, startRight);
        float fitness = fitness::tetragramFitness(&decrypt);
        std::tuple<polybius, polybius> bestKey = { startTop, startRight };
        float bestFitness = fitness;
        float childFitness;

        //Empty vars to store stuff
        std::vector<std::tuple<polybius, polybius>> children;
        std::tuple<polybius, polybius> bestChild;

        while (true) {
            children = getAllChildKeysTwoSquare(startTop, startRight); //Get all children of current key
            fitness = -100; //Anything is better than this in practice
            bestChild = { nullPolybius, nullPolybius };
            for (const std::tuple<polybius, polybius>& child : children) { //Find the best child
                decrypt = vertTwoSquareDecrypt(cipher, std::get<0>(child), std::get<1>(child));
                childFitness = fitness::tetragramFitness(&decrypt);
                if (childFitness > fitness) {
                    fitness = childFitness;
                    bestChild = child;
                }
            }
            if (fitness > bestFitness) { //If the best this depth is better than the best in all prior searches, update it
                bestKey = bestChild;
                bestFitness = fitness;
            }
            else if (bestFitness > -15 || !ignoreBad) { //Else return bestkey, unless we are ignoring bad and the fitness < -15
                return bestKey;
            }
            else { //Return a null result, only happes if the above is false, so fitness <= -15 and ignoreBad is true
                return { nullPolybius, nullPolybius };
            }
        }
    }

    //CLI interface code for the vertical two-square hill-climber
    int cliVertTwoSquareHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

        auto bestKeys = vertTwoSquareHillClimber(cipher);
        if (std::get<0>(bestKeys) != nullPolybius) { //Check its not a null result
            std::string decrypt = vertTwoSquareDecrypt(cipher, std::get<0>(bestKeys), std::get<1>(bestKeys));
            if (fitness::tetragramFitness(&decrypt) > -15) {
                if (cliInterface::offerDecryption(decrypt)) {
                    return 1; //Success
                }
            }
        }
        return 0; //Failure
    }

    //A hill-climbing attack on the horizontal two-square cipher.
    //It uses the "best-current-child key with improver" setup, but as a shotgun hill-climber which restarts regularly if stuck
    //Not the best, but it works eventually, and quite quickly for longer (at the very least cipher challenge type length) texts
    std::tuple<polybius, polybius> horizTwoSquareHillClimber(std::string cipher) { //Not the best, but it works for fair-sized texts
        cipher = basics::formatString(cipher);

        std::string alphabetCopy = basics::alphabet;

        //Set up some memory allocations here
        //Best
        polybius bestLeftKey;
        polybius bestRightKey;
        std::string bestDecrypt;
        float bestFitness;

        //Current
        polybius currentLeftKey;
        polybius currentRightKey;
        std::string currentDecrypt;
        float currentFitness;

        //Child
        polybius childLeftKey;
        polybius childRightKey;
        std::string childDecrypt;
        float childFitness;

        //For improving
        std::tuple<polybius, polybius> improvedKey; //As the improver uses a tuple, which can't be transformed directly

        //Random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> squareChoice(0, 1);
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 5);

        //Loop control
        int counter = 0;
        int impatience = 0;

        bool wandering;

        int N = 0;//Counter for shotgun loop
        while (N < 1000) {
            std::cout << std::endl << "Restart " << N << std::endl;

            //Generate a random starting key pair
            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestLeftKey = makePolybius(alphabetCopy);

            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestRightKey = makePolybius(alphabetCopy);

            //Generate bestDecrypt and bestFitness from random key
            bestDecrypt = horizTwoSquareDecrypt(cipher, bestLeftKey, bestRightKey);
            bestFitness = fitness::tetragramFitness(&bestDecrypt);

            //Copy to current
            currentLeftKey = bestLeftKey;
            currentRightKey = bestRightKey;
            currentDecrypt = bestDecrypt;
            currentFitness = bestFitness;

            //Reset loop controls
            counter = 0;
            impatience = 0;

            wandering = false;

            while (counter < 20000 || (bestFitness > -37 && counter < (20000 + ((37 + bestFitness) * 10000)))) { //Allows more iterations as bestFitness gets above -37
                childLeftKey = currentLeftKey; //Copy to childKey
                childRightKey = currentRightKey;

                if (squareChoice(gen) == 0) { //Flip a coin
                    swapElems(childLeftKey, &dist, &gen); //If heads flip in the left square
                }
                else {
                    swapElems(childRightKey, &dist, &gen); //If tails flip in the right square
                }

                //Decrypt and get a new fitness
                childDecrypt = horizTwoSquareDecrypt(cipher, childLeftKey, childRightKey);
                childFitness = fitness::tetragramFitness(&childDecrypt);

                if (childFitness > currentFitness) { //If the key shows any form of promise
                    //Improve it
                    improvedKey = horizTwoSquareImprover(cipher, childLeftKey, childRightKey, false);
                    childLeftKey = std::get<0>(improvedKey);
                    childRightKey = std::get<1>(improvedKey);

                    //Decrypt again and get new fitness
                    childDecrypt = horizTwoSquareDecrypt(cipher, childLeftKey, childRightKey);
                    childFitness = fitness::tetragramFitness(&childDecrypt);
                }

                if (childFitness > bestFitness) {
                    wandering = false;

                    bestFitness = childFitness;
                    currentFitness = childFitness;

                    bestDecrypt = childDecrypt;
                    currentDecrypt = childDecrypt;

                    bestLeftKey = childLeftKey;
                    currentLeftKey = childLeftKey;
                    bestRightKey = childRightKey;
                    currentRightKey = childRightKey;

                    std::cout << bestFitness << " " << counter << std::endl;

                    counter = 0;
                    impatience = 0;
                }
                else if (childFitness > currentFitness) {
                    if (childFitness == bestFitness) {
                        impatience = 0;
                        wandering = false;
                    }
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentLeftKey = childLeftKey;
                    currentRightKey = childRightKey;
                }
                else if (counter > 100 && childFitness > (bestFitness + (bestFitness / 6)) && changeChoice(gen) == 1) { //Randomly step back, with permissible range set to be 7/6 of the bestFitness and a 20% chance
                    currentLeftKey = childLeftKey;
                    currentRightKey = childRightKey;
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    wandering = true;
                }

                if (impatience > 2000) {
                    currentLeftKey = bestLeftKey;
                    currentRightKey = bestRightKey;
                    impatience = 0;
                    wandering = false;
                }

                counter++;
                if (wandering) { //De-wander the key if it has wandered too far without improvement
                    impatience++;
                }

                if (bestFitness > -12 && counter > 10000) { //Break if its clearly a good key
                    break;
                }
            }

            if (fitness::tetragramFitness(&bestDecrypt) > -15) { //If its a good key, return it (and break as a result)
                return { bestLeftKey, bestRightKey };
            }

            N++; //Increment the outer counter
        }

        return { nullPolybius, nullPolybius }; //Return a null result
    }

    //A helper function for the horizontal two-square hill-climbing attack that repeatedly takes the best improvement until there are no more and returns the resultant key
    //This is very similar to playfairImprover
    //ignoreBad is a flag for if a result with fitness below -15 should be ignored and a null result given instead of returning it anyway
    std::tuple<polybius, polybius> horizTwoSquareImprover(std::string cipher, polybius startLeft, polybius startRight, bool ignoreBad) {
        //Get a starting point
        auto decrypt = horizTwoSquareDecrypt(cipher, startLeft, startRight);
        float fitness = fitness::tetragramFitness(&decrypt);
        std::tuple<polybius, polybius> bestKey = { startLeft, startRight };
        float bestFitness = fitness;
        float childFitness;

        //Empty vars to store stuff
        std::vector<std::tuple<polybius, polybius>> children;
        std::tuple<polybius, polybius> bestChild;

        while (true) {
            children = getAllChildKeysTwoSquare(startLeft, startRight); //Get all children of current key
            fitness = -100; //Anything is better than this in practice
            bestChild = { nullPolybius, nullPolybius };
            for (const std::tuple<polybius, polybius>& child : children) { //Find the best child
                decrypt = horizTwoSquareDecrypt(cipher, std::get<0>(child), std::get<1>(child));
                childFitness = fitness::tetragramFitness(&decrypt);
                if (childFitness > fitness) {
                    fitness = childFitness;
                    bestChild = child;
                }
            }
            if (fitness > bestFitness) { //If the best this depth is better than the best in all prior searches, update it
                bestKey = bestChild;
                bestFitness = fitness;
            }
            else if (bestFitness > -15 || !ignoreBad) { //Else return bestkey, unless we are ignoring bad and the fitness < -15
                return bestKey;
            }
            else { //Return a null sesult, only happes if the above is false, so fitness <= -15 and ignoreBad is true
                return { nullPolybius, nullPolybius };
            }
        }
        return bestKey;
    }

    //CLI interface code for the horizontal two-square hill-climber
    int cliHorizTwoSquareHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

        auto bestKeys = horizTwoSquareHillClimber(cipher);
        if (std::get<0>(bestKeys) != nullPolybius) { //Check its not a null result
            std::string decrypt = horizTwoSquareDecrypt(cipher, std::get<0>(bestKeys), std::get<1>(bestKeys));
            if (fitness::tetragramFitness(&decrypt) > -15) {
                if (cliInterface::offerDecryption(decrypt)) {
                    return 1; //Success
                }
            }
        }
        return 0; //Failure
    }

    //Decryption code for the four square cipher
    std::string fourSquareDecrypt(std::string text, polybius topRight, polybius bottomLeft) {
        auto blocks = strings::getBlocks(text, 2);

        std::string newText = "";
        newText.reserve(text.size());

        //Lookup table for char positions
        std::array<std::tuple<int, int>, 26> topRightLookup;
        for (char c = 97; c < 123; c++) {
            topRightLookup[c - 97] = findInPolybius(c, topRight);
        }
        std::array<std::tuple<int, int>, 26> bottomLeftLookup;
        for (char c = 97; c < 123; c++) {
            bottomLeftLookup[c - 97] = findInPolybius(c, bottomLeft);
        }

        //Temp storage inside loop
        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;

        for (const auto& block : blocks) {
            pos0 = topRightLookup[block[0]-97];
            pos1 = bottomLeftLookup[block[1]-97];

            //This cipher is quite simple, just use the other corners of the rectangle
            newText += alphabetPolybius[std::get<0>(pos0)][std::get<1>(pos1)];
            newText += alphabetPolybius[std::get<0>(pos1)][std::get<1>(pos0)];
        }

        return newText;
    }

    //A hill-climbing attack on the four-square cipher.
    //This uses a shotgun version of the "basic best-current-child key" setup
    //Though it is more for reliability than a small chance of the attack being successful
    std::tuple<polybius, polybius> fourSquareHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        //Random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> squareChoice(0, 1);
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 50);

        std::string alphabetCopy = basics::alphabet;

        //Memory allocations
        //Best and current
        polybius bestTopRightKey, bestBottomLeftKey, currentTopRightKey, currentBottomLeftKey;
        std::string bestDecrypt;
        std::string currentDecrypt;
        float bestFitness;
        float currentFitness;

        //Child
        polybius childTopRightKey, childBottomLeftKey;
        std::string childDecrypt;
        float childFitness;

        //Loop control
        int counter;
        int impatience;

        bool wandering;

        for (int i = 0; i < 20; i++) { //It can take a few attempts
            //Randomised starting key
            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestTopRightKey = makePolybius(alphabetCopy);
            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestBottomLeftKey = makePolybius(alphabetCopy);

            //Generate decrypt and fitness
            bestDecrypt = fourSquareDecrypt(cipher, bestTopRightKey, bestBottomLeftKey);
            bestFitness = fitness::tetragramFitness(&bestDecrypt);

            //Copy to current
            currentTopRightKey = bestTopRightKey;
            currentBottomLeftKey = bestBottomLeftKey;
            currentDecrypt = bestDecrypt;
            currentFitness = bestFitness;
            
            //Reset loop control
            counter = 0;
            impatience = 0;
            wandering = false;

            std::cout << "Iteration " << i << std::endl;

            while (counter < 200000) {
                childTopRightKey = currentTopRightKey; //Copy to child key
                childBottomLeftKey = currentBottomLeftKey;

                if (squareChoice(gen) == 0) { //Flip a coin to choose a key to modify
                    switch (changeChoice(gen)) { //This works just like in the playfair attacks
                    case 1:
                        flipDiag(childTopRightKey);
                        break;
                    case 2:
                        flipHoriz(childTopRightKey);
                        break;
                    case 3:
                        flipDiag(childTopRightKey);
                        break;
                    case 4:
                        swapRows(childTopRightKey, &dist, &gen);
                        break;
                    case 5:
                        swapCols(childTopRightKey, &dist, &gen);
                        break;
                    default:
                        swapElems(childTopRightKey, &dist, &gen);
                        break;
                    }
                }
                else {
                    switch (changeChoice(gen)) { //This works just like in the playfair attacks
                    case 1:
                        flipDiag(childBottomLeftKey);
                        break;
                    case 2:
                        flipHoriz(childBottomLeftKey);
                        break;
                    case 3:
                        flipDiag(childBottomLeftKey);
                        break;
                    case 4:
                        swapRows(childBottomLeftKey, &dist, &gen);
                        break;
                    case 5:
                        swapCols(childBottomLeftKey, &dist, &gen);
                        break;
                    default:
                        swapElems(childBottomLeftKey, &dist, &gen);
                        break;
                    }
                }

                //Get a decrypt and fitness
                childDecrypt = fourSquareDecrypt(cipher, childTopRightKey, childBottomLeftKey);
                childFitness = fitness::tetragramFitness(&childDecrypt);

                if (childFitness > bestFitness) {
                    wandering = false;

                    bestFitness = childFitness;
                    currentFitness = childFitness;

                    bestDecrypt = childDecrypt;
                    currentDecrypt = childDecrypt;

                    bestTopRightKey = childTopRightKey;
                    currentTopRightKey = childTopRightKey;
                    bestBottomLeftKey = childBottomLeftKey;
                    currentBottomLeftKey = childBottomLeftKey;

                    std::cout << bestFitness << " " << counter << std::endl;

                    counter = 0;
                    impatience = 0;
                }
                else if (childFitness == bestFitness) {
                    impatience = 0;
                    wandering = false;
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentTopRightKey = childTopRightKey;
                    currentBottomLeftKey = childBottomLeftKey;
                }
                else if (childFitness > currentFitness) {
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentTopRightKey = childTopRightKey;
                    currentBottomLeftKey = childBottomLeftKey;
                }
                else if (counter > 100 && childFitness > (bestFitness + (bestFitness / 6)) && changeChoice(gen) < 5) { //Randomly step back, with permissible range set to be 7/6 of the bestFitness and a 20% chance
                    currentTopRightKey = childTopRightKey;
                    currentBottomLeftKey = childBottomLeftKey;
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    wandering = true;
                }

                if (impatience > 2000) { //De-wander the key if it has wandered too far without improvement
                    currentTopRightKey = bestTopRightKey;
                    currentBottomLeftKey = bestBottomLeftKey;
                    impatience = 0;
                    wandering = false;
                }

                //Increment counter
                counter++;
                if (wandering) {
                    impatience++;
                }
                
                if (bestFitness > -12 && counter > 10000) { //Return and break if its clearly a good key
                    return { bestTopRightKey, bestBottomLeftKey };
                }
            }

            if (i != 19) { //Formatting code for iterations counter
                std::cout << std::endl;
            }
        }

        return { nullPolybius, nullPolybius }; //Return a null result
    }

    //CLI interface for the four-square hill-climbing attack
    int cliFourSquareHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

        std::tuple<polybius, polybius> result = fourSquareHillClimber(cipher);

        if (std::get<0>(result) != nullPolybius) { //Check its not a null result
            std::string decrypt = fourSquareDecrypt(cipher, std::get<0>(result), std::get<1>(result));
            if (cliInterface::offerDecryption(decrypt)) { //Can skip fitness here as filtering in hill-climber
                return 1; //Success
            }
        }

        return 0; //Failure
    }
}