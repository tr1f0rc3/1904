#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <iostream>
class ScoreManager

{

private:

	// 성적을 저장하는 여러 가지 멤버 변수들

	int ar[3][5][10][4];

public:

	ScoreManager() { memset(ar, 0, sizeof(ar)); }

	int &operator()(int Grade, int Class, int StNum, const char *Subj) {

		return ar[Grade][Class][StNum][0];

	}

	const int &operator()(int Grade, int Class, int StNum, const char *Subj) const {

		return ar[Grade][Class][StNum][0];

	}

};



int main()

{

	ScoreManager SM;



	printf("1학년 2반 3번 학생의 국어 성적 = %d\n", SM(1, 2, 3, "국어"));

	SM(2, 3, 4, "산수") = 99;

}