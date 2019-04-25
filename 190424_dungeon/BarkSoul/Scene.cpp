#include "pch.h"
#include "Scene.h"


Scene::Scene(SceneManager * a_pParent)
{
	assert(a_pParent != nullptr);
	m_refParent = a_pParent;
}

Scene::~Scene()
{
	m_refParent = nullptr;
}

void Scene::Init() { }
void Scene::Update() { }
void Scene::Render() { }
void Scene::KeyInput(SceneManager::eKey a_eKey) { }
