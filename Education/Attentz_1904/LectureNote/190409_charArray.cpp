#include <iostream>
#include <cassert>
#include <vld.h>

#define SAFE_DELETE(x)      { if((x)!= nullptr) { delete (x); (x) = nullptr; } }
#define SAFE_DELETE_ARR(x)  { if((x)!= nullptr) { delete[] (x); (x) = nullptr; } }

#define EXPECT_EQ(x,y)      { assert((x)==(y) && "not same"); }


class MyString
{
public:

	MyString(const char* a_pS)
	{
		if (a_pS == nullptr) { assert(false && "arg error"); }

		size_t nLen = strlen(a_pS);
		m_pArr = new char[nLen + 1];

		strcpy_s(m_pArr, nLen + 1, a_pS);
		m_nLength = nLen;
	}

	MyString(const MyString& a_c) : MyString(a_c.GetString()) { }

	~MyString()
	{
		SAFE_DELETE_ARR(m_pArr);
	}

	int SubStr(const char* a_pS) const
	{
		if (a_pS == nullptr) { return -1; }
		if (m_nLength == 0) { return -1; }

		const char* p = m_pArr;
		int nIndex = 0;
		int nLen = strlen(a_pS);

		while (*p != 0)
		{
			for (int i = 0; i < nLen; ++i)
			{
				if (p[i] == 0) { break; }

				if (p[i] != a_pS[i])
				{
					break;
				}

				if (i == nLen - 1)
				{
					return nIndex;
				}
			}

			++nIndex;
			++p;
		}

		return nIndex;
	}

	int SubStr(char a_c) const
	{
		if (m_nLength == 0) { return -1; }

		for (size_t i = 0; i < m_nLength; ++i)
		{
			if (m_pArr[i] == a_c) { return i; }
		}

		return -1;
	}

	const char* GetString() const { return m_pArr; }
	size_t      GetLength() const { return m_nLength; }

private:

	char*   m_pArr = nullptr;
	size_t  m_nLength = 0;
};

int main(int n, char* s[])
{
	using namespace std;

	MyString a("abcde");
	MyString b(a);

	cout << a.GetString() << endl;
	cout << b.GetString() << endl;

	EXPECT_EQ(a.SubStr(""), b.SubStr(""));
	EXPECT_EQ(a.SubStr("a"), b.SubStr("a"));
	EXPECT_EQ(a.SubStr("b"), b.SubStr("b"));
	EXPECT_EQ(a.SubStr("c"), b.SubStr("c"));
	EXPECT_EQ(a.SubStr("d"), b.SubStr("d"));
	EXPECT_EQ(a.SubStr("e"), b.SubStr("e"));

	EXPECT_EQ(a.SubStr("ab"), b.SubStr("ab"));
	EXPECT_EQ(a.SubStr("ac"), b.SubStr("ac"));
	EXPECT_EQ(a.SubStr("abc"), b.SubStr("abc"));
	EXPECT_EQ(a.SubStr("abcd"), b.SubStr("abcd"));
	EXPECT_EQ(a.SubStr("bcd"), b.SubStr("bcd"));
	EXPECT_EQ(a.SubStr("def"), b.SubStr("def"));
	EXPECT_EQ(a.SubStr("de"), b.SubStr("de"));
	EXPECT_EQ(a.SubStr("abcde"), b.SubStr("abcde"));

	const char* p = "abcde";
	for (int i = 0; i < 5; ++i)
	{
		EXPECT_EQ(a.SubStr(p), i);
		++p;
	}

	EXPECT_EQ(a.GetLength(), 5);
	EXPECT_EQ(b.GetLength(), 5);

	EXPECT_EQ(b.GetString() != a.GetString(), true);

	getchar();
	return 0;
}

