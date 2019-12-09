#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <array>

using namespace std;

class mystr {

public:
	mystr() = default;
	mystr(char* _m_ps);
	mystr(string _m_ps);
	mystr(const mystr& _mystr) : mystr(_mystr.m_ps) {}
	mystr(mystr&& _mystr) noexcept;

	~mystr() { delete[] m_ps; m_ps = nullptr;}

	mystr& operator=(mystr&);
	mystr& operator=(mystr&&);
	mystr operator+(const char*);
	mystr operator+(const string&);
	mystr& operator+=(const char*);
	mystr& operator+=(const string&);
	int operator()(int, int);
	
	vector<char> stringtoVec(const string& _m_ps) const ;
	mystr& replaceDynamicBuffer(char*);

	int size() const;
	void clear();
	bool empty() const;
	const char * c_str() const;

protected:
	char* m_ps = nullptr;
	size_t m_nsize = 0;
};

mystr::mystr(char* _m_ps) {
	if (_m_ps == nullptr) {
		m_ps = nullptr;
		m_nsize = 0;
	}
	else {
		size_t len = strlen(_m_ps);
		char* buffer = new char[len + 1];
		strcpy_s(buffer, len + 1, _m_ps);
		buffer[len] = 0;
		m_ps = buffer;
		m_nsize = len;
	}
}

inline mystr::mystr(string _m_ps) {
	vector<char> vec = stringtoVec(_m_ps);
	char* ptr = &vec[0];
	size_t len = strlen(ptr);
	char* buffer = new char[len + 1];
	strcpy_s(buffer, len + 1, ptr);
	buffer[len] = 0;
	m_ps = buffer;
	m_nsize = len;
}

inline mystr::mystr(mystr&& _mystr) noexcept : m_ps(_mystr.m_ps), m_nsize(_mystr.m_nsize)  {
	_mystr.m_ps = nullptr;
}

inline vector<char> mystr::stringtoVec(const string& _m_ps) const {
	std::vector<char> writable(_m_ps.begin(), _m_ps.end());
	writable.push_back('\0');
	return writable;
}

inline mystr& mystr::replaceDynamicBuffer(char* _m_ps) {
	if (_m_ps == nullptr) { delete m_ps; }
	m_ps = _m_ps;
	m_nsize = strlen(_m_ps);
	return *this;
}

inline mystr& mystr::operator=(mystr& _mystr) {
	mystr temp(_mystr);
	return temp;
}

inline mystr& mystr::operator=(mystr&& _mystr) {
	m_ps = _mystr.m_ps;
	m_nsize = _mystr.m_nsize;
	_mystr.m_ps = nullptr;
	return *this;
}

mystr mystr::operator+(const char* _m_ps) {
	if (_m_ps == nullptr) {
		return *this;
	}
	else {
		size_t len = strlen(_m_ps);
		size_t newLen = m_nsize + len + 1;
		char* buffer = new char[newLen];
		strcpy_s(buffer, m_nsize + 1, m_ps);
		strcpy_s(buffer + m_nsize, len + 1, _m_ps);
		mystr tempMystr;
		tempMystr.replaceDynamicBuffer(buffer);
		return tempMystr;
	}
}

inline mystr mystr::operator+(const string& _m_ps) {
	vector<char> vec = stringtoVec(_m_ps);
	char* ptr = &vec[0];
	return *this + ptr;
}

inline mystr& mystr::operator+=(const char* _m_ps) {
	*this = *this + _m_ps;
	return *this;
}

inline mystr& mystr::operator+=(const string& _m_ps) {
	*this = *this + _m_ps;
	return *this;
}

inline int mystr::operator()(int a, int b) {
	return a + b;
}

inline int mystr::size() const {
	return m_nsize;
};

inline void mystr::clear() {
	delete[] m_ps;
	m_ps = nullptr;
	m_nsize = 0;
}

inline bool mystr::empty() const {
	return (m_ps == nullptr);
}

inline const char * mystr::c_str() const {
	return m_ps;
}

mystr createMystr(const char* input) {
	return mystr(input);	
}

int main() {
	
	mystr mystrDefault;
	mystr mystrNull(nullptr);
	const char* abc("abc");
	mystr mystrChar(abc);
	mystr mystrString(string("cde"));

	mystr mystrPlusChar = mystrChar + "bcd";
	mystr mystrPlusString = mystrString + string("def");
	mystr mystrPlusNull = mystrChar + nullptr;

	mystr mystrPlusAssignmentChar = mystrChar;
	mystr mystrPlusAssignmentString = mystrString;
	mystr mystrPlusAssignmentNull = mystrNull;

	mystr* ptrMystrChar = new mystr(mystrChar);
	delete ptrMystrChar;

	mystr mystrSwap1("use =operator1");
	mystr mystrSwap2("use =operator2");
	mystrSwap1 = mystrSwap2;
	mystr mystrSwap3("use =operator3");
	mystrSwap3 = createMystr("use =operator4");

	mystrPlusAssignmentChar += "123";
	mystrPlusAssignmentString += string("456");
	mystrPlusAssignmentNull += nullptr;

	mystr mystrChartoMove(abc);
	mystr mystrCstr(mystrChartoMove);
	mystr mystrMoveCstr(createMystr("temp"));

	int mystrIntSum1 = mystr()(2, 3);
	int mystrIntSum2 = mystrDefault(4, 5);

	getchar();
}