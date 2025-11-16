#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

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

int main() {
    
    cout << "\n----- CryptX (v0.1) -----\n";

    cout << "\n----- XOR Cipher Tool (ENCRYPT TEXT) -----\n";
    
    encryptText();
        
    return 0;
}
