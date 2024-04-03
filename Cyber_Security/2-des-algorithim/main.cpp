// Name: Talal Shafei
// ID: 2542371


#include "DES.h"

// core functions
void menu();
void initialize(string&,string&,string&);
string encryptStr(string&, string&, string&);
string decryptStr(string&, string&, string&);

// helper functions
string enterKey();
string enterIv();
string enterInput();
bool checkValidHexString(const string&);
int hexCharToInt(char);
vector<Byte> hexToBytes(const string&);
vector<Byte> stringToBytes(const string&);
string bytesToString(const vector<Byte>&);

int main() {

	cout << "Notes: \n"
		<< "1. Key must be 56 bits so only 14 hex chars! (cause each hex is 4 bits)\n"
		<< "	another 8 bits will be added as parity check to make it 64 bits by the system that sends the data\n"
		<< "	so in this example, you can either enter 14 hex chars for the key and the main function will add the parity bits\n"
		<< "	or you can enter 16 hex chars and DES will show only a warning if the entered parity bits are incorrect\n"
		<< "	then it will ignore them and proceed with the algorithim\n"
		<< "2. IV size must be equal to the block size so in DES it must be 64 bits so enter 16 hex chars!\n" 
		<< "3. Input (plain or cipher) format is text, contrary to hex each char here is considered 8 bits because of ASCII format\n"
		<< "	so input can be any size but if not a multiple of 64 bits (8 chars) it will be padded with 0 bytes (ASCII null)\n"
		<< "4. Since this DES implementation uses bytes and manipulates the real bits in the memory directly it can\n"
		<< "	be used to encrypt any type of data (such as files, packets, etc)\n"
		<< "	all it needs is an interface to convert the data to a vector of bytes\n"
		<< "	and here the main function will be the interface that will translate the user's\n"
		<< "	input text to a vector of bytes before passing it to DES"
		<< endl;

	string option="";

	string key = "";
	string iv = "";
	string plainText = "";
	string cipherText = "";


	while (option != "3") {
		menu();
		getline(cin, option); // choose an option

		try {

			if (option == "1") {
				initialize(key, iv, plainText);
				cipherText = encryptStr(key, iv, plainText);
			}
			else if (option == "2") {
				initialize(key, iv, cipherText);
				plainText = decryptStr(key, iv, cipherText);
			}
			else if (option == "3")
				cout << "\n\nGoodbye!" << endl;
			else
				cout << "\n*** Invalid option **\n" << endl;

		}
		catch (const string& msg) {
			cout << "\n\n*** " << msg << endl << "Please try again\n" << endl;
		}

	}


	return 0;

}

void menu() {
	cout<< "\n%% The DES algorithm with CBC mode %%\n"
		<< "[1] Encrypt\n"
		<< "[2] Decrypt\n"
		<< "[3] Exit\n"
		<< "Selection: ";
}

void initialize(string& key, string& iv, string& input) {
	if (key != "") {
		cout << "Do you want to use the previous key, IV, and output?"
			<< " (y/n [default no]): ";
		string choice = "";
		getline(cin, choice);
		if (choice == "y" || choice == "Y") {
			return;
		}
	}

	// set the values
	key = enterKey();
	iv = enterIv();
	input = enterInput();

}

string encryptStr(string& strKey, string& strIV, string& strPlainText) {
	cout<< "\nEncrypting...,"<< endl;

	vector<Byte> key = hexToBytes(strKey);
	vector<Byte> iv = hexToBytes(strIV);
	vector<Byte> plainText = stringToBytes(strPlainText);

	DES des = DES(key, iv, plainText);
	vector<Byte> cipherText = des.encrypt();

	return bytesToString(cipherText);
}

string decryptStr(string& strKey, string& strIV, string& strCipherText) {
	cout<< "\nDecrypting..., " << endl;

	vector<Byte> key = hexToBytes(strKey);
	vector<Byte> iv = hexToBytes(strIV);
	vector<Byte> cipherText = stringToBytes(strCipherText);
	
	DES des = DES(key, iv, cipherText);

	vector<Byte> plainText = des.decrypt();

	return bytesToString(plainText);
}

int hexCharToInt(char ch) {
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;

	throw string("Invalid Hex Character: " + ch);
}

bool checkValidHexString(const string& str) {
	for (const auto& ch : str) {
		if (!((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')))
			return false;
	}
	return true;
}

vector<Byte> hexToBytes(const string& str) {
	vector<Byte> bytesVector;
	for (int i = 0; i < str.length(); i += 2) {
		int upper4 = hexCharToInt(str[i]);
		int lower4 = hexCharToInt(str[i+1]);		
		bytesVector.push_back((Byte)((upper4 << 4) | lower4));
	}
	return bytesVector;
}

vector<Byte> stringToBytes(const string& str) {
	int len = str.length();
	vector<Byte> bytesVector(len);
	for (int i = 0; i<len; i++)
		bytesVector[i] = (Byte)str[i];
	
	return bytesVector;
}

string bytesToString(const vector<Byte>& bytesVector) {
	string str;
	for (const auto& b : bytesVector)
		str.push_back((char)b);
	return str;
}

string enterKey() {
	cout << "Enter the key: ";
	string enteredKey;
	getline(cin, enteredKey);
	if (enteredKey.length() != 14 && enteredKey.length() != 16) {
		throw string("Invalid Key: Key must be 14 or 16 hex characters long!");
	}

	// check if its valid hex string
	if (!checkValidHexString(enteredKey)) {
		throw string("Invalid Key: Key must be a valid hex string!");
	}

	if (enteredKey.length() == 14) {
		cout << "Warning: Key is 14 hex characters long so 8 parity bits will be added to make it 16 hex chars" << endl;
		string binary;
		for (const auto& ch : enteredKey) {
			int num = hexCharToInt(ch);
			// check the lower 4 bits
			for (int i = 0; i < 4; i++) {
				binary.push_back((num >> i) & 1 ? '1' : '0');
			}
		}
		// add the parity bits
		string addedParity;
		int parityCount = 0;
		for (int i = 0; i < 56; i++) {
			addedParity.push_back(binary[i]);


			if (binary[i] == '1')
				parityCount++;

			// reached the seventh bit so we need to decide what is the parity
			if (i % 7 == 0) {
				// if odd add parity bit 1
				if (parityCount % 2)
					addedParity.push_back('1');
				else
					addedParity.push_back('0');

				// reset the count
				parityCount = 0;
			}
		}

		// convert the binary to hex
		enteredKey.clear();
		for (int i = 0; i < 16; i++) {
			int num = 0;
			// each 4 bit is a hex char
			for (int j = 0; j < 4; j++) {
				num += (addedParity[i * 4 + j] == '1') << (3 - j);
			}
			enteredKey.push_back(num < 10 ? num + '0' : num - 10 + 'A');
		}

		cout << "\n**Entered Key with parity bits: " << enteredKey << '\n' << endl;
	}

	return enteredKey;
}

string enterIv() {
	cout << "Enter the IV: ";
	string enteredIv;
	getline(cin, enteredIv);
	if (enteredIv.length() != 16) {
		throw string("Invalid IV: IV must be 16 hex characters long!");
	}
	if (!checkValidHexString(enteredIv)) {
		throw string("Invalid IV: IV must be a valid hex string!");
	}
	return enteredIv;
}

string enterInput() {
	cout << "Enter the Input: ";
	string enteredInput;
	getline(cin, enteredInput);

	if (enteredInput.length() == 0)
		throw string("Invalid Input: Input must not be empty!");

	short int padding = 8 - (enteredInput.length() % 8);
	if (padding != 8) {
		cout << "**Warning: Input length is not a multiple of 64 bits (8 chars) so it will be padded with nulls\n" << endl;
		for (int i = 0; i < padding; i++)
			enteredInput.push_back('\0');
	}

	return enteredInput;
}
