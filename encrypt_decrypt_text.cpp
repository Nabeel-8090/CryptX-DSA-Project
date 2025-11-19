#include <iostream>
#include <sstream>    // stringstream for building strings
#include <iomanip>    // hex, setw, setfill
#include <cctype>     // isxdigit()

using namespace std;

// Text Encryption
void encryptText() {

    string msg, key;

    cout << "Enter text to encrypt: ";
    getline(cin, msg);

    cout << "Enter key: ";
    getline(cin, key);

    if(key.empty()) { // key cannot be empty
        cout << "Key cannot be empty!";
        return;
    }

    stringstream res; // String stream to build encrypted HEX string
    int keyItr = 0;

    for(char c : msg) {
        char encryptedByte = c ^ key[keyItr]; // XOR current char with key char

        // Convert the XOR result to HEX (2 digits) and append to result
        res << hex << setfill('0') << setw(2) << (int)(unsigned char)encryptedByte;

        // Move to next key character, wrap around if end is reached
        keyItr = (keyItr+1) % key.length();
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

// Main Function
int main() {
    
    cout << "\n----- CryptX (v0.2) -----\n";

    int choice;

    do {
        cout << "\n----- XOR Cipher Tool -----\n";
        cout << "1. Encrypt Text\n";
        cout << "2. Decrypt Text\n";
        cout << "3. Exit\n";
        cout << "Choose: ";

        cin >> choice;
        cin.ignore(); // Clear newline

        switch(choice) {
            case 1: 
                encryptText(); 
                break;
            case 2: 
                decryptText(); 
                break;
            case 3: 
                cout << "Exiting." << endl; break;
            default: 
                cout << "Invalid choice!" << endl;
        }
    } while(choice != 3);

    return 0;
}
