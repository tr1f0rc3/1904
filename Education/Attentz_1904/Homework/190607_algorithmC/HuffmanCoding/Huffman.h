#pragma once
#include <map>
#include <bitset>
#include <stack>
#include "Heap.h"
#include "HNode.h"
#include "FileLoader.h"

template <typename T>
class Huffman
{
public:
	Huffman() : Huffman(std::vector<HNode>()) {};
	Huffman(std::vector<HNode> vec);
	~Huffman();

	void compressText(const std::string& inPath = "song.txt", const std::string& outPath = "csong.txt");
	void extractText(const std::string& inPath = "csong.txt", const std::string& outPath = "esong.txt");
	FileLoader fileLoader;

protected:
	bool textToHeap(const std::vector<std::string>& text);
	HNode* minHeapToHuffTree();
	void huffTreeToTable();
	void huffTreeProbe(HNode* node, std::string = "");
	void huffTreeDeleter(HNode* node);


	Heap<HNode> minHeap;
	HNode* huffTree = nullptr;
	std::map<char, std::string> table;


};




template <typename T>
Huffman<T>::Huffman(std::vector<HNode> vec) : minHeap(false, vec) {
};

template <typename T>
Huffman<T>::~Huffman() {
	if (huffTree != nullptr) {
		huffTreeDeleter(huffTree);
	}
}


template <typename T>
void Huffman<T>::compressText(const std::string& inPath, const std::string& outPath) {

	const auto& inText = fileLoader.loadTxt(inPath);
	std::cout << textToHeap(inText);
	huffTree = minHeapToHuffTree();
	huffTreeToTable();

	std::vector<std::string> outText;
	std::string tempStr("$$");

	for (auto itr = table.begin(); itr != table.end(); ++itr) {
		tempStr += '$';
		tempStr += itr->first;
		tempStr += '$';
		tempStr += itr->second;
	}
	tempStr += '$$';
	outText.emplace_back(tempStr);

	for (const auto& elem : inText) {
		tempStr = "";
		for (const char _char : elem) {
			tempStr += table.find(_char)->second;
		}
		outText.emplace_back(tempStr);
	}
	
	std::cout << fileLoader.saveTxt(outText, outPath);


}

template <typename T>
void Huffman<T>::huffTreeProbe(HNode* node, std::string str) {

	if (node->left != nullptr) {
		huffTreeProbe(node->left, str + '0');
	}
	if (node->right != nullptr) {
		huffTreeProbe(node->right, str + '1');
	}
	if (node->data != '$') {
		table.insert(std::make_pair(node->data, str));
	}
}

template <typename T>
void Huffman<T>::huffTreeDeleter(HNode* node) {

	if (node->left != nullptr) {
		huffTreeDeleter(node->left);
	}
	if (node->right != nullptr) {
		huffTreeDeleter(node->right);
	}
	delete node;
}



template <typename T>
void Huffman<T>::huffTreeToTable() {
	assert(minHeap.getSize() == 0);
	assert(huffTree != nullptr);

	table.clear();
	huffTreeProbe(huffTree);
}



template <typename T>
HNode* Huffman<T>::minHeapToHuffTree() {

	assert(minHeap.getSize() >= 0);
	int parentPos;

	while (minHeap.getSize() > 1) {
		HNode* left = new HNode(minHeap.extract());
		HNode* right = new HNode(minHeap.extract());
		HNode parent('$', left->freq + right->freq);
		parent->left = left;
		parent->right = right;
		minHeap.insert(parent);
	}
	return new HNode(minHeap.extract());
}


template <typename T>
bool Huffman<T>::textToHeap(const std::vector<std::string>& text) {

	assert(text.size() >= 0);

	std::map<char, int> freqMap;
	for (auto& strElem : text) {
		for (auto charElem : strElem) {
			++freqMap[charElem];
		}
	}

	minHeap.clearAll();
	for (auto it = freqMap.begin(); it != freqMap.end(); ++it) {
		HNode tempNode(it->first, it->second);
		minHeap.insert(tempNode);
	}
	return minHeap.isSorted();
}


template <typename T>
void Huffman<T>::extractText(const std::string& inPath, const std::string& outPath) {



}



