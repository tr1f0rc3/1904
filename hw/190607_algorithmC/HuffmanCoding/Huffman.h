#pragma once
#include <map>
#include "Heap.h"
#include "HNode.h"
template <typename T>
class Huffman
{
public:
	Huffman() = default;
	Huffman(std::vector<HNode> vec);

	std::vector<std::string> compress(const std::vector<std::string>& text);
	std::vector<std::string> extract(const std::vector<std::string>& ctext);

protected:
	void textToHeap(const std::vector<std::string>& text);

	Heap<HNode> heap;

};


template <typename T>
Huffman<T>::Huffman(std::vector<HNode> vec) : heap(vec) {};

template <typename T>
std::vector<std::string> Huffman<T>::compress(const std::vector<std::string>& text) {

	textToHeap(text);
	
	if (heap.getSize() > 1) {

		HNode *curr, *parent, *prev(nullptr);

		parent = new HNode('@', 0);
		parent->right = new HNode(heap.extractMin());
		parent->left = new HNode(heap.extractMin());
		parent->freq = parent->right->freq + parent->left->freq;

		while (heap.getSize() > 1) {
			curr = new HNode(heap.extractMin());
			parent = new HNode('@', curr->freq + prev->freq);
			if (curr->freq < prev->freq) {
				parent->left = prev;
				parent->right = curr;
			}
			else {
				parent->left = curr;
				parent->right = prev;
			}
			prev = curr;
		}
	}

	
	return std::vector<std::string>();


}

template <typename T>
void Huffman<T>::textToHeap(const std::vector<std::string>& text) {
	
	assert(text.size() >= 0);

	std::map<char, int> freqMap;
	for (auto& strElem : text) {
		for (auto charElem : strElem) {
			++freqMap[charElem];
		}
	}

	heap.clearAll();
	for (auto it = freqMap.begin(); it != freqMap.end(); ++it) {
		HNode tempNode(it->first, it->second);
		heap.insert(tempNode);
	}
}


template <typename T>
std::vector<std::string> Huffman<T>::extract(const std::vector<std::string>& ctext) {


	return std::vector<std::string>();

}



