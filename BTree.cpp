#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

const char* fileName;

class BTree {
public:
	BTree(const char*, int);
	bool insert(int, int);
	int* search(int);
	int* search(int, int);
	void print();
};

BTree::BTree(const char* _fileName, int _blockSize) {
	const int RootBID = 1;
	const int Depth = 0;

	ofstream makeBin;
	makeBin.open(_fileName, ios::out | ios::binary);
	makeBin.write((char*)&_blockSize, sizeof(_blockSize));
	makeBin.write((char*)&RootBID, sizeof(RootBID));
	makeBin.write((char*)&Depth, sizeof(Depth));
}

bool BTree::insert(int _key, int _rid) {
	//ifstream myFile(fileName, ios::in | ios::binary);
	//int val;
	//myFile.read((char*)&val, sizeof(int));

	//cout << val << endl;

	FILE* pFile;
	long iSize;
	char* buffer;
	size_t result;

	pFile = fopen(fileName, "rb");
	fseek(pFile, 0, SEEK_END);
	iSize = ftell(pFile);
	rewind(pFile);
	buffer = (char*)malloc(sizeof(char) * iSize);
	result = fread(buffer, 1, iSize, pFile);
	
	for (int i = 0; i < 12; i++) {
		cout << (int)buffer[i] << " ";
	}
	cout << endl;

	return false;
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
	fileName = argv[2];

	BTree* btree = new BTree("", 0);
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