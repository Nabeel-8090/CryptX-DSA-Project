#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <fstream> // file input/output (ifstream, ofstream)
#include <sstream> // stringstream for building strings
#include <iomanip> // hex, setw, setfill
#include <cctype> // isxdigit()
#include <ctime>

using namespace std;

/* =========================================================
                    OPERATION HISTORY STACK
   ========================================================= */

class HistoryNode {
public:
    string operation;
    string details;
    string timestamp;
    HistoryNode* next;
    
    HistoryNode(string op, string det) {
        operation = op;
        details = det;
        
        // Get current timestamp
        time_t now = time(0);
        timestamp = ctime(&now);
        timestamp.pop_back(); // Remove newline
        
        next = NULL;
    }
    
    void display() {
        cout << "[" << timestamp << "] ";
        cout << operation << " - " << details << endl;
    }
};

class HistoryStack {
private:
    HistoryNode* top;
    
public:
    HistoryStack() {
        top = NULL;
    }
    
    void push(string operation, string details) {
        HistoryNode* newNode = new HistoryNode(operation, details);
        newNode->next = top;
        top = newNode;
        cout << "Added to history: " << operation << endl;
    }
    
    void displayHistory() {
        if(top == NULL) {
            cout << "No history available." << endl;
            return;
        }
        
        cout << "\n=== OPERATION HISTORY (Most Recent First) ===\n";
        HistoryNode* current = top;
        int count = 1;
        
        while(current != nullptr) {
            cout << count << ". ";
            current->display();
            current = current->next;
            count++;
        }
        cout << "===========================================\n";
    }
    
    void clearHistory() {
        while(top != nullptr) {
            HistoryNode* temp = top;
            top = top->next;
            delete temp;
        }
        cout << "History cleared." << endl;
    }
    
    ~HistoryStack() { // destructor
        while(top != nullptr) {
            HistoryNode* temp = top;
            top = top->next;
            delete temp;
        }
    }
};

// Global history stack
HistoryStack operationHistory;

/* =========================================================
                    HUFFMAN COMPRESSION
   ========================================================= */

class Node {
public:
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char c, int f) {
        ch = c;
        freq = f;
        left = right = NULL;
    }
};

/* ==========================
    MIN HEAP COMPARATOR
   ========================== */
class Compare {
public:
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

/* ==========================
   BUILD HUFFMAN TREE
   ========================== */
Node* buildTree(map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for(auto p : freq) {
        pq.push(new Node(p.first, p.second));
    }

    while(pq.size() > 1) {
        Node* l = pq.top(); 
        pq.pop();
        Node* r = pq.top(); 
        pq.pop();
        
        Node* parent = new Node('\0', l->freq + r->freq);
        parent->left = l;
        parent->right = r;
        pq.push(parent);
    }
    return pq.top();
}

/* ==========================
   GENERATE HUFFMAN CODES
   ========================== */
void generateCodes(Node* root, string code, map<char, string>& codes) {
    if(root == NULL) {
        return;
    }
    if(root->left == NULL && root->right == NULL) {
        codes[root->ch] = code;
    }
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

/* ==========================
   COMPRESS FILE (Huffman)
   ========================== */
void compressFile(string inputFile) {
    ifstream fin(inputFile);
    if(!fin) {
        cout << "Error: Cannot open file!" << endl;
        return;
    }

    string text((istreambuf_iterator<char>(fin)), {});
    fin.close();

    map<char, int> freq;
    for(char c : text) {
        freq[c]++;
    }

    Node* root = buildTree(freq);

    map<char, string> codes;
    generateCodes(root, "", codes);

    string encoded;
    for(char c : text) {
        encoded += codes[c];
    }

    string outputFile = inputFile + ".zip";
    ofstream fout(outputFile, ios::binary);

    // Write frequency table
    int size = freq.size();
    fout.write((char*)&size, sizeof(size));
    for(auto p : freq) {
        fout.write(&p.first, sizeof(char));
        fout.write((char*)&p.second, sizeof(int));
    }

    // Padding
    int padding = (8 - encoded.size() % 8) % 8;
    fout.write((char*)&padding, sizeof(padding));

    unsigned char byte = 0;
    int count = 0;
    for(char bit : encoded) {
        byte = (byte << 1) | (bit - '0');
        count++;
        if(count == 8) {
            fout.put(byte);
            byte = 0;
            count = 0;
        }
    }
    if(count > 0) {
        byte <<= (8 - count);
        fout.put(byte);
    }

    fout.close();

    // Add to history
    operationHistory.push("Compress File", inputFile + " -> " + outputFile);
    cout << "Compression successful -> " << outputFile << endl;
}

/* ==========================
   DECODE FILE (Huffman)
   save = true → writes .txt file
   save = false → just returns string
   ========================== */
string decodeFile(string inputFile, bool save) {
    ifstream fin(inputFile, ios::binary);
    if(!fin) {
        cout << "Error: Cannot open file!" << endl;
        return "";
    }

    // Read frequency table
    int size;
    fin.read((char*)&size, sizeof(size));
    map<char, int> freq;
    for(int i=0; i<size; i++){
        char c; int f;
        fin.read(&c, sizeof(char));
        fin.read((char*)&f, sizeof(int));
        freq[c] = f;
    }

    Node* root = buildTree(freq);

    // Read padding info
    int padding;
    fin.read((char*)&padding, sizeof(padding));

    vector<unsigned char> data((istreambuf_iterator<char>(fin)), {});
    fin.close();

    // Decode Huffman encoded data
    string result;
    Node* cur = root;
    for(int i=0; i<data.size(); i++){
        int bits = (i == data.size()-1) ? 8-padding : 8;
        for(int b=0; b<bits; b++) {
            cur = (data[i] & (1 << (7-b))) ? cur->right : cur->left;
            if(cur->left == NULL && cur->right == NULL){
                result += cur->ch;
                cur = root;
            }
        }
    }

    // Save decompressed file
    if(save){
        // Determine original filename and add "_dec" before extension
        size_t lastSlash = inputFile.find_last_of("/\\");
        string filename = (lastSlash == string::npos) ? inputFile : inputFile.substr(lastSlash+1);

        // Remove .zip extension if present
        size_t dotPos = filename.rfind(".zip");
        string baseName = (dotPos != string::npos) ? filename.substr(0, dotPos) : filename;

        // Create decompressed filename: baseName + "_dec" + original extension
        string decFileName;
        size_t extPos = baseName.rfind('.');
        if(extPos != string::npos){
            decFileName = baseName.substr(0, extPos) + "_dec" + baseName.substr(extPos);
        } else {
            decFileName = baseName + "_dec.txt";
        }

        ofstream fout(decFileName);
        fout << result;
        fout.close();

        // Add to history
        operationHistory.push("Decompress File", inputFile + " -> " + decFileName);

        cout << "Decompressed -> " << decFileName << endl;
    }

    return result;
}


/* =========================================================
                XOR ENCRYPTION & DECRYPTION
   ========================================================= */

// File Encryption
void encryptFile() {

    string inputFile, outputFile, key;

    cout << "Enter input file path: ";
    getline(cin, inputFile);

    cout << "Enter key: ";
    getline(cin, key);

    if(key.empty()) {
        cout << "Key cannot be empty!" << endl;
        return;
    }

    ifstream fin(inputFile, ios::binary); // read file in binary mode
    if(!fin) {
        cout << "Error opening input file!" << endl;
        return;
    }

    outputFile = inputFile + ".enc"; // Encrypted file

    ofstream fout(outputFile, ios::binary); // open file in binary mode
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

    // Add to history
    operationHistory.push("Encrypt File", inputFile + " -> " + outputFile);

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

    ifstream fin(inputFile, ios::binary); // read file in binary mode
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

    // Add to history
    operationHistory.push("Decrypt File", inputFile + " -> " + outputFile);
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
        cout << "Key cannot be empty!" << endl;
        return;
    }

    stringstream res;
    int keyItr = 0;

    for(char c : msg) {
        char encryptedByte = c ^ key[keyItr];
        res << hex << setfill('0') << setw(2) << (int)(unsigned char)encryptedByte;

        keyItr = (keyItr + 1) % key.length();
    }

    // Add to history
    operationHistory.push("Encrypt Text", "Text length: " + to_string(msg.length()) + " chars");

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
    for(size_t i=0; i<msg.length(); i+=2) { // size_t: Starts at 0 and goes up
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

    // Add to history
    operationHistory.push("Decrypt Text", "HEX length: " + to_string(msg.length()) + " chars");

    cout << "Decrypted Text: " << decryptedText << endl;
}

/* =========================================================
                          MAIN MENU
   ========================================================= */

int main() {

    int choice;
    string file;

    do {
        cout << "\n====== CryptX (ALL FEATURES) ======" << endl;
        cout << "1. Compress File (Huffman)" << endl;
        cout << "2. Decompress File" << endl;
        cout << "3. Display Compressed Content" << endl;
        cout << "4. Encrypt File (XOR)" << endl;
        cout << "5. Decrypt File (XOR)" << endl;
        cout << "6. Encrypt Text" << endl;
        cout << "7. Decrypt Text" << endl;
        cout << "8. View Operation History" << endl;
        cout << "9. Clear History" << endl;
        cout << "Choice: ";
        cin >> choice;
        cin.ignore(); // Clear newline

        switch(choice) {
            case 1: 
                cout << "File: "; 
                cin >> file; 
                compressFile(file); 
                break;
            case 2: 
                cout << "File: "; 
                cin >> file; 
                decodeFile(file, true); 
                break;
            case 3: 
                cout << "File: "; 
                cin >> file; 
                cout << decodeFile(file, false) << endl; 
                break;
            case 4: 
                encryptFile(); 
                break;
            case 5: 
                decryptFile(); 
                break;
            case 6: 
                encryptText(); 
                break;
            case 7:  
                decryptText(); 
                break;
            case 8:
                operationHistory.displayHistory();
                break;
            case 9:
                operationHistory.clearHistory();
                break;
            case 10:
                cout << "Thank you for using CryptX!" << endl;
                break;
            default: 
                cout << "Invalid choice!" << endl;
        }
    } while(choice!=10);

    return 0;
}
