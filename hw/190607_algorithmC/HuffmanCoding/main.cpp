// HuffmanCoding.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "Heap.h"
#include "Huffman.h"
#include "FileLoader.h"
int main()
{
	using namespace std;
	{
		FileLoader fl;
		auto text1 = fl();
		Huffman<int> hm;
		hm.compress(text1);
	}
	{
		Heap<int> heap;
		std::vector<int> vec1 = { 5, 3, 2, 19, 8, 30, 1, 70, 3, 4, 6, 80, 12, 11, 5, 4 };
		for (auto elem : vec1) {
			heap.insert(elem);
			heap.printAll();
			cout << heap.isSorted() << endl;
		}
		for (size_t i = 0; i < vec1.size(); ++i) {
			heap.extractMin();
			heap.printAll();
			cout << heap.isSorted() << endl;
		}
	}


    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
