#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
using namespace std;


struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char c, int f) {
        ch = c;
        freq = f;
        left = right = NULL;
    }
};


struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};


unordered_map<char, string> huffmanCode;


void generateCodes(Node* root, string code) {
    if (!root) return;

    // Leaf node
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = code;
    }

    generateCodes(root->left, code + "0");
    generateCodes(root->right, code + "1");
}


Node* buildHuffmanTree(unordered_map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        Node* newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);
    }

    return pq.top();
}


void compressFile(string inputFile, string compressedFile) {
    ifstream in(inputFile);
    ofstream out(compressedFile, ios::binary);

    unordered_map<char, int> freq;
    char ch;

    while (in.get(ch))
        freq[ch]++;

    Node* root = buildHuffmanTree(freq);
    generateCodes(root, "");

    
    out << freq.size() << '\n';
    for (auto p : freq)
        out << p.first << " " << p.second << '\n';

    in.clear();
    in.seekg(0);

    string encoded = "";
    while (in.get(ch))
        encoded += huffmanCode[ch];

    out << encoded;

    in.close();
    out.close();

    cout << "File Compressed Successfully!\n";
}


void decompressFile(string compressedFile, string outputFile) {
    ifstream in(compressedFile, ios::binary);
    ofstream out(outputFile);

    unordered_map<char, int> freq;
    int n;
    in >> n;

    for (int i = 0; i < n; i++) {
        char ch;
        int f;
        in >> ch >> f;
        freq[ch] = f;
    }

    Node* root = buildHuffmanTree(freq);
    in.ignore();

    string bits;
    getline(in, bits);

    Node* curr = root;
    for (char bit : bits) {
        if (bit == '0')
            curr = curr->left;
        else
            curr = curr->right;

        if (!curr->left && !curr->right) {
            out << curr->ch;
            curr = root;
        }
    }

    in.close();
    out.close();

    cout << "File Decompressed Successfully!\n";
}


int main() {
    int choice;
    cout << "1. Compress File\n2. Decompress File\nEnter choice: ";
    cin >> choice;

    if (choice == 1)
        compressFile("input.txt", "compressed.bin");
    else if (choice == 2)
        decompressFile("compressed.bin", "decompressed.txt");
    else
        cout << "Invalid choice\n";

    return 0;
}
