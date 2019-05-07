// ConsoleApplication8.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"

#include "isPrime.h"
#include "VM.h"

#include "Market.h"

int MCN(int lhs, int rhs) {
	assert(lhs != 0 && rhs != 0);
	if (rhs > lhs) {
		swap(lhs, rhs);
	}
	int mcn = 1;
	if (lhs % rhs != 0) {
		mcn = MCN(lhs - rhs, rhs);
	}
	else { mcn = rhs; }
	return mcn;
}

int LMN(int lhs, int rhs) {
	assert(lhs != 0 && rhs != 0);
	int mcn = MCN(lhs, rhs);
	return lhs * rhs / mcn;
}

void game369(int _input) {
	assert(_input > 0);
	int i(1);
	while (i <= _input) {
		int isClap = 0;
		string toTest = to_string(i);
		for (char elem : toTest) {
			if (elem == '3' || elem == '6' || elem == '9') {
				cout << "cl ";
				isClap = 1;
				break;
			}
		}
		if (isClap == 0) { cout << i << ' '; }
		++i;
	}
}


int main()
{
	// prob 1
	int mcn1 = MCN(12, 8);
	int mcn2 = MCN(7, 5);
	// prob 2
	int lhs1 = LMN(12, 8);
	int lhs2 = LMN(7, 5);
	// prob 3
	for (int i = 3; i != 35; ++i) {
		cout << i << ": " << static_cast<int>(isPrime()(i)) << ", ";
	}
	cout << endl;
	// prob 4
	game369(34);

	// prob 5
	Market mkt1;
	Buyer buy1(100000);
	Seller sel1({ { 1,30000 }, {2, 25000} }, { { 1, 5 }, { 2, 7 } });
	sel1.Sell(&(buy1.discount), 2, 2);
	mkt1.addSeller(&sel1);
	mkt1.findSeller(&sel1);
	mkt1.removeSeller(&sel1);



	// prob 6
	VM vm1;
	auto change1 = vm1.buy(1, 82380);

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
