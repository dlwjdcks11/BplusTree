#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

int blockSize;

class Node {
public:
	bool isLeaf;
	pair<int, int>* keyPair;
	Node** next;
	Node();
};

Node::Node() {
	this->isLeaf = false;
	this->keyPair = new pair<int, int>[(blockSize - 4) / 4];
	this->next = new Node * [(blockSize - 4) / 4 + 1];
}

class BTree {
public:
	const char* fileName;

	BTree(const char*, int);
	bool insert(int, int);
	void print();
	int* search(int);
	int* search(int, int);
};

BTree::BTree(const char* _fileName, int _blockSize) {
	this->fileName = _fileName;
	blockSize = _blockSize;
}

bool BTree::insert(int key, int rid) {

}

void BTree::print() {

}

int* BTree::search(int key) {

}

int* BTree::search(int startRange, int endRange) {

}

int main(int argc, char* argv[]) {
	char command = argv[1][0];
	BTree* btree = new BTree("", 0);

	switch (command) {
	case 'c':
		break;
	case 'i':
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