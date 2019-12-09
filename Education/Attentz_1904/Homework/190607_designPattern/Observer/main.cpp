#include "pch.h"
#include "ResourceMng.h"
#include "SceneMng.h"
#include "Scene.h"
#include "Data.h"
using namespace std;

int main() {

	SceneMng::instance()->resourceMng = ResourceMng::instance();
	ResourceMng::instance()->sceneMng = SceneMng::instance();
	Data data;
	ResourceMng::instance()->dataMap = &data;

	SceneMng::instance()->changeScene(SceneMng::EScene::title);
	SceneMng::instance()->getScene()->play();
	SceneMng::instance()->changeScene(SceneMng::EScene::loading);
	SceneMng::instance()->getScene()->play();
	SceneMng::instance()->changeScene(SceneMng::EScene::game);
	SceneMng::instance()->getScene()->play();


}