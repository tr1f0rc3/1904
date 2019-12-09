#pragma once
#include <iostream>

struct HNode {

	friend std::ostream& operator<<(std::ostream& out, const HNode& node);

	HNode() = default;
	HNode(char _data, int _freq) : data(_data), freq(_freq) {}
	//HNode(const HNode& hnode) : data(hnode.data), freq(hnode.freq) {}
	


	bool operator<(const HNode& rhs) const;
	bool operator>(const HNode& rhs) const;
	bool operator==(const HNode& rhs) const;
	bool operator<=(const HNode& rhs) const;
	bool operator>=(const HNode& rhs) const;

	HNode* operator->();
	
	char data = '$';
	int freq = 0;
	HNode* left = nullptr;
	HNode* right = nullptr;

};

std::ostream& operator<<(std::ostream& out, const HNode& node);
