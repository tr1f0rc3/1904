#include "pch.h"
#include "GameScene.h"


void Character::move(Pos _dir) {
	Pos dest = pos + _dir;
	if ((scene->currMap)[dest.y][dest.x] == '#') {
		return;
	}
	else if ((scene->currMap)[dest.y][dest.x] == 'M') {
		assert((scene->monsters)[dest]->pos == dest);
		Monster* opponent = (scene->monsters)[dest];
		attack(opponent);
	}
	else { pos + _dir; }
}

inline void Character::attack(Character* _oppo) {
	_oppo->damage(attackDamage);
}

void Player::moveCmd(SceneManager::eKey _input) {
	Pos dest;
	switch (_input) {
	case SceneManager::eKey::Up: {
		dest = Pos(-1, 0);
		break;
	}
	case SceneManager::eKey::Down: {
		dest = Pos(1, 0);
		break;
	}
	case SceneManager::eKey::Left: {
		dest = Pos(0, -1);
		break;
	}
	case SceneManager::eKey::Right: {
		dest = Pos(0, 1);
		break;
	}
	default:
		break;
	}
	move(dest);
}

inline void Player::attack(Character* _oppo) {
	_oppo->damage(attackDamage + weaponDamage);
	if (durability > 0) {
		--durability;
		if (durability == 0 && weaponDamage > 0) {
			weaponDamage = 0;
		}
	}
};

void Character::damage(int _damage) {
	life -= _damage;
	if (life <= 0) { die(); };
}

Player::Player(GameScene* _scene) : Character(_scene, 'P') {
	life = 1000 + getRandom(100);
	attackDamage = 100 + getRandom(20);
	sightRange = 10 + getRandom(2);
	sightArea = RADIUS_TO_POS(sightRange);
}

void Player::myTurn() {
	if (exp >= nextToLevelUp) {
		++level;
		exp %= nextToLevelUp;
		nextToLevelUp += level * 10;
	}
}


void Player::die() {
	SceneMng()->ChangeScene(SceneManager::eScene::GameOver);
}

Monster::Monster(GameScene* _scene) : Character(_scene, 'M') {
	int stages = scene->stage;
	life = 200 + 100 * stages + getRandom(30 * stages);
	attackDamage = 40 + 20 * stages + getRandom(6 * stages);
	exp = 5 + stages;
	aggroRange = 2 + getRandom(3);
	aggroArea = RADIUS_TO_POS(aggroRange);
	++aliveCnt;
}

void Monster::myTurn() {
	int dir1;
	Pos dist(0, 0);
	auto it = find(aggroArea.begin(), aggroArea.end(), scene->player->pos);
	if (it == aggroArea.end()) {
		if (isOccured(0.5)) {
			(isOccured(0.5)) ? (dir1 = 1) : (dir1 = -1);
			(isOccured(0.5)) ? (dist.x = dir1) : (dist.y = dir1);
		}
	}
	else {
		dist = scene->player->pos - pos;
		if (isOccured(0.5)) {
			if (dist.y > 0) { dist.y = 1; }
			else { dist.y = -1; }
			dist.x = 0;
		}
		else {
			if (dist.x < 0) { dist.x = 1; }
			else { dist.x = -1; }
			dist.y = 0;
		}
	}
	move(dist);
}

inline void Monster::die() {
	(scene->player)->exp += exp;
	--aliveCnt;
	graphic = ' ';
}