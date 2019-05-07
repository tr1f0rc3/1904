// ConsoleApplication2.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;
struct SuperSum {

	SuperSum() = default;
	int ssd(vector<int> _stairVec, int cnt) {

		int sum(0);

		auto it = dic.find({ _stairVec, cnt });
		if (it != dic.end()) {
			sum = it->second;
			return sum;
		}

		size_t sLength = _stairVec.size();

		if (sLength > 1) {
			vector<int> vec1 = _stairVec;
			vec1.pop_back();
			vector<int> vec2 = vec1;
			vec2.pop_back();
			int value1, value2;
			value1 = ssd(vec2, 0);
			if (cnt < 1) {
				value2 = ssd(vec1, cnt + 1);
			}
			sum = _stairVec[sLength - 1] + max(value1, value2);
		}
		else if (sLength == 1) { sum = _stairVec[sLength - 1]; }
		else { sum = 0; }
		dic.insert({ {_stairVec,cnt}, sum });
		return sum;
	}

	map<pair<vector<int>, int>, int> dic;
};

int main()
{
	int k, n, i(0);
	SuperSum ss;
	vector<int> vec;
	cin >> n;
	for (int i = 0; i < n; ++i) {
		cin >> k;
		vec.push_back(k);
	}
	int value = ss.ssd(vec, 0);
	cout << value << endl;


}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
