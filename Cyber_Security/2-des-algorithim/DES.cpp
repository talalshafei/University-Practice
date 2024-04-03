#include "DES.h"

DES::DES(vector<Byte> key, vector<Byte> iv, vector<Byte> input)
	: key(key), iv(iv), input(input){

	// verify parity bits and show warning if any
	for (int i = 0; i < 8; i++) {
		// make sure each last bit is the correct parity bit
		Byte b = key[i];
		bool parityBit = b & 1;
		int count = 0; 
		for (int j = 1; j < 8; j++) 
			count += (b >> j) & 1;
		
		if ((count % 2) != parityBit)
			cout << "**Warning: Parity bit for the Byte "<<i+1<<" in the key is incorrect" << endl;
		
	}


	generateKeys();

	cout << "\n\n-------------------------------------------\n";
	cout<< "**** Starting DES Algorithm... ****"<< endl;

	cout << "Key\n"
		<< "	Hex: ";
	printBytesAsHex(key);
	cout << "	Binary: ";
	printBytesAsBits(key);
	cout << endl;

	cout << "IV\n"
		<< "	Hex: ";
	printBytesAsHex(iv);
	cout << "	Binary: ";
	printBytesAsBits(iv);
	cout << endl;

	cout << "Input\n"
		<< "	Text: ";
		printBytes(input);
	cout << "	Hex: ";
	printBytesAsHex(input);
	cout << endl;

	cout << "\n**** DES Output ****" << endl;


}

bool DES::getBit(const vector<Byte>& bytesVector, unsigned int pos) {

	// find which Byte the bit is in
	unsigned int bytePos = pos / 8;

	// find which bit in the Byte
	unsigned int bitPos = 7 - (pos % 8);

	// get the bit
	bool bit = (bytesVector[bytePos] >> bitPos) & 1;

	return bit;
}

void DES::setBit(vector<Byte>& bytesVector, unsigned int pos, bool value) {
	// find which Byte the bit is in
	unsigned int bytePos = pos / 8;

	// find which bit in the Byte
	unsigned int bitPos = 7 - (pos % 8);

	// set or clear the bit
	if(value)
		bytesVector[bytePos] |= 1 << bitPos;
	else
		bytesVector[bytePos] &= ~(1 << bitPos);

}

void DES::xorBytes(vector<Byte>& result, const vector<Byte>& a, const vector<Byte>& b) {
	int size = (a.size() < b.size()) ? a.size() : b.size();
	for (int i = 0; i < a.size(); i++) {
		result[i] = a[i] ^ b[i];
	}
}

void DES::applyPermutation(vector<Byte>& result, const vector<Byte>& source, const unsigned int table[], unsigned int size) {
	for (int i = 0; i < size; i++) {
		bool bit = getBit(source, table[i] - 1);
		setBit(result, i, bit);
	}
}

void DES::generateKeys() {

	vector<Byte> permutedKey(7);
	for (int i = 0; i < 56; i++) {
		bool bit = getBit(key, PC1[i] - 1);
		setBit(permutedKey, i, bit);
	}

	// unfortunately 28 bits are not divisible by 8 and we cant just make a vector of size 3.5
	// and since we are only shifting here we can use strings
	// also we can use bitset class for optimzation but I will do it with strings for now

	string C0, D0;
	for (int i = 0; i < 28; i++) {
		C0+= getBit(permutedKey, i) + '0';
		D0+= getBit(permutedKey, i + 28) + '0';
	}

	// apply left shifts
	vector<string> C(17), D(17), K();
	C[0] = C0;
	D[0] = D0;
	for (int i = 1; i <= 16; i++) {
		int n = shiftTable[i - 1]; // get number of shifts
		C[i] = C[i - 1].substr(n) + C[i - 1].substr(0, n);
		D[i] = D[i - 1].substr(n) + D[i - 1].substr(0, n);
	}

	// form the keys
	for (int i = 0; i < 16; i++) {
		string CD = C[i + 1] + D[i + 1];
		roundKeys[i] = vector<Byte>(6);
		for (int j = 0; j < 48; j++) {
			bool bit = CD[PC2[j] - 1] - '0';
			setBit(roundKeys[i], j, bit);
		}
	}

}

vector<Byte> DES::desAlgorithm(bool isEncryption) {

	vector<Byte> output;

	// loop over blocks of 64 bits
	int numBlocks = input.size() / 8;
	for (int i = 0; i < numBlocks; i++) {
		vector<Byte> block(input.begin() + i * 8, input.begin() + (i + 1) * 8);

		// first apply CBC mode
		if (isEncryption) {
			if (i == 0)
				xorBytes(block, block, iv);

			else {
				vector<Byte> cBlock = vector<Byte>(output.begin() + (i - 1) * 8, output.begin() + i * 8);
				xorBytes(block, block, cBlock);
			}
		}
		

		// apply initial permutation IP
		vector<Byte> ipBlock(8);
		applyPermutation(ipBlock, block, IP, 64);

		vector<Byte> L0(ipBlock.begin(), ipBlock.begin() + 4);
		vector<Byte> R0(ipBlock.begin() + 4, ipBlock.end());

		vector<vector<Byte>> L(17), R(17);
		L[0] = L0;
		R[0] = R0;

		// Apply f function in DES encryption
		// 16 rounds for each block
		for (int i = 1; i <= 16; i++) {
			vector<Byte> roundKey;
			if(isEncryption)
				roundKey = roundKeys[i - 1];
			else
				roundKey = roundKeys[16 - i];

			// use Ln = Rn-1 and Rn = Ln-1 xor f(Rn-1, Kn)
			L[i] = R[i - 1];

			// F function
			// expand Rn-1 to 48 bits
			vector<Byte> expandedR(6);
			applyPermutation(expandedR, R[i - 1], E, 48);

			// xor with round key
			vector<Byte> rXorK(6);
			xorBytes(rXorK, expandedR, roundKey);


			// reduces the 48 bit to 32 bit
			// by applying S-boxes
			vector<Byte> sBoxOutput(4, 0); // initialize the bits to 0s

			for (int j = 0; j < 8; j++) {
				// get the jth 6 bits of rXorK
				vector<bool> bits(6);
				for (int k = 0; k < 6; k++) {
					bits[k] = getBit(rXorK, (j * 6) + k);
				}

				// get the row and column
				// row first and last bits
				// column 4 middle bits
				int row = bits[0] * 2 + bits[5];
				int col = bits[1] * 8 + bits[2] * 4 + bits[3] * 2 + bits[4];

				// get the value from S-box
				int val = S[j][row][col];

				// set the value in sBoxOutput
				// if j even set the first 4 bits in the j/2 byte
				// if j is odd set the last 4 bits in the j/2 byte
				if (j & 1)
					sBoxOutput[j / 2] |= val;
				else
					sBoxOutput[j / 2] |= (val << 4);
			}

			vector<Byte> fOutput(4);
			applyPermutation(fOutput, sBoxOutput, P, 32);
			// End of F function

			// Rn = Ln-1 xor f(Rn-1, Kn)
			R[i] = vector<Byte>(4);
			xorBytes(R[i], L[i - 1], fOutput);

		}

		// R16 and L16 are cobined after as R.L
		vector<Byte> combined(8);
		for (int j = 0; j < 4; j++) {
			combined[j] = R[16][j];
			combined[j + 4] = L[16][j];
		}

		// apply final permutation IP^-1
		vector<Byte> finalBlock(8);
		applyPermutation(finalBlock, combined, IP_1, 64);

		// if decryption remove CBC mode
		if (!isEncryption) {
			// remove CBC mode
			if (i == 0) 
				xorBytes(finalBlock, finalBlock, iv);
			else {
				vector<Byte> pBlock = vector<Byte>(input.begin() + (i - 1) * 8, input.begin() + i * 8);
				xorBytes(finalBlock, finalBlock, pBlock);
				
			}
		}

		// append to output
		output.insert(output.end(), finalBlock.begin(), finalBlock.end());
	}

	return output;
}

vector<Byte> DES::encrypt() {
	vector<Byte> cipherText = desAlgorithm(true);

	cout << "Encrypted Cipher\n"
		<< "	Text: ";
	printBytes(cipherText);
	cout << "	Hex: ";
	printBytesAsHex(cipherText);
	cout << "\n-------------------------------------------\n";

	return cipherText;
}

vector<Byte> DES::decrypt() {
	vector<Byte> plainText = desAlgorithm(false);
	cout << "Decrypted Message\n"
		<< "	Text: ";
	printBytes(plainText);
	cout << "	Hex: ";
	printBytesAsHex(plainText);
	cout << "\n-------------------------------------------\n";

	return plainText;
}