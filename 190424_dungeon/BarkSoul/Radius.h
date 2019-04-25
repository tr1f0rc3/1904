#pragma once

class Radius
{
public:
	static Radius* getInstance();
	void releaseInstance();
	static inline Radius* instance = nullptr;
	// interface
	std::vector<Pos> radiusToPos(int);

private:
	// cstr 
	Radius() = default;
	Radius(const Radius& _input) = delete;
	Radius(Radius&& _input) = delete;
	~Radius() { delete instance; instance = nullptr; }
	// data
	std::map<int, std::vector<Pos>> radiusToPosDict;
};

inline Radius* Radius::getInstance() {
	if (instance == nullptr) {
		auto temp = new Radius;
		instance = temp;
	}
	return instance;
}

inline void Radius::releaseInstance() {
	delete instance;
	instance = nullptr;
}

#define RADIUS_TO_POS(x)		(Radius::getInstance()->radiusToPos((x)))
#define RELEASE_RAIDUS()	(Radius::releaseInstance())

