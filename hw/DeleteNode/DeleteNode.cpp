
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
using namespace std;

template<typename T>
struct Tree {
	Tree(const T& v) : value(v), left(nullptr), right(nullptr) {}
	T value;
	Tree* left;
	Tree* right;
};

struct SearchResult {
	SearchResult() = default;
	SearchResult(Tree<int>* _parent, Tree<int>* _child, Tree<int>* _root, bool _isRight) :
		parent(_parent), child(_child), root(_root), isRight(_isRight) {};
	Tree<int>* parent = nullptr;
	Tree<int>* child = nullptr;
	Tree<int>* root = nullptr;
	bool isRight = 0;
};

SearchResult findNode(Tree<int>* t, int val) {
	SearchResult ans;
	ans.root = t;
	if (t == nullptr) {
		return ans;
	}
	else if (t->value == val) {
		ans.child = t;
		return ans;
	}

	Tree<int>* prev(nullptr), * front(t);
	while (front != nullptr) {
		if (front->value == val) {
			ans.parent = prev;
			ans.child = front;
			return ans;
		}
		else {
			prev = front;
			if (val < front->value) {
				ans.isRight = 0;
				front = front->left;
			}
			else {
				ans.isRight = 1;
				front = front->right;
			}
		}
	}
	return ans;
}

SearchResult findPrev(Tree<int>* root) {
	SearchResult ans;
	ans.root = root;
	if (root == nullptr) {}
	else if (root->left == nullptr) {
		ans.child = root;
	}
	else {
		Tree<int>* prev(root), * front(root->left);
		while (front->right != nullptr) {
			prev = front;
			front = front->right;
			ans.isRight = 1;
		}
		ans.parent = prev;
		ans.child = front;
	}
	return ans;
}

bool deleteNode(SearchResult* result) {
	Tree<int>* parent = result->parent;
	Tree<int>* child = result->child;
	Tree<int>* root = result->root;
	bool isRight = result->isRight;

	if (child == nullptr) {
		return false;
	}
	else if (child == root) {
		if (child->left == nullptr && child->right == nullptr) {
			result->root = nullptr;
			return true;
		}
		else if (child->right == nullptr) {
			if (child->left != nullptr) {
				child->value = child->left->value;
			}
			child->left = child->left->left;
			child->right = child->left->right;
			return true;
		}
		else if (child->left == nullptr) {
			if (child->right != nullptr) {
				child->value = child->right->value;
			}
			child->left = child->right->left;
			child->right = child->right->right;
			return true;
		}
		else {
			Tree<int> dummyParent(-1);
			parent = &dummyParent;
			parent->left = child;
			parent->right = nullptr;
			isRight = 0;
		}
	}
	else if (parent == nullptr) {
		return false;
	}

	if (child->left == nullptr && child->right == nullptr) {
		if (isRight) {
			parent->right = nullptr;
		}
		else {
			parent->left = nullptr;
		}
	}
	else if (child->left == nullptr || child->right == nullptr) {
		if (isRight) {
			if (child->left != nullptr) {
				parent->right = child->left;
			}
			if (child->right != nullptr) {
				parent->right = child->right;
			}
		}
		else {
			if (child->left != nullptr) {
				parent->left = child->left;
			}
			if (child->right != nullptr) {
				parent->left = child->right;
			}
		}
	}
	else
	{
		SearchResult findPrevResult = findPrev(child);
		if (findPrevResult.root == findPrevResult.child) {
			findPrevResult.parent = parent;
			findPrevResult.isRight = isRight;
		}
		child->value = (findPrevResult.child)->value;
		return deleteNode(&findPrevResult);
	}
	return true;
}

Tree<int>* deleteFromBST(Tree<int>* t, std::vector<int> queries) {
    if (t == nullptr) {
   	 return t;
    }
    Tree<int>* root(t);
    SearchResult findNodeResult;
    for (int val : queries) {
   	 findNodeResult = findNode(root, val);
   	 if (findNodeResult.child != nullptr) {
   		 bool isSucceeded = deleteNode(&findNodeResult);
   		 assert(isSucceeded);
   		 if (findNodeResult.root == nullptr) {
   			 return nullptr;
   		 }
   	 }
    }
    return findNodeResult.root;
}

int main()
{
	{
		Tree<int> t31(5);
		Tree<int> t32(2);
		Tree<int> t33(6);
		Tree<int> t34(1);
		Tree<int> t35(3);
		Tree<int> t36(8);
		Tree<int> t37(7);

		t31.left = &t32;
		t31.right = &t33;
		t32.left = &t34;
		t32.right = &t35;
		t33.right = &t36;
		t36.left = &t37;

		vector<int>	queries = { 4,5,6 };
		deleteFromBST(&t31, queries);
	}

	{
		Tree<int> t41(3);
		Tree<int> t42(2);
		Tree<int> t43(5);
		Tree<int> t44(1);

		t41.left = &t42;
		t41.right = &t43;
		t42.left = &t44;

		vector<int> queries = { 3,2,1 };
		deleteFromBST(&t41, queries);
	}

	{
		Tree<int> t51(3);
		Tree<int> t52(2);
		Tree<int> t53(5);

		t51.left = &t52;
		t51.right = &t53;

		vector<int> queries = { 1,2,3,5 };
		deleteFromBST(&t51, queries);

	}
}
