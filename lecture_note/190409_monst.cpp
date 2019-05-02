#include <iostream>
#include <cassert>
#include <vld.h>

#define SAFE_DELETE(x)      { if((x)!= nullptr) { delete (x); (x) = nullptr; } }
#define SAFE_DELETE_ARR(x)  { if((x)!= nullptr) { delete[] (x); (x) = nullptr; } }
#define EXPECT_EQ(x,y)      { assert((x)==(y) && "not same"); }
using namespace std;


class Monster {

public:
	Monster() = default;
	Monster(int _hp, int _maxHp, int _def, int _atk) : hp(_hp), maxHp(_maxHp), def(_def), atk(_atk) {}

	bool damage(int a_nDamage);
	virtual void attack() const { cout << "atk: " << atk << endl; }
	void info();

protected:
	int hp = 1;
	int maxHp = 1;
	int def = 0;
	int atk = 0;
};

class FireMonster : public Monster {
public:
	FireMonster() = default;
	FireMonster(int _hp, int _maxHp, int _def, int _atk, int _fireAtk) : Monster(_hp, _maxHp, _def, _atk), fireAtk(_fireAtk) {}

	virtual void attack() const override { cout << "atk: " << atk << ", fireAtk: " << fireAtk << endl; }

protected:
	int fireAtk = 0;
};

bool Monster::damage(int a_nDamage) {

	if (a_nDamage >= hp + def) {
		hp = 0;
		def = 0;
		cout << "YOU DIED" << endl;
		return true;
	}
	else {
		if (a_nDamage <= def)
			def -= a_nDamage;
		else {
			hp -= (a_nDamage - def);
			def = 0;
		}
		return false;
	}
}

inline void Monster::info() const {
	cout << "hp, maxHp, def atk: " << hp
		<< ", " << maxHp << ", " << def << ", " << atk << endl;
}

int main() {
	Monster m1(50, 100, 40, 40);
	m1.info();
	m1.damage(30);
	m1.info();
	m1.damage(30);
	m1.info();

	Monster m1(50, 100, 40, 40);


}