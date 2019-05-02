
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

template <typename T>
inline void atest(T a, T b) {
	bool c(0);
	if (a == b) {
		c = 1;
	}
	cout << c << endl;
}

class Animal {
public:

	enum class efeedingType { canivore, herbivore, ovum, };

	Animal() = default;
	Animal(int _numberPedal, efeedingType _feedingType) : numberPedal(_numberPedal), feedingType(_feedingType) {}
	virtual ~Animal() {}

	virtual void Action() const = 0;
	virtual void Info() const;

protected:
	int numberPedal = 0;
	efeedingType feedingType = efeedingType::ovum;
};

inline void Animal::Info() const {
	cout << "number of pedal: " << numberPedal << endl;
}

class Duck : public Animal {
	

public:

	Duck() : Animal(2, efeedingType::ovum) {}

	virtual void Action() const override;
	virtual void Info() const override;
};

inline void Duck::Action() const {
	cout << "quack!" << endl;
}
inline void Duck::Info() const {
	cout << "Animal:Duck" << endl;
	cout << "feeding type: " << "ovum" << endl;
}

class Lion : public Animal {

public:

	Lion() : Animal(4, efeedingType::canivore) {}

	virtual void Action() const override;
	virtual void Info() const override;
};

inline void Lion::Action() const {
	cout << "growl..." << endl;
}
inline void Lion::Info() const {
	cout << "Animal:Lion" << endl;
	cout << "feeding type: " << "canivore" << endl;
}

int main() {
	
	Duck d1;
	Lion l1;
	
	d1.Action();
	d1.Info();
	l1.Action();
	l1.Info();

	getchar();
	return 0;

}