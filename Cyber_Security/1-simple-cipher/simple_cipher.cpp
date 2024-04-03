// Name: Talal Shafei

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// global variables
// based on the given table
char table[][26] = {
	{'M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A',},
	{'B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M',},
	{'C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B',},
	{'D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C',},
	{'F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D',},
	{'E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F',},
	{'G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E',},
	{'H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G',},
	{'K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H',},
	{'J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K',},
	{'I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J',},
	{'S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I',},
	{'Z','N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S',},
	{'N','O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z',},
	{'O','P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N',},
	{'P','Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O',},
	{'Q','R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P',},
	{'R','L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q',},
	{'L','T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R',},
	{'T','U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L',},
	{'U','X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T',},
	{'X','W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U',},
	{'W','V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X',},
	{'V','Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W',},
	{'Y','A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V',},
	{'A','M','B','C','D','F','E','G','H','K','J','I','S','Z','N','O','P','Q','R','L','T','U','X','W','V','Y',},
};

// core functions
void encrypt(vector<string>&);
void decrypt(vector<string>&);
void menu();

// helper functions
void cleanString(string&); // applies trim and upper case
void padOddString(string&);
void removeOddPadding(string&);
void drawSpacer(const string&&="");

int main(){
	int option = 0;
	
	vector<string> result = {"", "", ""}; //{lastKey, lastPlaintext, lastCipher}

	while (option!=3) {
		// take menu input
		menu();
		cin >> option; cin.ignore(); // ignore \n from input buffer that resulted from the previous cin

		try {

			if (option == 1) {
				encrypt(result);
			}
			else if (option == 2) {
				decrypt(result);
			}
			else if (option == 3) 
				cout << "\n\nGoodbye!" << endl;
			else 
				cout << "\n*** Invalid option **\n" << endl;
			
		}
		catch (char c) {
			cout << "\n\n**** Invalid character: '" << c << "' ****\n"
				 << "Only English Alphabet is Allowed!!\n"
				 << "Please try again\n" << endl;
		}
		catch (const char* msg) {
			cout << "\n\n**** Error: '" << msg << "' ****\n"
				 << "Please try again\n" << endl;
		}
		catch (...) {
			cout << "\n\n**** Unknown Error ****\n"
				 << "Exiting\n" << endl;
			return -1;
		}
	}


	return 0;
    
}

void menu() {
    cout << endl
		 << "Simple Cipher:\n"
		 << "[1] Encrypt\n"
		 << "[2] Decrypt\n"
		 << "[3] Exit\n"
		 << "Selection: ";
}

void encrypt(vector<string>& result) {
	char choice='\0';
	// if last plaintext and key are not empty, ask if the user wants to use them
	if (result[1] != "" && result[0] != "") {
		cout<< "Do you want to use the plain text and key from the last operation? (y/n): ";
		cin >> choice; cin.ignore();
	}

	// take input
	string plainText;
	string key;

	if (choice == 'y' || choice == 'Y') {
		plainText = result[1];
		key = result[0];
	}
	else {
		cout << endl;
		cout << "Enter text: ";
		getline(cin, plainText);
		cleanString(plainText);
		cout << "Enter key: ";
		getline(cin, key);
		cleanString(key);
	}



	drawSpacer(" Encryption ");

	// phase 1
	cout << "Encryption Phase-1 " << endl;
	cout << "Plaintext: " << plainText << endl;

	int textSize = plainText.length();

	// first adjust the key size to be equal to the plaintext
	string adjustedKey(textSize, ' ');
	for (int i = 0; i < textSize; i++) {
		// this will repeat the key until its the same size as the plain text
		// or will truncate the key if it was larger than the plain text
		adjustedKey[i] = key[i % key.length()];
	}
	cout << "Adjusted Key: " << adjustedKey << endl;


	string output1(textSize, ' ');
	for (int i = 0; i < textSize; i++)
		output1[i] = table[adjustedKey[i] - 'A'][plainText[i] - 'A'];
		
		// This for the normal case as in the image in the pdf
		//output1[i] = (plainText[i] + adjustedKey[i]) % 26 + 'A';
	

	cout << "Output (phase 1): " << output1 <<endl;



	// phase 2
	cout << "\n\nEncryption Phase-2 " << endl;
	padOddString(output1);
	textSize = output1.length();

	cout << "Input text: " << output1 << endl;

	string group1 = output1.substr(0, textSize / 2);
	string group2 = output1.substr(textSize / 2, textSize / 2);
	cout << "group-1: " << group1
		 << '\n'
		 << "group-2: " << group2
		 << endl;

	string output2(textSize, ' ');
	for (int i = 0; i < textSize / 2; i++) {
		output2[2*i] = group1[i];
		output2[2*i + 1] = group2[i];
	}


	cout << "\nCiphertext: " << output2 << endl;
	drawSpacer("************");

	result[0] = key;
	result[1] = plainText;
	result[2] = output2;

}

void decrypt(vector<string>& result){
	char choice = '\0';
	// if last cipher and key are not empty, ask if the user wants to use them
	if (result[2] != "" && result[0] != "") {
		cout << "Do you want to use the cipher text and key from the last operation? (y/n): ";
		cin >> choice; cin.ignore();
	}

	// take input
	string cipherText;
	string key;

	if (choice == 'y' || choice == 'Y') {
		cipherText = result[2];
		key = result[0];
	}
	else {
		cout << endl;
		cout << "Enter cipher: ";
		getline(cin, cipherText);

		if (cipherText.length() % 2)
			throw "Cipher Text Shouldn't have odd length";

		removeOddPadding(cipherText);
		cleanString(cipherText);
		cout << "Enter key: ";
		getline(cin, key);
		cleanString(key);
	}

	

	drawSpacer(" Decryption ");

	// phase 2
	cout << "Decryption Phase-2 " << endl;
	cout << "Input text: " << cipherText << endl;

	
	string group1;
	string group2;
	for (int i = 0; i < cipherText.length(); i++) {
		if(i % 2 == 0) group1.push_back(cipherText[i]);
		else if(cipherText[i] != '0') group2.push_back(cipherText[i]); // remove padding
	}

	cout << "group-1: " << group1
		<< '\n'
		<< "group-2: " << group2
		<< endl;

	string output2 = group1 + group2;
	const int textSize = output2.length();
	cout << "Output (phase 2): " << output2 << endl;

	// phase 1
	cout << "\nDecryption Phase-1 " << endl;
	cout << "Input text: " << output2 << endl;

	string adjustedKey(textSize, ' ');
	for (int i = 0; i < textSize; i++) 
		adjustedKey[i] = key[i % key.length()];

	cout << "Adjusted Key: " << adjustedKey << endl;


	string plainText(textSize, ' ');
	for (int i = 0; i < textSize; i++) {
		for (int j = 0; j < 26; j++) {
			if (table[adjustedKey[i] - 'A'][j] == output2[i]) {
				plainText[i] = j + 'A';
				break;
			}
		}
	}
	// This for the normal case as in the image in the pdf
	// +26 to avoid negative numbers
	//for (int i = 0; i < textSize; i++)
	//	plainText[i] = ((26 + (output2[i] - adjustedKey[i])) % 26) + 'A';

	
	

	cout << "\nPlaintext: " << plainText << endl;
	drawSpacer("************");

	result[0] = key;
	result[1] = plainText;
	result[2] = output2;
}


// helpers
void cleanString(string& s) {
	// remove whitespaces
	s.erase(remove(s.begin(), s.end(), ' '), s.end());

	// convert to upper case letters
	for (int i = 0; i < s.length(); i++) {
		if(s[i] >= 'a' && s[i] <= 'z')s[i] = s[i] - 32;

		// if it has any non-alphabetic characters, raise error
		if (s[i] < 'A' || s[i] > 'Z') {
			throw s[i];
		}
	}
}

void drawSpacer(const string&& title) {
	cout << "\n**********************************************"
		 << title 
		 << "**********************************************\n" 
		 << endl;
}

void padOddString(string& s) {
	if (s.length() % 2) {
		s.push_back('0');
	}
}

void removeOddPadding(string& s) {
	s.erase(remove(s.begin(), s.end(), '0'), s.end());
}