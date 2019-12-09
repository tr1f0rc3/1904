#include "HNode.h"

bool HNode::operator<(const HNode& rhs) const {
	return freq < rhs.freq;
}
bool HNode::operator>(const HNode& rhs) const {
	return freq > rhs.freq;
}
bool HNode::operator==(const HNode& rhs) const {
	return freq == rhs.freq;
}
bool HNode::operator<=(const HNode& rhs) const {
	return freq <= rhs.freq;
}
bool HNode::operator>=(const HNode& rhs) const {
	return freq >= rhs.freq;
}

HNode* HNode::operator->() {
	return this;
}

std::ostream& operator<<(std::ostream& out, const HNode& node) {
	out << node.data << ": " << node.freq;
	return out;
}
