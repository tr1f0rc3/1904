#include <iostream>
#include <vector>
#include <string>
using namespace std;

vector<size_t> getPi(const string& pat) {
	size_t n = pat.length();
	vector<size_t> pi(n, 0);
	for (size_t i = 1, j = 0; i < n; ++i) {
		while (j > 0 && (pat[i] != pat[j])) {
			j = pi[j - 1];
		}
		if (pat[i] == pat[j]) {
			pi[i] = ++j;
		}
	}
	return pi;
}

vector<size_t> KMP(const string& text, const string& pat) {
	size_t textLen = text.length();
	size_t patLen = pat.length();
	vector<size_t> ans;
	if (textLen == 0 || patLen == 0 || textLen < patLen) {
		return ans;
	}
	else if (textLen == 1) {
		if (text == pat) {
			ans.emplace_back(0);
		}
		return ans;
	}

	vector<size_t> pi = getPi(pat);
	for (size_t i = 0, j = 0; i < textLen; ++i) {
		while (j > 0 && (text[i] != pat[j])) {
			j = pi[j - 1];
		}
		if (text[i] == pat[j]) {
			if (j == patLen - 1) {
				ans.emplace_back(i - patLen + 1);
				j = pi[j];
			}
			else {
				++j;
			}
		}
	}
	return ans;
}


 vector<size_t> RabinKarp(const string& text, const string& pat, int q = 101) {

	 size_t textLen = text.length();
	 size_t patLen = pat.length();
	 vector<size_t> ans;
	 if (textLen == 0 || patLen == 0 || textLen < patLen) {
		 return ans;
	 }

	 enum {
		 d = 256,
	 };
	 int h(1);
	 for (size_t i = 0; i < patLen - 1; ++i) {
		 h = (h * d) % q;
	 }
	 int p(0), t(0);
	 for (size_t i = 0; i < patLen; ++i) {
		 p = (p * d + pat[i]) % q;
		 t = (t * d + text[i]) % q;
	 }

	 for (size_t i = 0; i < textLen - patLen + 1; ++i) {
		 if (p == t) {
			 size_t j(0);
			 for (; j < patLen; ++j) {
				 if (text[i + j] != pat[j]) {
					 break;
				 }
			 }
			 if (j == patLen) {
				 ans.emplace_back(i);
			 }
		 }
		 if (i < textLen - patLen) {
			 t = (d * (t - h * text[i]) + text[i + patLen]) % q;
			 if (t < 0) {
				 t += q;
			 }
		 }
	 }
	 return ans;
 }

int main()
{
	string pat = "abaab";
	string text = "aababaabab";
	RabinKarp(text, pat);
	KMP(text, pat);
}