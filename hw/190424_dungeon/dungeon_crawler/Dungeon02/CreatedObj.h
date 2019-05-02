#pragma once

enum class direction {
	none, vertical, horitzontal,
};

class CreatedObj;
class Skill;
class CreatedSkill;

class CreatedObj {
public:
	// cstr
	CreatedObj() : id() { ++creationCnt; } // dangerous
	CreatedObj(int _id) : id(_id) { ++creationCnt; }
	virtual ~CreatedObj() {}
	// get set
	int getId() const { return id; }
	// interface
	virtual void castSkill(CreatedSkill* _skill, CreatedObj* _target);
	// 
	inline static int creationCnt = 0;
	inline static time_t randomSeed = 0;
protected:
	static bool isOccured(float _probabilty);
	int id = 0; // should be unique!
};

inline static bool isOccured(float _probabilty) {
	float temp = (static_cast<double>(rand()) / (RAND_MAX));
	return (_probabilty > temp) ? true : false;
}


struct CreatedObjComposite : public Composite<CreatedObj> {};


class Skill {


};

class CreatedSkill {
public:
	CreatedObj* user = nullptr;
	int useCnt = 0;
	int coolDown = 0;
	Skill* info = nullptr;
	bool Usable = true;
};
