#pragma once


#include "Scene.h"
#include "SceneManager.h"
#include "Map.h"


struct CreatedObj;
struct Monster;
struct Item;
struct Player;
struct NextStage;




class GameScene : public Scene
{
public:

	GameScene(SceneManager * a_pParent);
	virtual ~GameScene();

	virtual SceneManager::eScene GetScene();

	virtual void Init();
	virtual void Update();
	virtual void Render();
	void Scan();

	virtual void KeyInput(SceneManager::eKey a_eKey);

public:

	class SceneManager * m_refParent = nullptr;

public:
	void ClearStage();

public:
	int stage = 0;
	enum { mapLimit = 200 };
	char currMap[mapLimit][mapLimit]{ ' ', };
	std::map<Pos, Monster*> monsters;
	std::map<Pos, Item*> items;
	Player* player;
	NextStage* nextStage;

};




struct CreatedObj {
	// cstr
	// CreatedObj() { id = creationCnt++; }
	CreatedObj(GameScene* _scene, char _graphic) : scene(_scene), graphic(_graphic) { id = creationCnt++; }
	// CreatedObj(int _id, GameScene* _scene) : id(_id), scene(_scene) { ++creationCnt; }
	virtual ~CreatedObj() {}

	virtual void myTurn() = 0;
	static bool isOccured(float _probabilty);
	static int getRandom(int _input);

	char graphic;
	Pos pos = { 0,0 };
	GameScene* scene;
	inline static int creationCnt = 0;
	inline static time_t randomSeed = 0;
	int id = 0; // should be unique!
};

inline bool CreatedObj::isOccured(float _probabilty) {
	randomSeed = time(0);
	srand(randomSeed);
	float temp = (static_cast<double>(rand()) / (RAND_MAX));
	return (_probabilty > temp) ? true : false;
}

inline int CreatedObj::getRandom(int _input) {
	int temp(0);
	if (_input != 0) {
		randomSeed = time(0);
		srand(randomSeed);
		temp = rand() % _input;
	}
	return temp;
}

struct CreatedObjComposite : public Composite<CreatedObj> {};

struct Character : public CreatedObj {
	Character(GameScene* _scene, char _graphic) : CreatedObj(_scene, _graphic) {}
	void move(Pos _dir);
	virtual void attack(Character* _oppo);
	void damage(int _damage);
	virtual void die() = 0;

	int life = 0;
	int attackDamage = 0;
	int exp = 0;

};

struct Player : public Character {
	Player(GameScene* _scene);
	void moveCmd(SceneManager::eKey _input);
	void die() override;
	void myTurn() override;
	virtual void attack(Character* _oppo) override;
	int level = 1;
	int nextToLevelUp = 20;
	int weaponDamage = 0;
	int durability = 0;
	int sightRange = 0;
	std::vector<Pos> sightArea;
};

struct Monster : public Character {
	Monster(GameScene* _scene);
	void die() override;
	void myTurn() override;
	int aggroRange = 0;
	inline static int aliveCnt = 0;
	std::vector<Pos> aggroArea;
};


struct Item : public CreatedObj {
	Item(GameScene* _scene) : CreatedObj(_scene, 'I') {}

	void myTurn() override;
};

struct NextStage : public CreatedObj {
	NextStage(GameScene* _scene) : CreatedObj(_scene, 'E') {}

	void myTurn() override;
};