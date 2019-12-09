#include <iomanip>
#include <iostream>
#include <cassert>
#include <vector>
#include <vld.h>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

template <typename T>
class Node {
public:
   T data;
   Node* getNext() const;
   Node* getPrev() const;

   Node<T>* next = nullptr;
   Node<T>* prev = nullptr;
};

template <typename T>
inline Node<T>* Node<T>::getNext() const {
   if (next == nullptr) {
      cout << "this is tail" << endl;
   }
   else {
      return next;
   }
   
}

template <typename T>
inline Node<T>* Node<T>::getPrev() const {
   if (prev == nullptr) {
      cout << "this is head" << endl;
   }
   else {
      return prev;
   }
   
}

template <typename T>
class linkedList {
public:
   void addNode(T);

   Node<T>* head = nullptr;
   Node<T>* tail = nullptr;
};

template <typename T>
inline void linkedList<T>::addNode(T _input) {
   Node<T>* temp = new Node<T>;
   temp->data = _input;

   if (head == nullptr) {
      head = temp;
      tail = temp;
   }
   else {
      temp->prev = tail;
      tail->next = temp;
      tail = temp;
   }
}


int main() {
   linkedList<int> LL;
   LL.addNode(5);
   LL.addNode('a');
   LL.addNode(7);
   LL.addNode(8);
   cout << LL.head->data << endl;
   cout << LL.head->getPrev()->data << endl;
   cout << LL.head->getNext()->data << endl;


}



