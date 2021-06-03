#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class BTree {
public:
	const char* fileName;
	int blockSize;

	BTree(const char*, int);
	bool insert(int, int);
	int* search(int);
	int* search(int, int);
	void print();
};

BTree::BTree(const char* _fileName, int _blockSize) {
	const int RootBID = 1;
	const int Depth = 0;
	this->fileName = _fileName;
	this->blockSize = _blockSize;

	ofstream makeBin;
	makeBin.open(fileName, ios::out | ios::binary);
	makeBin.write((char*)&blockSize, sizeof(blockSize));
	makeBin.write((char*)&RootBID, sizeof(RootBID));
	makeBin.write((char*)&Depth, sizeof(Depth));
}

bool BTree::insert(int _key, int _rid) {
	ofstream insertNode;
	insertNode.open("btree.bin", ios::out | ios::binary);
	

}

int* BTree::search(int _key) {

}

int* BTree::search(int _startRange, int _endRange) {

}

void BTree::print() {

}

vector<string> split(string s, string div) {
	vector<string> v;
	char* c = strtok((char*)s.c_str(), div.c_str());
	while (c) {
		v.push_back(c);
		c = strtok(NULL, div.c_str());
	}
	return v;
}

int main(int argc, char* argv[]) {
	char command = argv[1][0];
	BTree* btree;
	ifstream openText;
	string buffer;

	// btree.exe c btree.bin 36 기준으로,
	// argv[1][0] == c
	// argv[2] == btree.bin
	// argv[3] == 36

	switch (command) {
	case 'c':
		btree = new BTree(argv[2], atoi(argv[3]));
		break;
	case 'i':
		openText.open(argv[3]);

		while (openText.peek() != EOF) {
			getline(openText, buffer);
			vector<string> input = split(buffer, ",");
			
			btree->insert(stoi(input[0]), stoi(input[1]));
		}

		break;
	case 's':
		break;
	case 'r':
		break;
	case 'p':
		break;

	default:
		break;
	}

	return 0;
}