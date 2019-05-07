#include "pch.h"
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

struct Pos;
struct Velocity;
class Car;
class GasStation;

enum class edir { left, right, up, down, };

// Structures
struct Pos {

	Pos() = default;
	Pos(float _x, float _y) : x(_x), y(_y) {}
	//~Pos();

	Pos& operator+=(const Pos &rhs);

	float x = 0;
	float y = 0;
};

struct Velocity {

	Velocity() = default;
	Velocity(float _speed) : speed(_speed) {}
	//~Velocity();

	edir dir = edir::left;
	float speed = 0;
};

// Classes
class Car {

	enum { emaxDeposit, emaxSpeed = 400, emaxFuel = 1500, eknownFuelEfficiency = 7 };

public:

	Car() = default;
	Car(int _deposit, float _speed, float _fuel,
		pair<float, float> _pos = make_pair(0, 0), float _totalDistance = 0) :
		deposit(_deposit), vel(_speed), fuel(_fuel),
		pos(_pos.first, _pos.second), totalDistance(_totalDistance) {}
	//~Car();


	// interaction functions
	bool run(float t);
	bool pay(GasStation* _gasStation, int _price);
	bool buyFuel(GasStation* _gasStation, float _fuel);
	Pos getPos() { return pos; }
	Velocity getVelocity() { return vel; }
	int getDeposit() { return deposit; }
	float getTotalDistance() { return totalDistance; }

	//status
	float fuel = 0;
	Velocity vel;

private:

	// status
	float maxFuel = emaxFuel;
	int deposit = 0;
	int maxDeposit = emaxDeposit;
	Pos pos;
	float maxSpeed = emaxSpeed;
	float totalDistance = 0;
	float knownFuelEfficiency = eknownFuelEfficiency;

};

class GasStation {

	enum { eFuelPrice = 100, eMaxFuel = 100000, };

public:

	GasStation() = default;
	GasStation(int _deposit, float _fuel, pair<float, float> _pos = make_pair(0, 0)) :
		deposit(_deposit), fuel(_fuel), pos(_pos.first, _pos.second) {}
	//~GasStation();

	bool giveFuel(Car* _car, float fuel);

	// get
	Pos getPos() { return pos; }
	float getFuel() { return fuel; }
	int getFuelPrice() { return fuelPrice; }

	// status
	int deposit = 0;


private:

	Pos pos = { 0,0 };
	float fuel = 0;
	float MaxFuel = eMaxFuel;
	int fuelPrice = eFuelPrice;
};

// Pos functions
inline Pos& Pos::operator+=(const Pos &rhs) {

	x += rhs.x;
	y += rhs.y;
	return *this;
}

// Car functions
inline bool Car::run(float t = 1) {

	float moveDistance = vel.speed * t;
	float fuelConsumption = moveDistance * knownFuelEfficiency;
	if (fuel - fuelConsumption < 0) {
		float ConsumptionRatio = fuel / fuelConsumption;
		return run(ConsumptionRatio * t);
	}

	switch (vel.dir)
	{
	case edir::left:
		pos.x -= moveDistance;
		break;
	case edir::right:
		pos.x += moveDistance;
		break;
	case edir::up:
		pos.y += moveDistance;
		break;
	case edir::down:
		pos.y -= moveDistance;
		break;
	default:
		break;
	}

	fuel -= fuelConsumption;
	totalDistance += moveDistance;

	return true;
}


inline bool Car::pay(GasStation* _gasStation, int _price) {
	if (deposit - _price >= 0) {
		deposit -= _price;
		_gasStation->deposit += _price;
		return true;
	}
	return false;
}

inline bool Car::buyFuel(GasStation* _gasStation, float _fuel) {

	if (maxFuel - fuel - _fuel < 0) {
		int fuelFreeSpace = static_cast<int>(maxFuel - fuel);
		return buyFuel(_gasStation, static_cast<float>(fuelFreeSpace));
	}
	else {
		int totalPrice = static_cast<int>(fuel) * (_gasStation->getFuelPrice());
		if (!pay(_gasStation, totalPrice)) { return false; }
		if (!(_gasStation->giveFuel(this, _fuel))) { return false; }
	}
	return true;
}

class CarMap {
	enum { maxCar = 10, maxGasStation = 5, mapWidth = 30, mapHeight = 15 };

public:
	vector<Car*> cars;
	vector<GasStation*> gasStations;
	char graphic[mapHeight][mapWidth + 1] = { ' ', };
	char defualtMap[mapHeight][mapWidth + 1] = {
	 "##############################",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "#                            #",
	 "##############################" };


	void drawObjects() {
		memset(graphic, ' ', sizeof(graphic));
		for (int i = 0; i != mapHeight; ++i) {
			for (int j = 0; j != mapWidth; ++j) {
				graphic[i][j] = defualtMap[i][j];
			}
		}
		for (int i = 0; i != cars.size(); ++i) {
			Pos tempPos = cars[i]->getPos();
			graphic[static_cast<int>(tempPos.y)][static_cast<int>(tempPos.x)] = '@';
		}
		for (int i = 0; i != gasStations.size(); ++i) {
			Pos tempPos = gasStations[i]->getPos();
			graphic[static_cast<int>(tempPos.y)][static_cast<int>(tempPos.x)] = '%';
		}
		for (int i = 0; i != mapHeight; ++i) {
			for (int j = 0; j != mapWidth; ++j) {
				cout << graphic[i][j];
			}
			cout << endl;
		}
	}

	int numOfCars = 0;
	int numOfGasStation = 0;
};


// GasStation functions
inline bool GasStation::giveFuel(Car* _car, float _fuel) {
	if (fuel - _fuel >= 0) {
		fuel -= _fuel;
		_car->fuel += _fuel;
		return true;
	}
	return false;
}

int main() {

	enum { maxCar = 10, maxGasStation = 5, mapWidth = 30, mapHeight = 15 };

	srand(time(0));

	CarMap cmap;
	cmap.drawObjects();
	Car car1(50000, 1, 100, make_pair((rand() % (mapWidth - 2)) + 1, (rand() % (mapHeight - 2)) + 1));
	Car car2(50000, 2, 100, make_pair((rand() % (mapWidth - 2)) + 1, (rand() % (mapHeight - 2)) + 1));
	cmap.cars.push_back(&car1);
	cmap.cars.push_back(&car2);
	cmap.drawObjects();

	cout << "fuel: " << car2.fuel << endl;
	cout << "pos: " << car2.getPos().x << ' ' << car2.getPos().y << endl;
	car2.run(3);
	cmap.drawObjects();
	car2.vel.dir = edir::right;
	car2.run(3);
	cmap.drawObjects();
	cout << "fuel: " << car2.fuel << endl;
	cout << "pos: " << car2.getPos().x << ' ' << car2.getPos().y << endl;


	GasStation gs2(4000, 50000, make_pair((rand() % (mapWidth - 2)) + 1, (rand() % (mapHeight - 2)) + 1));
	cmap.gasStations.push_back(&gs2);
	cmap.drawObjects();
	cout << "car fuel: " << car2.fuel << ' ' << "gs fuel: " << gs2.getFuel() << endl;
	cout << "car deposit: " << car2.getDeposit() << ' ' << "gs deposit: " << gs2.deposit << endl;
	car2.buyFuel(&gs2, 10);
	cout << "car fuel: " << car2.fuel << ' ' << "gs fuel: " << gs2.getFuel() << endl;
	cout << "car deposit: " << car2.getDeposit() << ' ' << "gs deposit: " << gs2.deposit << endl;


	car2.buyFuel(&gs2, 2000);
	cout << "car fuel: " << car2.fuel << ' ' << "gs fuel: " << gs2.getFuel() << endl;
	cout << "car deposit: " << car2.getDeposit() << ' ' << "gs deposit: " << gs2.deposit << endl;



	return 0;

}