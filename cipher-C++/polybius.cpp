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

    //Flips a polybius along the y=x diagonal (given top-left (0, 0) bottom-right (4, 4))
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
    //This uses the "recursive improving best-current-child key" setup
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

        //To control goto statement
        bool improved;

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

            improved = false; //Key hasn't been recursively improved

            playfairEvaluate: //Label for goto. I know using goto is generally bad but it is strictly controlled here and it seemed the best way to reevaluate improved keys
            if (childFitness > bestFitness) {
                if (!improved) {
                    childKey = playfairBacktracking(cipher, childKey, false); //Recursively improve the key by only taking forward steps
                    //Don't need to set improved here as no goto is used

                    //Redecrypt
                    childDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, childKey));
                    childFitness = fitness::tetragramFitness(&childDecrypt);
                }

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

            else if (childFitness >= currentFitness) {
                if (childFitness == bestFitness) {
                    impatience = 0;
                    wandering = false;
                }
                if (!improved) { //If the key hasn't been improved
                    //Improve it
                    improved = true; //To prevent loop 

                    childKey = playfairBacktracking(cipher, childKey, false);

                    childDecrypt = processPlayfairDecrypt(playfairDecrypt(cipher, childKey));
                    childFitness = fitness::tetragramFitness(&childDecrypt);

                    goto playfairEvaluate; //Re-evaluate
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

    //A copy of the above but using 2025Variation decryption and recursive improver
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

        //To control goto statement
        bool improved;

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

            //To control goto statement
            improved = false;

        playfair2025VariationEvaluate: //Label for goto
            if (childFitness > bestFitness) {
                if (!improved) {
                    childKey = playfairBacktracking(cipher, childKey, false); //Recursively improve the key by only taking forward steps
                    //Don't need to set improved here as no goto is used

                    //Redecrypt
                    childDecrypt = processPlayfairDecrypt(playfair2025VariationDecrypt(cipher, childKey));
                    childFitness = fitness::tetragramFitness(&childDecrypt);
                    std::cout << childFitness << std::endl;
                }

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
                if (!improved) { //If the key hasn't been improved
                    //Improve it
                    improved = true; //To prevent loop 

                    childKey = playfair2025VariationBacktracking(cipher, childKey, false);

                    childDecrypt = processPlayfairDecrypt(playfair2025VariationDecrypt(cipher, childKey));
                    childFitness = fitness::tetragramFitness(&childDecrypt);

                    goto playfair2025VariationEvaluate; //Re-evaluate
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
    polybius playfairBacktracking(std::string cipher, polybius startKey, bool ignoreBad) {
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
            for (const polybius& child : children) { //Check if any of the children are better than the others in this depth
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
    polybius playfair2025VariationBacktracking(std::string cipher, polybius startKey, bool ignoreBad) {
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

    int cliPlayfairHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

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
                return 1; //Success
            }
        }
        return 0; //Failure
    }

    int cliPlayfair2025VariationHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

        polybius bestKey = playfair2025VariationHillClimber(cipher);
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
                std::cout << rawDecrypt << std::endl;
                return 1; //Success
            }
        }
        return 0; //Failure
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

    std::vector<std::tuple<polybius, polybius>> getAllChildKeysTwoSquare(polybius key1, polybius key2) {
        auto keys = std::vector<std::tuple<polybius, polybius>>();
        polybius childKey;

        //Swap two places in first square
        for (int y1 = 0; y1 < 5; y1++) {
            for (int y2 = 0; y2 < 5; y2++) {
                for (int x1 = 0; x1 < 5; x1++) {
                    for (int x2 = 0; x2 < 5; x2++) {
                        childKey = key1;
                        char _ = childKey[y1][x1];
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
                        childKey = key2;
                        char _ = childKey[y1][x1];
                        childKey[y1][x1] = childKey[y2][x2];
                        childKey[y2][x2] = _;
                        keys.push_back({ key1, childKey });
                    }
                }
            }
        }

        return keys;
    }
    
    std::tuple<polybius, polybius> vertTwoSquareHillClimber(std::string cipher) { //Not the best, but it works for fair-sized texts
        cipher = basics::formatString(cipher);

        auto alphabetCopy = basics::alphabet;

        polybius bestTopKey;
        polybius bestRightKey;

        std::string bestDecrypt;

        float bestFitness;
        
        polybius currentTopKey;
        polybius currentRightKey;
        std::string currentDecrypt;
        float currentFitness;

        polybius childTopKey;
        polybius childRightKey;
        std::string childDecrypt;
        float childFitness;

        std::tuple<polybius, polybius> improvedKey;
        bool improved;

        //Random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> squareChoice(0, 1);
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 5);

        int counter = 0;
        int impatience = 0;

        bool wandering;
        
        int N = 0;
        while (N < 1000) {
            std::cout << std::endl << "Restart " << N << std::endl;

            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestTopKey = makePolybius(alphabetCopy);

            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestRightKey = makePolybius(alphabetCopy);

            bestDecrypt = vertTwoSquareDecrypt(cipher, bestTopKey, bestRightKey);

            bestFitness = fitness::tetragramFitness(&bestDecrypt);

            currentTopKey = bestTopKey;
            currentRightKey = bestRightKey;
            currentDecrypt = bestDecrypt;
            currentFitness = bestFitness;

            counter = 0;
            impatience = 0;

            wandering = false;

            while (counter < 20000 || (bestFitness > -37 && counter < (20000 + ((37 + bestFitness) * 10000)))) {
                childTopKey = currentTopKey;
                childRightKey = currentRightKey;

                if (squareChoice(gen) == 0) {
                    swapElems(childTopKey, &dist, &gen);
                }
                else {
                    swapElems(childRightKey, &dist, &gen);
                }

                childDecrypt = vertTwoSquareDecrypt(cipher, childTopKey, childRightKey);
                childFitness = fitness::tetragramFitness(&childDecrypt);

                improved = false;

            vertTwoSquareEvaluate:
                if (childFitness > bestFitness) {
                    //Improve if not already improved, don't need to reevaluate as already the best key seen
                    if (!improved){
                        improvedKey = vertTwoSquareBacktracking(cipher, childTopKey, childRightKey, false);
                        childTopKey = std::get<0>(improvedKey);
                        childRightKey = std::get<1>(improvedKey);

                        childDecrypt = vertTwoSquareDecrypt(cipher, childTopKey, childRightKey);
                        childFitness = fitness::tetragramFitness(&childDecrypt);
                    }

                    wandering = false;

                    bestFitness = childFitness;
                    currentFitness = childFitness;

                    bestDecrypt = childDecrypt;
                    currentDecrypt = childDecrypt;

                    bestTopKey = childTopKey;
                    currentTopKey = childTopKey;
                    bestRightKey = childRightKey;
                    currentRightKey = childRightKey;

                    std::cout << bestFitness << " " << counter << std::endl;

                    counter = 0;
                    impatience = 0;
                }
                else if (childFitness == bestFitness) {
                    if (!improved) {
                        improvedKey = vertTwoSquareBacktracking(cipher, childTopKey, childRightKey, false);
                        childTopKey = std::get<0>(improvedKey);
                        childRightKey = std::get<1>(improvedKey);

                        childDecrypt = vertTwoSquareDecrypt(cipher, childTopKey, childRightKey);
                        childFitness = fitness::tetragramFitness(&childDecrypt);

                        improved = true; //Don't loop infinitely
                        goto vertTwoSquareEvaluate; //Re-evaluate now its improved
                    }

                    impatience = 0;
                    wandering = false;
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentTopKey = childTopKey;
                    currentRightKey = childRightKey;
                }
                else if (childFitness > currentFitness) {
                    if (!improved) {
                        improvedKey = vertTwoSquareBacktracking(cipher, childTopKey, childRightKey, false);
                        childTopKey = std::get<0>(improvedKey);
                        childRightKey = std::get<1>(improvedKey);

                        childDecrypt = vertTwoSquareDecrypt(cipher, childTopKey, childRightKey);
                        childFitness = fitness::tetragramFitness(&childDecrypt);

                        improved = true; //Don't loop infinitely
                        goto vertTwoSquareEvaluate; //Re-evaluate now its improved
                    }

                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentTopKey = childTopKey;
                    currentRightKey = childRightKey;
                }
                else if (counter > 100 && childFitness > (bestFitness + (bestFitness/6)) && changeChoice(gen) == 1) {
                    currentTopKey = childTopKey;
                    currentRightKey = childRightKey;
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    wandering = true;
                }

                if (impatience > 2000) {
                    currentTopKey = bestTopKey;
                    currentRightKey = bestRightKey;
                    impatience = 0;
                    wandering = false;
                }

                counter++;
                if (wandering) {
                    impatience++;
                }

                if (bestFitness > -12 && counter > 10000) {
                    break;
                }
            }

            if (fitness::tetragramFitness(&bestDecrypt) > -15) {
                return { bestTopKey, bestRightKey };
            }

            N++;
        }

        return { nullPolybius, nullPolybius };
    }

    std::tuple<polybius, polybius> vertTwoSquareBacktracking(std::string cipher, polybius startTop, polybius startRight, bool ignoreBad) {
        //Get a starting point
        auto decrypt = vertTwoSquareDecrypt(cipher, startTop, startRight);
        float fitness = fitness::tetragramFitness(&decrypt);
        std::tuple<polybius, polybius> bestKey = { startTop, startRight };
        float bestFitness = fitness;
        float childFitness;

        std::vector<std::tuple<polybius, polybius>> children;
        std::tuple<polybius, polybius> bestChild;

        while (true) {
            children = getAllChildKeysTwoSquare(startTop, startRight);
            fitness = -100;
            bestChild = { nullPolybius, nullPolybius };
            for (const std::tuple<polybius, polybius>& child : children) {
                decrypt = vertTwoSquareDecrypt(cipher, std::get<0>(child), std::get<1>(child));
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
                return { nullPolybius, nullPolybius };
            }
        }
        return bestKey;
    }

    int cliVertTwoSquareHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

        auto bestKeys = vertTwoSquareHillClimber(cipher);
        std::string decrypt = vertTwoSquareDecrypt(cipher, std::get<0>(bestKeys), std::get<1>(bestKeys));
        if (fitness::tetragramFitness(&decrypt) > -15) {
            if (cliInterface::offerDecryption(decrypt)) {
                return 1; //Success
            }
        }
        return 0; //Failure
    }

    std::tuple<polybius, polybius> horizTwoSquareHillClimber(std::string cipher) { //Not the best, but it works for fair-sized texts
        cipher = basics::formatString(cipher);

        auto alphabetCopy = basics::alphabet;

        polybius bestLeftKey;
        polybius bestRightKey;

        std::string bestDecrypt;

        float bestFitness;

        polybius currentLeftKey;
        polybius currentRightKey;
        std::string currentDecrypt;
        float currentFitness;

        polybius childLeftKey;
        polybius childRightKey;
        std::string childDecrypt;
        float childFitness;

        std::tuple<polybius, polybius> improvedKey;
        bool improved;

        //Random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> squareChoice(0, 1);
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 5);

        int counter = 0;
        int impatience = 0;

        bool wandering;

        int N = 0;
        while (N < 1000) {
            std::cout << std::endl << "Restart " << N << std::endl;

            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestLeftKey = makePolybius(alphabetCopy);

            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestRightKey = makePolybius(alphabetCopy);

            bestDecrypt = horizTwoSquareDecrypt(cipher, bestLeftKey, bestRightKey);

            bestFitness = fitness::tetragramFitness(&bestDecrypt);

            currentLeftKey = bestLeftKey;
            currentRightKey = bestRightKey;
            currentDecrypt = bestDecrypt;
            currentFitness = bestFitness;

            counter = 0;
            impatience = 0;

            wandering = false;

            while (counter < 20000 || (bestFitness > -37 && counter < (20000 + ((37 + bestFitness) * 10000)))) {
                childLeftKey = currentLeftKey;
                childRightKey = currentRightKey;

                if (squareChoice(gen) == 0) {
                    swapElems(childLeftKey, &dist, &gen);
                }
                else {
                    swapElems(childRightKey, &dist, &gen);
                }

                childDecrypt = horizTwoSquareDecrypt(cipher, childLeftKey, childRightKey);
                childFitness = fitness::tetragramFitness(&childDecrypt);

                improved = false;

            horizTwoSquareEvaluate:
                if (childFitness > bestFitness) {
                    //Improve if not already improved, don't need to reevaluate as already the best key seen
                    if (!improved) {
                        improvedKey = horizTwoSquareBacktracking(cipher, childLeftKey, childRightKey, false);
                        childLeftKey = std::get<0>(improvedKey);
                        childRightKey = std::get<1>(improvedKey);

                        childDecrypt = horizTwoSquareDecrypt(cipher, childLeftKey, childRightKey);
                        childFitness = fitness::tetragramFitness(&childDecrypt);
                    }

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
                else if (childFitness == bestFitness) {
                    if (!improved) {
                        improvedKey = horizTwoSquareBacktracking(cipher, childLeftKey, childRightKey, false);
                        childLeftKey = std::get<0>(improvedKey);
                        childRightKey = std::get<1>(improvedKey);

                        childDecrypt = horizTwoSquareDecrypt(cipher, childLeftKey, childRightKey);
                        childFitness = fitness::tetragramFitness(&childDecrypt);

                        improved = true; //Don't loop infinitely
                        goto horizTwoSquareEvaluate; //Re-evaluate now its improved
                    }

                    impatience = 0;
                    wandering = false;
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentLeftKey = childLeftKey;
                    currentRightKey = childRightKey;
                }
                else if (childFitness > currentFitness) {
                    if (!improved) {
                        improvedKey = horizTwoSquareBacktracking(cipher, childLeftKey, childRightKey, false);
                        childLeftKey = std::get<0>(improvedKey);
                        childRightKey = std::get<1>(improvedKey);

                        childDecrypt = horizTwoSquareDecrypt(cipher, childLeftKey, childRightKey);
                        childFitness = fitness::tetragramFitness(&childDecrypt);

                        improved = true; //Don't loop infinitely
                        goto horizTwoSquareEvaluate; //Re-evaluate now its improved
                    }

                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentLeftKey = childLeftKey;
                    currentRightKey = childRightKey;
                }
                else if (counter > 100 && childFitness > (bestFitness + (bestFitness / 6)) && changeChoice(gen) == 1) {
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
                if (wandering) {
                    impatience++;
                }

                if (bestFitness > -12 && counter > 10000) {
                    break;
                }
            }

            if (fitness::tetragramFitness(&bestDecrypt) > -15) {
                return { bestLeftKey, bestRightKey };
            }

            N++;
        }

        return { nullPolybius, nullPolybius };
    }

    std::tuple<polybius, polybius> horizTwoSquareBacktracking(std::string cipher, polybius startLeft, polybius startRight, bool ignoreBad) {
        //Get a starting point
        auto decrypt = horizTwoSquareDecrypt(cipher, startLeft, startRight);
        float fitness = fitness::tetragramFitness(&decrypt);
        std::tuple<polybius, polybius> bestKey = { startLeft, startRight };
        float bestFitness = fitness;
        float childFitness;

        std::vector<std::tuple<polybius, polybius>> children;
        std::tuple<polybius, polybius> bestChild;

        while (true) {
            children = getAllChildKeysTwoSquare(startLeft, startRight);
            fitness = -100;
            bestChild = { nullPolybius, nullPolybius };
            for (const std::tuple<polybius, polybius>& child : children) {
                decrypt = horizTwoSquareDecrypt(cipher, std::get<0>(child), std::get<1>(child));
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
                return { nullPolybius, nullPolybius };
            }
        }
        return bestKey;
    }

    int cliHorizTwoSquareHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

        auto bestKeys = horizTwoSquareHillClimber(cipher);
        std::string decrypt = horizTwoSquareDecrypt(cipher, std::get<0>(bestKeys), std::get<1>(bestKeys));
        if (fitness::tetragramFitness(&decrypt) > -15) {
            if (cliInterface::offerDecryption(decrypt)) {
                return 1; //Success
            }
        }
        return 0; //Failure
    }

    std::string fourSquareDecrypt(std::string text, polybius topRight, polybius bottomLeft) {
        auto blocks = strings::getBlocks(text, 2);

        std::string newText = "";
        newText.reserve(text.size());

        //Lookup table for char position
        std::array<std::tuple<int, int>, 26> topRightLookup;
        for (char c = 97; c < 123; c++) {
            topRightLookup[c - 97] = findInPolybius(c, topRight);
        }
        std::array<std::tuple<int, int>, 26> bottomLeftLookup;
        for (char c = 97; c < 123; c++) {
            bottomLeftLookup[c - 97] = findInPolybius(c, bottomLeft);
        }

        std::tuple<int, int> pos0;
        std::tuple<int, int> pos1;

        for (const auto& block : blocks) {
            pos0 = topRightLookup[block[0]-97];
            pos1 = bottomLeftLookup[block[1]-97];

            newText += alphabetPolybius[std::get<0>(pos0)][std::get<1>(pos1)];
            newText += alphabetPolybius[std::get<0>(pos1)][std::get<1>(pos0)];
        }

        return newText;
    }

    std::tuple<polybius, polybius> fourSquareHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        //Random numbers
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> squareChoice(0, 1);
        std::uniform_int_distribution<> dist(0, 4);
        std::uniform_int_distribution<> changeChoice(1, 50);

        auto alphabetCopy = basics::alphabet;

        polybius bestTopKey, bestRightKey, currentTopKey, currentRightKey;

        std::string bestDecrypt;
        std::string currentDecrypt;
        float bestFitness;
        float currentFitness;

        polybius childTopKey, childRightKey;
        std::string childDecrypt;
        float childFitness;

        int counter;
        int impatience;

        bool wandering;

        for (int i = 0; i < 20; i++) { //It can take a few attempts
            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestTopKey = makePolybius(alphabetCopy);
            std::shuffle(alphabetCopy.begin(), alphabetCopy.end(), gen);
            bestRightKey = makePolybius(alphabetCopy);

            bestDecrypt = fourSquareDecrypt(cipher, bestTopKey, bestRightKey);

            bestFitness = fitness::tetragramFitness(&bestDecrypt);

            currentTopKey = bestTopKey;
            currentRightKey = bestRightKey;
            currentDecrypt = bestDecrypt;
            currentFitness = bestFitness;

            counter = 0;
            impatience = 0;
            wandering = false;

            std::cout << "Iteration " << i << std::endl;

            while (counter < 200000) {
                childTopKey = currentTopKey;
                childRightKey = currentRightKey;

                if (squareChoice(gen) == 0) {
                    switch (changeChoice(gen)) {
                    case 1:
                        flipDiag(childTopKey);
                        break;
                    case 2:
                        flipHoriz(childTopKey);
                        break;
                    case 3:
                        flipDiag(childTopKey);
                        break;
                    case 4:
                        swapRows(childTopKey, &dist, &gen);
                        break;
                    case 5:
                        swapCols(childTopKey, &dist, &gen);
                        break;
                    default:
                        swapElems(childTopKey, &dist, &gen);
                        break;
                    }
                }
                else {
                    switch (changeChoice(gen)) {
                    case 1:
                        flipDiag(childRightKey);
                        break;
                    case 2:
                        flipHoriz(childRightKey);
                        break;
                    case 3:
                        flipDiag(childRightKey);
                        break;
                    case 4:
                        swapRows(childRightKey, &dist, &gen);
                        break;
                    case 5:
                        swapCols(childRightKey, &dist, &gen);
                        break;
                    default:
                        swapElems(childRightKey, &dist, &gen);
                        break;
                    }
                }

                childDecrypt = fourSquareDecrypt(cipher, childTopKey, childRightKey);
                childFitness = fitness::tetragramFitness(&childDecrypt);

                if (childFitness > bestFitness) {
                    wandering = false;

                    bestFitness = childFitness;
                    currentFitness = childFitness;

                    bestDecrypt = childDecrypt;
                    currentDecrypt = childDecrypt;

                    bestTopKey = childTopKey;
                    currentTopKey = childTopKey;
                    bestRightKey = childRightKey;
                    currentRightKey = childRightKey;

                    std::cout << bestFitness << " " << counter << std::endl;

                    counter = 0;
                    impatience = 0;
                }
                else if (childFitness == bestFitness) {
                    impatience = 0;
                    wandering = false;
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentTopKey = childTopKey;
                    currentRightKey = childRightKey;
                }
                else if (childFitness > currentFitness) {
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    currentTopKey = childTopKey;
                    currentRightKey = childRightKey;
                }
                else if (counter > 100 && childFitness > (bestFitness + (bestFitness / 6)) && changeChoice(gen) < 5) {
                    currentTopKey = childTopKey;
                    currentRightKey = childRightKey;
                    currentFitness = childFitness;
                    currentDecrypt = childDecrypt;
                    wandering = true;
                }

                if (impatience > 2000) {
                    currentTopKey = bestTopKey;
                    currentRightKey = bestRightKey;
                    impatience = 0;
                    wandering = false;
                }

                counter++;
                if (wandering) {
                    impatience++;
                }

                if (bestFitness > -12 && counter > 10000) {
                    return { bestTopKey, bestRightKey };
                }
            }

            if (i != 19) {
                std::cout << std::endl;
            }
        }

        return { nullPolybius, nullPolybius };
    }

    int cliFourSquareHillClimber(std::string cipher) {
        cipher = basics::formatString(cipher);

        if (cipher.length() % 2 == 1) { //Can't be bigram substitution as it has an odd number of letters
            return 0; //Failure
        }

        if (cipher.find('j') != std::string::npos) { //not possible as 'j' is disallowed
            return 0; //Failure
        }

        std::tuple<polybius, polybius> result = fourSquareHillClimber(cipher);

        if (std::get<0>(result) != nullPolybius) {
            std::string decrypt = fourSquareDecrypt(cipher, std::get<0>(result), std::get<1>(result));
            if (cliInterface::offerDecryption(decrypt)) { //Can skip fitness here as filtering in hill-climber
                return 1; //Success
            }
        }

        return 0; //Failure
    }
}