#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

const char* fileName;
const char* outputFileName;

class BTree {
public:
	BTree(const char*, int);
	int insert(int, int, int, int);
	int search(int);
	vector<pair<int, int>> search(int, int);
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

int BTree::insert(int _bid, int _key, int _rid, int _depth) {
	FILE* pFile;
	long iSize;
	int* buffer;
	size_t result;

	pFile = fopen(fileName, "rb");
	fseek(pFile, 0, SEEK_END);
	iSize = ftell(pFile);
	rewind(pFile);
	buffer = (int*)malloc(sizeof(char) * iSize);
	result = fread(buffer, 1, iSize, pFile);
	fclose(pFile);
 
	int blockSize = (int)buffer[0];
	int MAX = ((int)buffer[0] - 4) / 8;
	int root = (int)buffer[1];
	int depth = (int)buffer[2];
	char init[] = { 0, 0, 0, 0 };

	// Physical offset of a Block ID = 12 + ((BID-1) * BlockSize)
	if (result == 12) {
		pFile = fopen(fileName, "ab");

		for (int i = 0; i < MAX * 2 + 1; i++)
			fwrite(init, sizeof(char), sizeof(init), pFile);

		fclose(pFile);
	}

	vector<pair<int, int>> v;

	pFile = fopen(fileName, "rb");
	fseek(pFile, 0, SEEK_END);
	iSize = ftell(pFile);
	rewind(pFile);
	buffer = (int*)malloc(sizeof(char) * iSize);

	fseek(pFile, 12 + (_bid - 1) * blockSize, SEEK_SET);

	result = fread(buffer, 1, iSize, pFile);
	fclose(pFile);

	int totalBlock = (result - 12) / blockSize;

	if (depth == _depth) {
		for (int i = 0; i < blockSize / 4 - 1; i += 2) {
			if (buffer[i] != 0) {
				v.push_back(pair<int, int>(buffer[i], buffer[i + 1]));
			}
		}

		v.push_back(pair<int, int>(_key, _rid));
		sort(v.begin(), v.end());

		if (MAX + 1 == v.size()) {
			pFile = fopen(fileName, "rb+");

			if (depth == 0) {
				totalBlock++;
				int newBlock1 = totalBlock;

				totalBlock++;
				int newBlock2 = totalBlock;

				fseek(pFile, 12 + (_bid - 1) * blockSize, SEEK_SET);

				for (int i = 0; i < v.size() / 2; i++) {
					fwrite((char*)&v[i].first, sizeof(int), 1, pFile);
					fwrite((char*)&v[i].second, sizeof(int), 1, pFile);
				}

				int tmpSize = v.size();
				if (tmpSize % 2 == 1)
					tmpSize--;

				for (int i = 0; i < blockSize / 4 - tmpSize - 1; i++)
					fwrite(init, sizeof(char), sizeof(init), pFile);
				fwrite((char*)&newBlock2, sizeof(int), 1, pFile);

				fseek(pFile, 12 + (newBlock2 - 1) * blockSize, SEEK_SET);

				for (int i = v.size() / 2; i < v.size(); i++) {
					fwrite((char*)&v[i].first, sizeof(int), 1, pFile);
					fwrite((char*)&v[i].second, sizeof(int), 1, pFile);
				}
				for (int i = 0; i < blockSize / 4 - (v.size() - v.size() / 2) * 2; i++)
					fwrite(init, sizeof(char), sizeof(init), pFile);

				fseek(pFile, 12 + (newBlock1 - 1) * blockSize, SEEK_SET);

				fwrite((char*)&_bid, sizeof(int), 1, pFile);
				fwrite((char*)&v[v.size() / 2].first, sizeof(int), 1, pFile);
				fwrite((char*)&newBlock2, sizeof(int), 1, pFile);

				for (int i = 0; i < blockSize / 4 - 3; i++)
					fwrite(init, sizeof(char), sizeof(init), pFile);

				depth++;
				root = newBlock1;
				fseek(pFile, 4, SEEK_SET);
				fwrite((char*)&root, sizeof(int), 1, pFile);
				fwrite((char*)&depth, sizeof(int), 1, pFile);
				fclose(pFile);
			
				return 0;
			}
			else {
				fseek(pFile, 12 + (_bid - 1) * blockSize, SEEK_SET);
				totalBlock++;

				for (int i = 0; i < v.size() / 2; i++) {
					fwrite((char*)&v[i].first, sizeof(int), 1, pFile);
					fwrite((char*)&v[i].second, sizeof(int), 1, pFile);
				}

				int tmpSize = v.size();
				if (tmpSize % 2 == 1)
					tmpSize--;

				for (int i = 0; i < blockSize / 4 - tmpSize - 1; i++)
					fwrite(init, sizeof(char), sizeof(init), pFile);
				fwrite((char*)&totalBlock, sizeof(int), 1, pFile);
				fseek(pFile, 12 + (totalBlock - 1) * blockSize, SEEK_CUR);

				for (int i = v.size() / 2; i < v.size(); i++) {
					fwrite((char*)&v[i].first, sizeof(int), 1, pFile);
					fwrite((char*)&v[i].second, sizeof(int), 1, pFile);
				}
				for (int i = 0; i < blockSize / 4 - (v.size() - v.size() / 2) * 2; i++)
					fwrite(init, sizeof(char), sizeof(init), pFile);
				fwrite((char*)&buffer[blockSize / 4 - 1], sizeof(int), 1, pFile);
				fclose(pFile);

				return v[v.size() / 2].first;
			}
		}
		else {
			pFile = fopen(fileName, "rb+");
			fseek(pFile, 12 + (_bid - 1) * blockSize, SEEK_SET);

			for (int i = 0; i < v.size(); i++) {
				buffer[i * 2] = v[i].first;
				buffer[i * 2 + 1] = v[i].second;
			}

			for (int i = 0; i < blockSize / 4; i++)
				fwrite((char*)&buffer[i], sizeof(int), 1, pFile);

			fclose(pFile);
		}
	}
	else {
		int current;
		if (_key < buffer[1])
			current = buffer[1];
		else {
			for (int i = 1; i < blockSize / 4; i += 2) {
				if (buffer[i] <= _key && buffer[i] != 0)
					current = buffer[i + 1];
				else
					break;
			}
		}

		int underValue = insert(current, _key, _rid, _depth + 1);

		if (underValue != 0) {
			for (int i = 1; i < blockSize / 4; i += 2) {
				if (buffer[i] != 0)
					v.push_back(pair<int, int>(buffer[i], buffer[i + 1]));
				else
					break;
			}
			v.push_back(pair<int, int>(underValue, totalBlock));
			sort(v.begin(), v.end());

			if (MAX + 1 == v.size()) {
				if (_depth == 0) {
					pFile = fopen(fileName, "rb+");

					totalBlock++;
					int newBlock1 = totalBlock;

					totalBlock++;
					int newBlock2 = totalBlock;

					fseek(pFile, 12 + (_bid - 1) * blockSize, SEEK_SET);
					fwrite((char*)&buffer[0], sizeof(int), 1, pFile);

					for (int i = 0; i < v.size() / 2; i++) {
						fwrite((char*)&v[i].first, sizeof(int), 1, pFile);
						fwrite((char*)&v[i].second, sizeof(int), 1, pFile);
					}

					int tmpSize = v.size();
					if (tmpSize % 2 == 1)
						tmpSize--;

					for (int i = 0; i < blockSize / 4 - tmpSize - 1; i++)
						fwrite(init, sizeof(char), sizeof(init), pFile);

					fseek(pFile, 12 + (newBlock1 - 1) * blockSize, SEEK_SET);
					fwrite((char*)&_bid, sizeof(int), 1, pFile);
					fwrite((char*)&v[v.size() / 2].first, sizeof(int), 1, pFile);
					fwrite((char*)&newBlock2, sizeof(int), 1, pFile);

					for (int i = 3; i < blockSize / 4; i++)
						fwrite(init, sizeof(char), sizeof(init), pFile);
					
					fwrite((char*)&v[v.size() / 2].second, sizeof(int), 1, pFile);

					for (int i = v.size() / 2 + 1; i < v.size(); i++) {
						fwrite((char*)&v[i].first, sizeof(int), 1, pFile);
						fwrite((char*)&v[i].second, sizeof(int), 1, pFile);
					}

					for (int i = 0; i < blockSize / 4 - v.size() * 2 + tmpSize + 1; i++)
						fwrite(init, sizeof(char), sizeof(init), pFile);

					fclose(pFile);
					root = newBlock1;
					depth++;

					fseek(pFile, 4, SEEK_SET);
					fwrite((char*)&root, sizeof(int), 1, pFile);
					fwrite((char*)&depth, sizeof(int), 1, pFile);
					fclose(pFile);
				}
				else {
					totalBlock++;

					pFile = fopen(fileName, "rb+");
					fseek(pFile, 12 + (_bid - 1) * blockSize, SEEK_SET);
					fwrite((char*)&buffer[0], sizeof(int), 1, pFile);

					for (int i = 0; i < v.size() / 2; i++) {
						fwrite((char*)&v[i].first, sizeof(int), 1, pFile);
						fwrite((char*)&v[i].second, sizeof(int), 1, pFile);
					}

					int tmpSize = v.size();
					if (tmpSize % 2 == 1)
						tmpSize--;

					for (int i = 0; i < blockSize / 4 - tmpSize - 1; i++)
						fwrite(init, sizeof(char), sizeof(init), pFile);

					fseek(pFile, 12 + (totalBlock - 1) * blockSize, SEEK_SET);
					fwrite((char*)&v[v.size() / 2].second, sizeof(int), 1, pFile);

					for (int i = v.size() / 2 + 1; i < v.size(); i++) {
						fwrite((char*)&v[i].first, sizeof(int), 1, pFile);
						fwrite((char*)&v[i].second, sizeof(int), 1, pFile);
					}

					for (int i = 0; i < blockSize / 4 - v.size() * 2 + tmpSize + 1; i++)
						fwrite(init, sizeof(char), sizeof(init), pFile);

					fclose(pFile);

					return v[v.size() / 2].first;
				}
			}
			else {
				pFile = fopen(fileName, "rb+");
				fseek(pFile, 12 + (_bid - 1) * blockSize, SEEK_SET);
				fwrite((char*)&buffer[0], sizeof(int), 1, pFile);

				for (int i = 0; i < v.size(); i++) {
					fwrite((char*)&v[i].first, sizeof(int), 1, pFile);
					fwrite((char*)&v[i].second, sizeof(int), 1, pFile);
				}

				fclose(pFile);
			}
		}
	}
}

int BTree::search(int _key) {
	FILE* pFile;
	long iSize;
	int* buffer;
	size_t result; 

	pFile = fopen(fileName, "rb");
	fseek(pFile, 0, SEEK_END);
	iSize = ftell(pFile);
	rewind(pFile);
	buffer = (int*)malloc(sizeof(char) * iSize);
	result = fread(buffer, 1, iSize, pFile);
	fclose(pFile);

	int blockSize = (int)buffer[0];
	int current = (int)buffer[1];
	int depth = (int)buffer[2];

	for (int i = 0; i <= depth; i++) {
		fseek(pFile, 12 + (current - 1) * blockSize, SEEK_SET);

		if (i != depth) {
			result = fread(buffer, 1, iSize, pFile);

			current = buffer[0];

			for (int i = 1; i < blockSize / 4 && buffer[i] != 0; i += 2) {
				if (buffer[i] <= _key)
					current = buffer[i + 1];
				else
					break;
			}
		}
		else {
			result = fread(buffer, 1, iSize, pFile);

			for (int i = 0; i < blockSize / 4 - 1; i += 2) {
				if (_key == buffer[i] && buffer[i] != 0)
					return buffer[i + 1];
			}
		}
	}
}

vector<pair<int, int>> BTree::search(int _startRange, int _endRange) {
	FILE* pFile;
	long iSize;
	int* buffer;
	size_t result;
	vector<pair<int, int>> v;

	pFile = fopen(fileName, "rb");
	fseek(pFile, 0, SEEK_END);
	iSize = ftell(pFile);
	rewind(pFile);
	buffer = (int*)malloc(sizeof(char) * iSize);
	result = fread(buffer, 1, iSize, pFile);
	fclose(pFile);

	int blockSize = (int)buffer[0];
	int current = (int)buffer[1];
	int depth = (int)buffer[2];

	for (int i = 0; i <= depth; i++) {
		fseek(pFile, 12 + (current - 1) * blockSize, SEEK_SET);

		if (i != depth) {
			result = fread(buffer, 1, iSize, pFile);
			current = buffer[0];

			for (int i = 1; i < blockSize / 4; i += 2) {
				if (buffer[i] <= _startRange && buffer[i] != 0)
					current = buffer[i + 1];
				else
					break;
			}
		}
		else {
			result = fread(buffer, 1, iSize, pFile);

			for (int i = 0; i < blockSize / 4 - 1 && buffer[i] != 0; i += 2) {
				if (_startRange <= buffer[i]) 
					v.push_back(pair<int, int>(buffer[i], buffer[i + 1]));
			}

			while (buffer[0] <= _endRange) {
				current = buffer[blockSize / 4 - 1];
				fseek(pFile, 12 + (current - 1) * blockSize, SEEK_SET);
				result = fread(buffer, 1, iSize, pFile);

				for (int i = 0; i < blockSize / 4 - 1 && buffer[i] != 0; i += 2) {
					if (buffer[i] <= _endRange)
						v.push_back(pair<int, int>(buffer[i], buffer[i + 1]));
				}
			}
		}
	}

	return v;
}

void BTree::print() {
	FILE* pFile;
	long iSize;
	int* buffer;
	size_t result;

	pFile = fopen(fileName, "rb");
	fseek(pFile, 0, SEEK_END);
	iSize = ftell(pFile);
	rewind(pFile);
	buffer = (int*)malloc(sizeof(char) * iSize);
	result = fread(buffer, 1, iSize, pFile);
	fclose(pFile);

	int blockSize = (int)buffer[0];
	int root = (int)buffer[1];
	int depth = (int)buffer[2];

	pFile = fopen(fileName, "rb");
	fseek(pFile, 0, SEEK_END);
	iSize = ftell(pFile);
	rewind(pFile);
	buffer = (int*)malloc(sizeof(char) * iSize);

	fseek(pFile, 12 + (root - 1) * blockSize, SEEK_SET);

	result = fread(buffer, 1, iSize, pFile);
	fclose(pFile);

	ofstream output;
	output.open(outputFileName, ios::out);

	output << "<0>\n";
	for (int i = 1; i < blockSize / 4; i += 2) {
		if (buffer[i] != 0)
			output << buffer[i];
		if (blockSize / 4 != i + 2 && buffer[i + 2] != 0)
			output << ", ";
	}
	output << "\n<1>\n";

	int* buffer2;
	buffer2 = (int*)malloc(sizeof(char) * iSize);
	for (int i = 0; i < blockSize / 4; i += 2) {
		if (buffer[i] != 0)
			fseek(pFile, 12 + (buffer[i] - 1) * blockSize, SEEK_SET);
		else
			break;

		result = fread(buffer2, 1, iSize, pFile);

		if (depth == 1) {
			for (int k = 0; k < blockSize / 4; k += 2) {
				if (buffer2[k] != 0)
					output << buffer2[k];
			}
		}
		else {
			for (int k = 0; k < blockSize / 4; k += 2) {
				if (buffer2[k] != 0)
					output << buffer2[k] << ", ";
			}
		}
	}
	output << "\n";
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
	ofstream writeText;
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
			
			btree->insert(1, stoi(input[0]), stoi(input[1]), 0);
		}
		break;
	case 's':
		openText.open(argv[3]);
		writeText.open(argv[4]);
		
		while (openText.peek() != EOF) {
			getline(openText, buffer);
			writeText << buffer << ", " << btree->search(stoi(buffer)) << "\n";
		}

		break;
	case 'r':
		openText.open(argv[3]);
		writeText.open(argv[4]);

		while (openText.peek() != EOF) {
			getline(openText, buffer);
			vector<string> input = split(buffer, ",");

			vector<pair<int, int>> v = btree->search(stoi(input[0]), stoi(input[1]));

			for (int i = 0; i < v.size(); i++)
				writeText << v[i].first << ", " << v[i].second << "\t";
			writeText << "\n";
		}
		break;
	case 'p':
		outputFileName = argv[3];
		btree->print();
		break;

	default:
		break;
	}

	return 0;
}