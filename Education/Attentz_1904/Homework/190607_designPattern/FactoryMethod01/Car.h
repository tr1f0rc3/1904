#pragma once

class Engine {

};

class GoldEngine : public Engine {

};

class SilverEngine : public Engine {

};

class Wheel {

};

class GoldWheel : public Wheel {

};

class SilverWheel : public Wheel {

};

class FactoryBase {
public:
	virtual Engine* createEngine() = 0;
	virtual Wheel* createWheel() = 0;
};

class GoldFactory : public FactoryBase {
	virtual GoldEngine* createEngine() override {
		return new GoldEngine();
	}
	virtual GoldWheel* createWheel() override {
		return new GoldWheel();
	}
};

class SilverFactory : public FactoryBase {
	virtual SilverEngine* createEngine() override {
		return new SilverEngine();
	}
	virtual SilverWheel* createWheel() override {
		return new SilverWheel();
	}
};

class Car {
public:
	virtual ~Car() {
		if (engine != nullptr) {
			delete engine;
		}
		if (wheel != nullptr) {
			delete wheel;
		}
		engine = nullptr;
		wheel = nullptr;
	}
	Engine* engine = nullptr;
	Wheel* wheel = nullptr;
};

enum class CarType {
	Gold,
	Silver,
};

class CarFactory {
public:
	void createCar(CarType carType, Car* car) {
		if (car == nullptr) {
			return;
		}
		std::shared_ptr<FactoryBase> factory = nullptr;
		switch (carType) {
		case CarType::Gold: {
			factory = std::make_shared<GoldFactory>();
			break;
		}
		case CarType::Silver: {
			factory = std::make_shared<SilverFactory>();
			break;
		}
		default:
			break;
		}
		car->engine = factory->createEngine();
		car->wheel = factory->createWheel();
	}
};



