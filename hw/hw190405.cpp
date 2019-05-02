#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <vector>
#include <array>
#include <string>
using namespace std;

int Fib(int i, map<int, int> dynamicFib) {

	int term_iminusone, term_iminustwo;

	auto findFib = dynamicFib.find(i - 1);
	if (findFib != dynamicFib.end()) {
		term_iminusone = findFib->second;
		term_iminustwo = (--findFib)->second;
	}
	else {
		term_iminusone = Fib(i - 1, dynamicFib);
		term_iminusone = Fib(i - 2, dynamicFib);
	}

	return term_iminusone + term_iminustwo;
}

int main() {
	{
		int n = 9;
		int maxSetWidth;
		if (n % 2 == 0) {
			maxSetWidth = n + 1;
		}
		else {
			maxSetWidth = n;
		}
		cout.fill(' ');
		for (int i = 0; i < maxSetWidth / 2; ++i) {

			cout.width(maxSetWidth / 2 + 1 + i);
			string stars = string(1 + 2 * i, '*');
			cout << right << stars << endl;

		}
		for (int i = maxSetWidth / 2; i >= 0; --i) {

			cout.width(maxSetWidth / 2 + 1 + i);
			string stars = string(1 + 2 * i, '*');
			cout << right << stars << endl;

		}
	}

	{
		int n = 100;

		set<int> primes;
		set<int> composite;
		int k(0);
		for (int i = 2; i <= n; ++i) {
			auto findComposite = find(composite.begin(), composite.end(), i);
			if (findComposite != composite.end()) {
				continue;
			}
			else {
				primes.insert(i);
				k = 0;
				for (int j = 2; k <= 100; ++j) {
					k = i * j;
					composite.insert(k);
				}
			}
		}
		for (auto& elem : primes) {
			cout << elem << ' ';
		}
		cout << endl;
	}

	{
		const int n = 10;

		array<int, n+1> dynamicFib{ 1, 1 };
		for (int i = 2; i <= n; ++i) {
			dynamicFib[i] = dynamicFib[i - 2] + dynamicFib[i - 1];
		}
		int i(0);
		for (auto& elem : dynamicFib) {
			cout << "fib(" << i++ << ")=" << elem << ' ';
		}
		cout << endl;
	}

	{
		int n = 10;

		map<int, int> dynamicFib;
		dynamicFib.insert({ 0,1 });
		dynamicFib.insert({ 1,1 });
		for (int i = 2; i <= n; ++i) {
			int calculatedFib = Fib(i, dynamicFib);
			dynamicFib.insert({ i,calculatedFib });
		}
		for (auto it = dynamicFib.begin(); it != dynamicFib.end(); ++it) {
			cout << "fib(" << it->first << ")=" << it->second << ' ';
		}
		cout << endl;
	}
	getchar();
}