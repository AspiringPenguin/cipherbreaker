#include "basics.h"
#include "fitness.h"
#include "strings.h"
#include "interface.h"
#include "transpositions.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

namespace transpositions{
	//Helper function to divide two integers, but always round up if they do not produce an integer result. e.g. 36,6 -> 6 but 37,6 -> 7
	int integerDivisionRoundUp(int a, int b) {
		float fRes = static_cast<float>(a) / b;
		#pragma warning(push)
		#pragma warning(disable:4244) //Intentional behaviour to narrow cast
		int res = fRes;
		#pragma warning(pop)
		if (std::fmod(fRes, 1) != 0) {
			res++;
		}
		return res;
	}

	//Generator for permutations of array 0->n inclusive, using heap's algorithm
	//This uses std::generator and coroutines so the permuatations can be generated on the fly
	// as opposed to generating them all and consuming large amounts of memory in a vactor,
	// which could potentially cause system crashes due to the huge contiguous memory allocations
	std::generator<std::vector<int>> heapsPerms(int n) { 
		auto A = std::vector<int>(); //The array being modified
		auto c = std::vector<int>(); //A collection of counters
		for (int i = 0; i < n; i++) {
			A.push_back(i);
			c.push_back(0);
		}
		int i = 0;
		int _;
		co_yield A; //Yield the unmodified array

		while (i < n) {
			if (c[i] < i) {
				if (i % 2 == 0) {
					_ = A[0];
					A[0] = A[i];
					A[i] = _;
				}
				else {
					_ = A[c[i]];
					A[c[i]] = A[i];
					A[i] = _;
				}
				co_yield A; //Yield the current value of A - it will be used by the code calling the generator, then this code will continue until another co_yield is reached
				c[i]++;
				i = 0;
			}
			else {
				c[i] = 0;
				i++;
			}
		}

		co_return; //Exit to show the generator is finished
	}

	//A helper function for hill-climbing attacks to 'roll' a key e.g. 0123 roll 2 -> 2301
	std::vector<int> rollKey(std::vector<int>& key, int roll) {
		auto res = std::vector<int>();
		int keyLen = key.size();
		for (int i = roll; i < (keyLen + roll); i++) {
			res.push_back(key[i % keyLen]);
		}
		return res;
	}

	//Like the above but for strings
	std::string rollString(std::string toRoll, int roll) {
		std::string res = "";
		int len = toRoll.length();
		for (int i = roll; i < (len + roll); i++) {
			res += toRoll[i % len];
		}
		return res;
	}

	//Permutation cipher decryption
	//This adds and then remove nulls by default as the columns don't have to be the same length
	//Duplicated in permutationBruteForce to reuse objects and reduce memory overhead
	std::string permutationDecrypt(std::string cipher, std::vector<int> key) { 
		auto cols = strings::getColumns(cipher, key.size());
		auto newCols = std::vector<std::string>();
		for (const auto& i : key) {
			newCols.push_back(cols[i]);
		}
		return strings::columnsToString(newCols);
	}

	//Brute force attack on the permutation cipher
	//No longer used as replaced by a hill-climbing attack
	std::string permutationBruteForce(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto newCols = std::vector<std::string>();
		std::vector<std::string> cols;
		std::string decrypt;

		for (int n = 2; n < 11; n++) { //For keysize 2->10 inclusive
			std::cout << n << std::endl;
			cols = strings::getColumns(cipher, n); //Duplicate of permuatation decrypt
			for (const auto& perm : heapsPerms(n)) { //For all permutations
				newCols.clear();
				for (const auto& i : perm) {
					newCols.push_back(cols[i]);
				}
				decrypt = strings::columnsToString(newCols); //End duplicate
				if (fitness::tetragramFitness(&decrypt) > -15) { //Check if the decrypt is English-like
					return decrypt;
				}
			}
		}
		return ""; //Return empty string for failure
	}

	//CLI interface code for the above
	//No longer used as replaced by a hill-climbing attack
	int cliPermutationBruteForce(std::string cipher) {
		auto decrypt = permutationBruteForce(cipher);
		if (decrypt != "") { //Check for a null result
			if (cliInterface::offerDecryption(decrypt)) {
				return 1; //Success
			}
		}
		return 0; //Failure
	}

	//Hill-climbing attack on the permutation cipher for a given key length.
	//This uses the "basic hill climber with step back and extra limits" setup
	std::vector<int> permutationSubHillClimber(std::string cipher, int keyLen) {
		//Generate bestKey - an identity permuatation which has no effect
		auto bestKey = std::vector<int>();
		for (int i = 0; i < keyLen; i++) {
			bestKey.push_back(i);
		}
		//Get bestFitness and bestDecrypt
		float bestFitness = fitness::tetragramFitness(&cipher);
		std::string bestDecrypt = cipher;

		//child items
		std::vector<int> childKey;
		float childFitness;
		std::string childDecrypt;

		//Loop control
		int counter = 0;
		int limit = 1000 * keyLen;
		int absoluteLimit = 100000 * keyLen;
		int total = 0; //This is used to give up on wrong key lengths

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> coin(0, 1); //Coin toss
		std::uniform_int_distribution<> keyChoice(0, keyLen - 1); //Picking elements
		std::uniform_int_distribution<> rollChoice(1, keyLen - 1); //Picking roll amount
		std::uniform_int_distribution<> die(1, 20); //20-sided die

		int a, b, _; //For copies

		//counter < limit is normal, the next term is to quickly exit attacks that aren't going anywhere as total iterations exceeds the limit without a good fitness
		//final term is to exit after a longer time if total becomes too large because similar ciphers can cause this attack to get stuck oscillating between a few keys for a certain keylength
		while (counter < limit && (total < limit || bestFitness > -40) && total < absoluteLimit) { 
			childKey = bestKey; //Copy to childKey
			if (coin(gen) == 0) { //Flip a coin, heads - swap two elements
				a = keyChoice(gen);
				b = keyChoice(gen);
				while (a == b) {
					b = keyChoice(gen);
				}
				_ = childKey[a];
				childKey[a] = childKey[b];
				childKey[b] = _;
			}
			else { //tails - roll the key a random amount
				childKey = rollKey(childKey, rollChoice(gen));
			}

			//Decrypt and get fitness
			childDecrypt = permutationDecrypt(cipher, childKey);
			childFitness = fitness::tetragramFitness(&childDecrypt);

			if (counter > 100 && childFitness > bestFitness || (childFitness > (bestFitness - 2) && die(gen) == 1)) {
				counter = 0;
				bestKey = childKey;
				bestFitness = childFitness;
				bestDecrypt = childDecrypt;
			}

			//Increment loop variables
			counter++;
			total++;
		}

		return bestKey;
	}

	//Brute force wrapper for permutation hill-climber that tries every key length
	std::vector<int> permutationHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		std::string decrypt;
		for (int i = 2; i < 21; i++) { //Key length 2->20 inclusive
			std::cout << i << std::endl;
			auto res = permutationSubHillClimber(cipher, i); //Run the hill-climber
			decrypt = permutationDecrypt(cipher, res);
			if (fitness::tetragramFitness(&decrypt) > -15) {
				return res;
			}
		}
		return {}; //Return an empty vector as a null result
	}

	//CLI interface for the above attack
	int cliPermutationHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto res = permutationHillClimber(cipher);
		if (res.size() != 0) { //Ignore null results with size = 0
			auto decrypt = permutationDecrypt(cipher, res);
			if (cliInterface::offerDecryption(decrypt)) {
				return 1; //Success
			}
		}
		return 0; //Failure
	}

	//Columnar transposition decryption routine.
	std::string columnarDecrypt(std::string cipher, std::vector<int> key) {
		//Get keyLen and number of columns that are shorter
		int keyLen = key.size();
		int cipherLen = cipher.length();
		int numGaps = keyLen - (cipherLen % keyLen);
		if (numGaps == keyLen) {
			numGaps = 0;
		}

		//Get column lengths
		int columnLen = integerDivisionRoundUp(cipherLen, keyLen);

		//Find blocks which need extra spaces
		std::vector<int> needsGaps = std::vector<int>();
		for (int i = 0; i < numGaps; i++) {
			needsGaps.push_back(key[keyLen - i - 1]);
		}

		//Sort by position in cipher so spaces can be added easily
		std::sort(needsGaps.begin(), needsGaps.end());

		/*for (auto c : needsGaps) {
			std::cout << c << std::endl;
		}*/

		//Add the missing spaces to get correct columns
		for (int i = 0; i < numGaps; i++) {
			cipher = cipher.substr(0, columnLen * (needsGaps[i] + 1) - 1) + " " + cipher.substr(columnLen * (needsGaps[i] + 1) - 1);
		}

		/*std::cout << cipher << std::endl;*/

		std::vector<std::string> blocks = strings::getBlocks(cipher, columnLen); //This gets the columns, as the text is read off column by column when encrypted
		auto newBlocks = std::vector<std::string>();
		for (const int& index : key) { //Reorder the columns based on the key
			newBlocks.push_back(blocks[index]);
		}

		//Join up the columns into a plaintext
		std::string plain = "";
		for (int x = 0; x < columnLen; x++) {
			for (int y = 0; y < keyLen; y++) {
				plain += newBlocks[y][x];
			}
		}

		return basics::removeSpaces(plain); //Remove the extra spaces and return
	}

	//Brute force attack on the permutation cipher
	//No longer used as replaced by a hill-climbing attack
	std::string columnarBruteForce(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto newCols = std::vector<std::string>();
		std::vector<std::string> cols;
		std::string decrypt;

		for (int n = 2; n < 11; n++) { //For keylen 2->10 inclusive
			std::cout << n << std::endl;
			for (const auto& perm : heapsPerms(n)) { //Try every key
				decrypt = columnarDecrypt(cipher, perm);
				if (fitness::tetragramFitness(&decrypt) > -15) {
					return decrypt;
				}
			}
		}
		return ""; //Return a null result
	}

	//CLI interface for the above
	//No longer used as replaced by a hill-climbing attack
	int cliColumnarBruteForce(std::string cipher) {
		auto decrypt = columnarBruteForce(cipher); //Run the attack
		if (decrypt != "") { //Avoid null results
			if (cliInterface::offerDecryption(decrypt)) {
				return 1; //Success
			}
		}
		return 0; //Failure
	}

	//Hill-climbing attack on the columnar transposition cipher for a given key length.
	//This uses the "basic hill climber with step back and extra limits" setup
	std::vector<int> columnarSubHillClimber(std::string cipher, int keyLen) {
		//Generate bestKey - just 0->(keyLen-1) inclusive
		auto bestKey = std::vector<int>();
		for (int i = 0; i < keyLen; i++) {
			bestKey.push_back(i);
		}
		//Get bestfitness and decrypt
		std::string bestDecrypt = columnarDecrypt(cipher, bestKey);
		float bestFitness = fitness::tetragramFitness(&bestDecrypt);

		//Child items
		std::vector<int> childKey;
		float childFitness;
		std::string childDecrypt;

		//Loop control
		int counter = 0;
		int limit = 1000 * keyLen;
		int absoluteLimit = 100000 * keyLen;
		int total = 0; //This is used to give up on wrong key lengths

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> coin(0, 1); //Coin toss
		std::uniform_int_distribution<> keyChoice(0, keyLen - 1); //Picking elements
		std::uniform_int_distribution<> rollChoice(1, keyLen - 1); //Picking roll amount
		std::uniform_int_distribution<> die(1, 20); //20-sided die

		int a, b, _; //For copies

		while (counter < limit && (total < limit || bestFitness > -40) && total < absoluteLimit) {
			childKey = bestKey; //Copy to childKey
			if (coin(gen) == 0) { //Flip a coin, heads - swap two elements
				a = keyChoice(gen);
				b = keyChoice(gen);
				while (a == b) {
					b = keyChoice(gen);
				}
				_ = childKey[a];
				childKey[a] = childKey[b];
				childKey[b] = _;
			}
			else { //tails - roll the key a random amount
				childKey = rollKey(childKey, rollChoice(gen));
			}

			//Decrypt and get fitness
			childDecrypt = columnarDecrypt(cipher, childKey);
			childFitness = fitness::tetragramFitness(&childDecrypt);

			if (counter > 100 && childFitness > bestFitness || (childFitness > (bestFitness - 2) && die(gen) == 1)) {
				counter = 0;
				bestKey = childKey;
				bestFitness = childFitness;
				bestDecrypt = childDecrypt;
			}

			//Increment loop variables
			counter++;
			total++;
		}

		return bestKey;
	}

	//Brute force wrapper for columnar hill-climber that tries every key length
	std::vector<int> columnarHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		std::string decrypt;
		for (int i = 2; i < 21; i++) { //Key length 2->20 inclusive
			std::cout << i << std::endl;
			auto res = columnarSubHillClimber(cipher, i); //Run the hill-climber
			decrypt = columnarDecrypt(cipher, res);
			if (fitness::tetragramFitness(&decrypt) > -15) {
				return res;
			}
		}
		return {}; //Return an empty vector as a null result
	}

	//CLI interface for the above attack
	int cliColumnarHillClimber(std::string cipher) {
		cipher = basics::formatString(cipher);
		auto res = columnarHillClimber(cipher);
		if (res.size() != 0) { //Ignore null results with size = 0
			auto decrypt = columnarDecrypt(cipher, res);
			if (cliInterface::offerDecryption(decrypt)) {
				return 1; //Success
			}
		}
		return 0; //Failure
	}

	//Decryption routine for the twistedScytale cipher
	std::string twistedScytaleDecrypt(std::string cipher, int width, int twist) {
		//Get keyLen and number of columns that are shorter
		int cipherLen = cipher.length();
		int numGaps = width - (cipherLen % width);
		if (numGaps == width) {
			numGaps = 0;
		}

		//Get column lengths
		int columnLen = integerDivisionRoundUp(cipherLen, width);

		//Find columns which need extra spaces
		std::vector<int> needsGaps = std::vector<int>();
		for (int i = (width - numGaps); i < width; i++) {
			needsGaps.push_back(i);
		}

		//Add spaces to orig text
		for (int i = 0; i < numGaps; i++) {
			cipher = cipher.substr(0, columnLen * (needsGaps[i] + 1) - 1) + " " + cipher.substr(columnLen * (needsGaps[i] + 1) - 1);
		}

		//Get rows out
		std::vector<std::string> columns = strings::getBlocks(cipher, columnLen);
		//Generate vector of empty strings
		auto rows = std::vector<std::string>();
		for (int i = 0; i < columnLen; i++) {
			rows.push_back("");
		}
		//Fill out the rows
		for (int y = 0; y < columnLen; y++) {
			for (int x = 0; x < width; x++) {
				rows[y] += columns[x][y];
			}
		}

		//Detwist the rows
		auto rolledRows = std::vector<std::string>();
		int roll;
		std::string rolled;
		for (int y = 0; y < columnLen; y++) {
			roll = ((twist * y) % width) * -1;
			while (roll < 0) {
				roll += width;
			}
			rolled = rollString(rows[y], roll);
			rolledRows.push_back(rolled);
		}

		//Join rows together
		std::string plain = "";
		for (std::string row : rolledRows) {
			plain += row;
		}

		return basics::removeSpaces(plain); //Remove added spaces
	}

	//Brute force attack on the twisted scytale cipher
	std::string twistedScytaleBruteForce(std::string cipher) {
		int n = cipher.length();

		std::string decrypt;

		for (int width = 1; width < n; width++) { //For all widths
			for (int twist = 1; twist < width; twist++) { //For all twists
				decrypt = twistedScytaleDecrypt(cipher, width, twist); //Decrypt
				if (fitness::tetragramFitness(&decrypt) > -15) {
					return decrypt;
				}
			}
		}

		return ""; //Return a null result
	}

	int cliTwistedScytaleBruteForce(std::string cipher) {
		cipher = basics::formatString(cipher);

		auto res = twistedScytaleBruteForce(cipher);

		if (res != "") { //Ignore a null result
			if (cliInterface::offerDecryption(res)) {
				return 1; //Success
			}
		}
		
		return 0; //Failure
	}
}
