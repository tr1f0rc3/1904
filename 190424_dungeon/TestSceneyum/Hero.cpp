#include "pch.h"
#include "Hero.h"


Hero::Hero(Map* map, int x, int y) : BaseObject(map, x, y)
{
	life = 100;
	attackDamage = 10;
}

Hero::~Hero()
{
}

void Hero::Move(int _x, int _y, std::vector< std::vector<class BaseObject*>>* vcObject) {

	int newX = pos.x + _x;
	int newY = pos.y + _y;
	Point newPos(newX, newY);
	assert(newX > 0 && newY > 0 && newX < 10 && newY < 10);
	auto& newlineY = (*vcObject)[newY];
	auto it = newlineY.begin();
	for (; it != newlineY.end(); ++it) {
		if ((*it)->pos.x == newX) {
			break;
		}
	}
	if (it != newlineY.end()) {
		assert((*it)->GetType() != eObjectType::Hero);
		switch ((*it)->GetType())
		{
		case eObjectType::Wall: // no move
			break;
		case eObjectType::Item:
			pos.x = newX;
			pos.y = newY;
			life += 10;
			break;
		case eObjectType::Monster: { // attack 
			auto opponent = *it;
			Attack(this, opponent, weaponDamage + attackDamage);
			opponent->Attack(opponent, this, opponent->attackDamage);
			if (exp >= nextLevel) {
				LevelUp();
			}
			break;
		}
		case eObjectType::DeadBody:
			break;		
		default:
			break;
		}
	}
	else { // plain move
		pos.x = newX;
		pos.y = newY;
	}
	return;
};


void Hero::Damage(BaseObject* attacker, int dmg) {
	BaseObject::Damage(attacker, dmg);
	if (life <= 0) Die();
}

void Hero::Die() {
	std::cout << "YOU DIED" << std::endl;
	SceneMng()->ChangeScene(SceneManager::eScene::Intro);
}


eObjectType Hero::GetType()
{
	return eObjectType::Hero;
}
