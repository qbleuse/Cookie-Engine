#include <reactphysics3d/reactphysics3d.h>
#include "Coordinator.hpp"
#include "Resources/Scene.hpp"
#include "Vec3.hpp"
#include <iostream>

using namespace Cookie::Resources;
using namespace Cookie::Core::Math;

#define EPSILON 1/4096

Scene::Scene()
{
}

Scene::~Scene()
{

}

void Scene::InitCoordinator(Cookie::ECS::Coordinator& coordinator)
{
	coordinator.entityHandler = &entityHandler;
	coordinator.componentHandler = &componentHandler;
	coordinator.armyHandler = &armyHandler;
}

