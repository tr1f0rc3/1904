#pragma once
#include "Scene.h"

class Scene1 : public Scene {
public:

	Scene1();
	virtual ~Scene1() {}

	virtual void getInfo() const override;

	virtual bool init() override;
	virtual bool getInput(SceneMaster::keyInput _input) override;
	virtual bool update() override;
	virtual bool render() const override;


	int sc0Cnt = 0;
	std::string printText = "Welcome to Scene 1";
};
