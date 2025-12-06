#include <iostream>
#include <fstream>  // file input/output (ifstream, ofstream)
#include <sstream>  // stringstream for building strings
#include <iomanip>  // hex, setw, setfill
#include <cctype>   // isxdigit()

using namespace std;

// File Encryption
void encryptFile() {

    string inputFile, outputFile, key;

    cout << "Enter input file path: ";
    getline(cin, inputFile);

    cout << "Enter key: ";
    getline(cin, key);

    ifstream fin(inputFile, ios::binary);
    if(!fin) {
        cout << "Error opening input file!" << endl;
        return;
    }

    outputFile = inputFile + ".enc"; // Encrypted file

    ofstream fout(outputFile, ios::binary);
    if(!fout) {
        cout << "Error creating output file!" << endl;
        return;
    }

    char ch;
    int keyItr = 0;

    while(fin.get(ch)) {
        char encryptedByte = ch ^ key[keyItr];
        fout.put(encryptedByte);

        keyItr = (keyItr + 1) % key.length();
    }

    cout << "File encrypted successfully!\nEncrypted file: " << outputFile << endl;

    fin.close();
    fout.close();
}

// File Decryption
void decryptFile() {

    string inputFile, outputFile, key;

    cout << "Enter encrypted file path: ";
    getline(cin, inputFile);

    cout << "Enter key: ";
    getline(cin, key);

    ifstream fin(inputFile, ios::binary);
    if(!fin) {
        cout << "Error opening encrypted file!" << endl;
        return;
    }

    size_t lastSlash = inputFile.find_last_of("/\\"); // handle folder path
    string filename = (lastSlash == string::npos) ? inputFile : inputFile.substr(lastSlash + 1);

    size_t dotPos = filename.rfind(".enc"); // find .enc at the end
    string baseName, extension;

    if(dotPos != string::npos) {
        baseName = filename.substr(0, dotPos); // before .enc
        size_t extDot = baseName.rfind(".");
        if (extDot != string::npos) {
            extension = baseName.substr(extDot); // original extension
            baseName = baseName.substr(0, extDot); // filename without extension
        } else {
            extension = ""; // no original extension
        }
    } else {
        baseName = filename;
        extension = "";
    }

    outputFile = baseName + "_decrypted" + extension;

    ofstream fout(outputFile, ios::binary);
    if(!fout) {
        cout << "Error creating output file!" << endl;
        return;
    }

    char ch;
    int keyItr = 0;

    while(fin.get(ch)) {
        char decryptedByte = ch ^ key[keyItr];
        fout.put(decryptedByte);

        keyItr = (keyItr + 1) % key.length();
    }

    cout << "File decrypted successfully!\nDecrypted file: " << outputFile << endl;

    fin.close();
    fout.close();
}

// Text Encryption
void encryptText() {

    string msg, key;

    cout << "Enter text to encrypt: ";
    getline(cin, msg);

    cout << "Enter key: ";
    getline(cin, key);

    if(key.empty()) {
        cout << "Key cannot be empty!";
        return;
    }

    stringstream res;
    int keyItr = 0;

    for(char c : msg) {
        char encryptedByte = c ^ key[keyItr];
        res << hex << setfill('0') << setw(2) << (int)(unsigned char)encryptedByte;

        keyItr = (keyItr + 1) % key.length();
    }

    cout << "Encrypted Text (HEX): " << res.str() << endl;
}

// Text Decryption
void decryptText() {

    string msg, key;

    cout << "Enter HEX encrypted text: ";
    getline(cin, msg);

    // Validation: HEX string must have even length
    if(msg.length()%2 != 0) {
        cout << "Invalid HEX input!";
        return;
    } 

    for(char c : msg) {
        if(!isxdigit(c)) { // HEX string must contain only valid HEX characters
            cout << "Invalid HEX characters detected!" << endl;
            return;
        }
    }

    cout << "Enter key: ";
    getline(cin, key);

    if(key.empty()) { // key cannot be empty
        cout << "Key cannot be empty!";
        return;
    }

    // Convert HEX string back to raw bytes
    string rawData;
    for(size_t i=0; i<msg.length(); i+=2) {
        string hexByte = msg.substr(i, 2); // take 2 HEX digits
        char byte = (char)strtol(hexByte.c_str(), NULL, 16); // convert HEX to char
        rawData += byte; // append to raw data
    }

    string decryptedText;
    int keyItr = 0;

    for(char c : rawData) {
        char decryptedByte = c ^ key[keyItr]; // XOR with key char
        decryptedText += decryptedByte;

        // Move to next key character, wrap around if needed
        keyItr = (keyItr+1) % key.length();
    }

    cout << "Decrypted Text: " << decryptedText << endl;
}

int main() {

    cout << "\n----- CryptX (v0.3) -----" << endl;

    int choice;

    do {
        cout << "\n----- XOR Cipher Tool -----" << endl;
        cout << "1. Encrypt File" << endl;
        cout << "2. Decrypt File" << endl;
        cout << "3. Encrypt Text" << endl;
        cout << "4. Decrypt Text" << endl;
        cout << "5. Exit" << endl;
        cout << "Choose: ";

        cin >> choice;
        cin.ignore(); // Clear newline

        switch(choice) {
            case 1: 
                encryptFile(); 
                break;
            case 2: 
                decryptFile(); 
                break;
            case 3: 
                encryptText();  
                break;
            case 4: 
                decryptText(); 
                break;
            case 5: 
                cout << "Exiting." << endl;
                break;
            default: 
                cout << "Invalid choice!" << endl;
        }
    } while(choice != 5);

    return 0;
}
