#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <iostream>
class ScoreManager

{

private:

	// ������ �����ϴ� ���� ���� ��� ������

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



	printf("1�г� 2�� 3�� �л��� ���� ���� = %d\n", SM(1, 2, 3, "����"));

	SM(2, 3, 4, "���") = 99;

}