#include <iostream>
#include <stdio.h>
#include <string.h>


#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE


using namespace std;



class Str

{

	friend ostream &operator <<(ostream &c, const Str &S);

	friend const Str operator +(const char *ptr, Str &s);

	friend bool operator ==(const char *ptr, Str &s);

	friend bool operator !=(const char *ptr, Str &s);

	friend bool operator >(const char *ptr, Str &s);

	friend bool operator <(const char *ptr, Str &s);

	friend bool operator >=(const char *ptr, Str &s);

	friend bool operator <=(const char *ptr, Str &s);

protected:

	char *buf;

	int size;



public:

	Str();

	Str(const char *ptr);

	Str(const Str &Other);

	explicit Str(int num);

	virtual ~Str();



	int length() const { return strlen(buf); }

	Str &operator =(const Str &Other);

	Str &operator +=(Str &Other);

	Str &operator +=(const char *ptr);

	char &operator [](int idx) { return buf[idx]; }

	const char &operator [](int idx) const { return buf[idx]; }

	operator const char *() { return (const char *)buf; }

	operator int() { return atoi(buf); }

	const Str operator +(Str &Other) const;

	const Str operator +(const char *ptr) const { return *this + Str(ptr); }

	bool operator ==(Str &Other) { return strcmp(buf, Other.buf) == 0; }

	bool operator ==(const char *ptr) { return strcmp(buf, ptr) == 0; }

	bool operator !=(Str &Other) { return strcmp(buf, Other.buf) != 0; }

	bool operator !=(const char *ptr) { return strcmp(buf, ptr) != 0; }

	bool operator >(Str &Other) { return strcmp(buf, Other.buf) > 0; }

	bool operator >(const char *ptr) { return strcmp(buf, ptr) > 0; }

	bool operator <(Str &Other) { return strcmp(buf, Other.buf) < 0; }

	bool operator <(const char *ptr) { return strcmp(buf, ptr) < 0; }

	bool operator >=(Str &Other) { return strcmp(buf, Other.buf) >= 0; }

	bool operator >=(const char *ptr) { return strcmp(buf, ptr) >= 0; }

	bool operator <=(Str &Other) { return strcmp(buf, Other.buf) <= 0; }

	bool operator <=(const char *ptr) { return strcmp(buf, ptr) <= 0; }

	void Format(const char *fmt, ...);

};



// 디폴트 생성자

Str::Str()

{

	size = 1;

	buf = new char[size];

	buf[0] = 0;

}



// 문자열로부터 생성하기

Str::Str(const char *ptr)

{

	size = strlen(ptr) + 1;

	buf = new char[size];

	strcpy(buf, ptr);

}



// 복사 생성자

Str::Str(const Str &Other)

{

	size = Other.length() + 1;

	buf = new char[size];

	strcpy(buf, Other.buf);

}



// 정수형 변환 생성자

Str::Str(int num)

{

	char temp[128];



	itoa(num, temp, 10);

	size = strlen(temp) + 1;

	buf = new char[size];

	strcpy(buf, temp);

}



// 파괴자

Str::~Str()

{

	delete[] buf;

}



// 대입 연산자

Str &Str::operator =(const Str &Other)

{

	if (this != &Other) {

		size = Other.length() + 1;

		delete[] buf;

		buf = new char[size];

		strcpy(buf, Other.buf);

	}

	return *this;

}



// 복합 연결 연산자

Str &Str::operator +=(Str &Other)

{

	char *old;

	old = buf;

	size += Other.length();

	buf = new char[size];

	strcpy(buf, old);

	strcat(buf, Other.buf);

	delete[] old;

	return *this;

}



Str &Str::operator +=(const char *ptr)

{

	return *this += Str(ptr);

}



// 연결 연산자

const Str Str::operator +(Str &Other) const

{

	Str T;



	delete[] T.buf;

	T.size = length() + Other.length() + 1;

	T.buf = new char[T.size];

	strcpy(T.buf, buf);

	strcat(T.buf, (const char *)Other);

	return T;

}



// 출력 연산자

ostream &operator <<(ostream &c, const Str &S)

{

	c << S.buf;

	return c;

}



// 더하기 및 관계 연산자

const Str operator +(const char *ptr, Str &s) { return Str(ptr) + s; }

bool operator ==(const char *ptr, Str &s) { return strcmp(ptr, s.buf) == 0; }

bool operator !=(const char *ptr, Str &s) { return strcmp(ptr, s.buf) != 0; }

bool operator >(const char *ptr, Str &s) { return strcmp(ptr, s.buf) > 0; }

bool operator <(const char *ptr, Str &s) { return strcmp(ptr, s.buf) < 0; }

bool operator >=(const char *ptr, Str &s) { return strcmp(ptr, s.buf) >= 0; }

bool operator <=(const char *ptr, Str &s) { return strcmp(ptr, s.buf) <= 0; }



// 서식 조립 함수

void Str::Format(const char *fmt, ...)

{

	char temp[1024];

	va_list marker;



	va_start(marker, fmt);

	vsprintf(temp, fmt, marker);

	*this = Str(temp);

}



void main()

{

	Str s = "125";

	int k;

	k = (int)s + 123;



	Str s1("문자열");      // 문자열로 생성자

	Str s2(s1);                   // 복사 생성자

	Str s3;                    // 디폴트 생성자

	s3 = s1;                    // 대입 연산자



	// 출력 연산자

	cout << "s1=" << s1 << ",s2=" << s2 << ",s3=" << s3 << endl;

	cout << "길이=" << s1 << endl;



	// 정수형 변환 생성자와 변환 연산자

	Str s4(1234);

	cout << "s4=" << s4 << endl;

	int num = int(s4) + 1;

	cout << "num=" << num << endl;



	// 문자열 연결 테스트

	Str s5 = "First";

	Str s6 = "Second";

	cout << s5 + s6 << endl;

	cout << s6 + "Third" << endl;

	cout << "Zero" + s5 << endl;

	cout << "s1은 " + s1 + "이고 s5는 " + s5 + "이다." << endl;

	s5 += s6;

	cout << "s5와 s6을 연결하면 " << s5 << "이다." << endl;

	s5 += "Concatination";

	cout << "s5에 문자열을 덧붙이면 " << s5 << "이다." << endl;



	// 비교 연산자 테스트

	if (s1 == s2) {

		cout << "두 문자열은 같다." << endl;

	}
	else {

		cout << "두 문자열은 다르다." << endl;

	}



	// char *형과의 연산 테스트

	Str s7;

	s7 = "상수 문자열";

	cout << s7 << endl;

	char str[128];

	strcpy(str, s7);

	cout << str << endl;



	// 첨자 연산자 테스트

	Str s8("Index");

	cout << "s8[2]=" << s8[2] << endl;

	s8[2] = 'k';

	cout << "s8[2]=" << s8[2] << endl;



	// 서식 조립 테스트

	Str sf;

	int i = 9876;

	double d = 1.234567;

	sf.Format("서식 조립 가능하다. 정수=%d, 실수=%.2f", i, d);

	cout << sf << endl;

}