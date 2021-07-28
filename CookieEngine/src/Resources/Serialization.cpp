#include "Physics/PhysicsHandle.hpp"
#include "Game.hpp"

#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Resources/Scene.hpp"

#include "SoundManager.hpp"
#include "Resources/ResourcesManager.hpp"
#include "ECS/Coordinator.hpp"
#include "ECS/ComponentHandler.hpp"
#include "Resources/Serialization.hpp"
#include "Resources/Prefab.hpp"
#include "Light.hpp"
#include "Particles/ParticlesSystem.hpp"
#include "ComponentGameplay.hpp"
#include "Camera.hpp"
#include "Gameplay/AIBehavior.hpp"
#include <reactphysics3d/reactphysics3d.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;
using namespace Cookie::Resources;
using namespace Cookie::ECS;
using namespace Cookie::Resources::Particles;
using namespace Cookie::Resources::Serialization;
using namespace Cookie::Gameplay;

void Cookie::Resources::Serialization::Save::ToJson(json& js, const Cookie::ECS::EntityHandler& entity)
{
	//Save data in entity
	for (int i = 0; i < entity.livingEntities; i++)
		js["EntityHandler"] += json{ { "entity", { { "id", entity.entities[i].id }, { "signature", entity.entities[i].signature }, 
								{ "name", entity.entities[i].name }, { "namePrefab", entity.entities[i].namePrefab } } } };
}

void Cookie::Resources::Serialization::Save::ToJson(json& js, const Cookie::ECS::EntityHandler& entity, Cookie::ECS::ComponentHandler& component, Cookie::Resources::ResourcesManager& resourcesManager)
{
	//Save all component for each entity
	for (int i = 0; i < entity.livingEntities; i++)
	{
		if (entity.entities[i].signature & C_SIGNATURE::TRANSFORM)
		{
			Cookie::ECS::ComponentTransform& transform = component.GetComponentTransform(entity.entities[i].id);
			json& trans = js["ComponentHandler"]["Transform"];
			trans += json{ { "localTRS", { { "translate", transform.pos.e },
													{ "rotation", transform.rot.e },
													{ "scale", transform.scale.e } } } };
		}
		if (entity.entities[i].signature & C_SIGNATURE::MODEL)
		{
			Cookie::ECS::ComponentModel& model = component.GetComponentModel(entity.entities[i].id);
			json& modelJ = js["ComponentHandler"]["Model"];
			modelJ += json{ { "model", model.mesh != nullptr ? model.mesh->name : "NO MESH" },
							{ "texture", { { "albedo" , model.albedo != nullptr ? model.albedo->name : "NO ALBEDO"},
										    { "normal" , model.normal != nullptr ? model.normal->name : "NO NORMAL"}, 
											{ "metallic" , model.metallicRoughness != nullptr ? model.metallicRoughness->name : "NO METALLIC"},
											{ "icon" , model.icon != nullptr ? model.icon->name : "NO ICON"}} } };
			
		}
		if (entity.entities[i].signature & C_SIGNATURE::PHYSICS)
		{
			SavePhysic(js["PhysicHandler"][js["PhysicHandler"].size()], component.GetComponentPhysics(entity.entities[i].id));
		}
		if (entity.entities[i].signature & C_SIGNATURE::GAMEPLAY)
		{
			ComponentGameplay& gameplay = component.GetComponentGameplay(entity.entities[i].id);

			int index = js["Gameplay"].size();
			json& game = js["Gameplay"][index];
			game["TeamName"] = gameplay.teamName;
			game["SignatureGameplay"] = gameplay.signatureGameplay;
			game["Type"] = gameplay.type;
			game["Cost"]["CostPrimary"] = gameplay.cost.costPrimary;
			game["Cost"]["CostSecondary"] = gameplay.cost.costSecondary;
			game["Cost"]["CostSupply"] = gameplay.cost.costSupply;
			game["Cost"]["TimeToProduce"] = gameplay.cost.timeToProduce;

			if (gameplay.signatureGameplay & CGP_SIGNATURE::LIVE)
			{
				game["CGPLive"]["Life"] = gameplay.componentLive.lifeCurrent;
				game["CGPLive"]["LifeMax"] = gameplay.componentLive.lifeMax;
				game["CGPLive"]["Armor"] = gameplay.componentLive.armor;
				game["CGPLive"]["PosLife"] = gameplay.componentLive.lifeBarOffset.e;
			}

			if (gameplay.signatureGameplay & CGP_SIGNATURE::ATTACK)
			{
				game["CGPAttack"]["NeedToAttack"] = gameplay.componentAttack.needToAttack;
				game["CGPAttack"]["AttackDamage"] = gameplay.componentAttack.attackDamage;
				game["CGPAttack"]["AttackSpeed"] = gameplay.componentAttack.attackSpeed;
				game["CGPAttack"]["AttackRange"] = gameplay.componentAttack.attackRange;
			}

			if (gameplay.signatureGameplay & CGP_SIGNATURE::MOVE)
			{
				game["CGPMove"]["MoveSpeed"] = gameplay.componentMove.moveSpeed;
				game["CGPMove"]["isFlying"] = gameplay.componentMove.isFlying;
			}

			if (gameplay.signatureGameplay & CGP_SIGNATURE::PRODUCER)
			{
				for (int i = 0; i < gameplay.componentProducer.possibleUnits.size(); i++)
					game["CGPProducer"]["name"] += gameplay.componentProducer.possibleUnits[i]->name;

				for (int i = 0; i < gameplay.componentProducer.occupiedTiles.size(); i++)
					game["CGPProducer"]["OccupiedTiles"] += gameplay.componentProducer.occupiedTiles[i]->id;

				game["CGPProducer"]["TileSize"] = gameplay.componentProducer.tileSize.e;
				game["CGPProducer"]["SupplyGiven"] = gameplay.componentProducer.supplyGiven;
			}

			if (gameplay.signatureGameplay & CGP_SIGNATURE::WORKER)
				for (int i = 0; i < gameplay.componentWorker.possibleBuildings.size(); i++)
					game["CGPWorker"]["name"] += gameplay.componentWorker.possibleBuildings[i]->name;

			if (gameplay.signatureGameplay & CGP_SIGNATURE::RESOURCE)
			{
				game["CGPResource"] = json{ { "resourcesReserve", gameplay.componentResource.resourceReserve },
											{ "isPrimary", gameplay.componentResource.isPrimary } };
			}	
		}
		if (entity.entities[i].signature & C_SIGNATURE::FX)
		{
			int index = js["FX"].size();
			json& fx = js["FX"][index];
			ComponentGameplay& gameplay = component.GetComponentGameplay(entity.entities[i].id);
			if (gameplay.signatureGameplay & CGP_SIGNATURE::ATTACK)
			{
				fx["CGPAttack"]["sfxAttack"] = gameplay.componentAttack.sfxAttack ? gameplay.componentAttack.sfxAttack->name : "NONE";
				fx["CGPAttack"]["vfxAttack"] = gameplay.componentAttack.vfxAttack ? gameplay.componentAttack.vfxAttack->name : "NONE";

			}
			if (gameplay.signatureGameplay & CGP_SIGNATURE::LIVE)
			{
				fx["CGPLive"]["sfxDeath"] = gameplay.componentLive.sfxDeath ? gameplay.componentLive.sfxDeath->name : "NONE";
				fx["CGPLive"]["vfxDeath"] = gameplay.componentLive.vfxDeath ? gameplay.componentLive.vfxDeath->name : "NONE";

				fx["CGPLive"]["sfxHit"] = gameplay.componentLive.sfxHit ? gameplay.componentLive.sfxHit->name : "NONE";
				fx["CGPLive"]["vfxHit"] = gameplay.componentLive.vfxHit ? gameplay.componentLive.vfxHit->name : "NONE";
			}
		}
	}
}

void Cookie::Resources::Serialization::Save::SaveScene(Cookie::Resources::Scene& actScene, Cookie::Resources::ResourcesManager& resourcesManager)
{
	std::ofstream file(actScene.filepath);

	json js;

	//Name Type
	{
		js["Name"] = actScene.name;
		js["Type"] = "map";
	}
	
	//Map
	{
		json& map = js["Map"];
		Cookie::Resources::Map& actMap = actScene.map;
		map["tilesNb"] = actMap.tilesNb.e;
		map["tilesSize"] = actMap.tilesSize.e;

		map["trs"]["pos"] = actMap.trs.pos.e;
		map["trs"]["rot"] = actMap.trs.rot.e;
		map["trs"]["scale"] = actMap.trs.scale.e;

		if (actMap.model.albedo != nullptr)
			map["model"]["texture"]["albedo"] = actMap.model.albedo->name;
		if (actMap.model.normal != nullptr)
			map["model"]["texture"]["normal"] = actMap.model.normal->name;
		if (actMap.model.metallicRoughness != nullptr)
			map["model"]["texture"]["metallic"] = actMap.model.metallicRoughness->name;

		SavePhysic(map["physic"], actMap.physic);
	}

	//Skybox
	{
		if (actScene.skyBox)
			js["SkyboxName"] = actScene.skyBox->name;
	}

	//Camera
	{
		json& cam = js["Camera"];
		Cookie::Render::Camera* camera = actScene.camera.get();
		if (camera)
		{
			cam["camNear"] = camera->camNear;
			cam["camFar"] = camera->camFar;
			cam["fov"] = camera->fov;
			cam["aspectRatio"] = camera->aspectRatio;
			cam["windowOffset"] = camera->windowOffset.e;
			cam["pos"] = camera->pos.e;
			cam["rot"] = camera->rot.e;
		}
	}

	if (actScene.entityHandler.livingEntities > 0)
	{
		//Entities
		Cookie::Resources::Serialization::Save::ToJson(js, actScene.entityHandler);
		//Components
		Cookie::Resources::Serialization::Save::ToJson(js, actScene.entityHandler, actScene.componentHandler, resourcesManager);
	}

	//ArmyHandler
	{
		json& ArmyHandler = js["ArmyHandler"];
		for (int i = 0; i < actScene.armyHandler.livingArmies; i++)
		{
			Cookie::Gameplay::Army& army = actScene.armyHandler.armies[i];
			ArmyHandler += json{ { "name", army.name },
								{ "Income", { { "incomePrimary", army.income.primary }, 
												{ "incomeSecondary", army.income.secondary },
												{ "supplyCurrent", army.income.supplyCurrent },
												{ "supplyMax", army.income.supplyMax } } } };
		}

	}

	//ArmyCoordinator
	{
		for (int i = 0; i < actScene.armyHandler.armiesCoordinator.size(); i++)
		{
			json& armyCoordinator = js["ArmiesCoordinator"][i];
			ArmyCoordinator& coordinator = actScene.armyHandler.armiesCoordinator[i];
			armyCoordinator["armyName"] = coordinator.army->name;
			armyCoordinator["aiBehaviorName"] = coordinator.behavior->name;

			armyCoordinator["nbOfWorkerInProduction"] = coordinator.nbOfWorkerInProduction;
			armyCoordinator["nbOfBuildingInProduction"] = coordinator.nbOfBuildingInProduction;
			armyCoordinator["nbOfUnitInProduction"] = coordinator.nbOfUnitInProduction;

			armyCoordinator["canAttack"] = coordinator.canAttack;
			armyCoordinator["canDefend"] = coordinator.canDefend;

			armyCoordinator["currentStepIndex"] = coordinator.currentStepIndex;
			//StepGoals
			{
				armyCoordinator["nbOfWorker"] = coordinator.stepGoals.nbOfWorker;
				armyCoordinator["nbOfUnits"] = coordinator.stepGoals.nbOfUnits;
				for (int j = 0; j < coordinator.stepGoals.listOfBuildings.size(); j++)
					armyCoordinator["ListOfBuilding"] += coordinator.stepGoals.listOfBuildings[j];
			}
		}
	}

	//UI Scene
	{
		Cookie::UI::UIscene& ui = actScene.uiScene;

		std::vector<Cookie::UI::UIscene::GameWindowInfo> info = ui.SaveLayout();
		for (int i = 0; i < info.size(); i++)
		{
			js["UIScene"] += json{  { "ID", info[i].ID },
									{ "xPos", info[i].xPos },
									{ "yPos", info[i].yPos },
									{ "width", info[i].width },
									{ "height", info[i].height } };
		}
	}

	//Light
	{
		Render::LightsArray& array = actScene.lights;
		json& light = js["LightsArray"];
		
		light["useDir"] = array.useDir;
		light["usedPoints"] = array.usedPoints;
		//DirLight
		{
			json& dir = light["DirLight"];
			dir["dir"] = array.dirLight.dir.e;
			dir["color"] = array.dirLight.color.e;
			dir["castShadow"] = array.dirLight.castShadow;
		}
		//PointLights
		{
			json& point = light["PointLights"];
			for (int i = 0; i < array.pointLights.size(); i++)
			{
				point += json{	{ "pos", array.pointLights[i].pos.e },
								{ "color", array.pointLights[i].color.e },
								{ "radius", array.pointLights[i].radius } };
			}
		}
	}

	file << std::setw(4) << js << std::endl;
}

void Cookie::Resources::Serialization::Save::SavePrefab(const Prefab* const & prefab)
 {
	 std::ofstream file(prefab->filepath);

	 json js;

	 js["Signature"] = prefab->signature;
	 js["Name"] = prefab->name;
	 
	 if (prefab->signature & C_SIGNATURE::TRANSFORM)
	 {
		 //Transform
		 json& trans = js["Transform"];
		 trans["Rotation"] = prefab->transform.rot.e;
		 trans["Scale"] = prefab->transform.scale.e;
	 }
	 
	 if (prefab->signature & C_SIGNATURE::MODEL)
	 {
		 //Model
		 json& model = js["Model"];
		 if (prefab->model.mesh != nullptr)
			 model["Mesh"] = prefab->model.mesh->name;
		 else
			 model["Mesh"] = 0;

		 if (prefab->model.albedo != nullptr)
			 model["Texture"]["albedo"] = prefab->model.albedo->name;
		 else
			 model["Texture"]["albedo"] = 0;

		 if (prefab->model.normal != nullptr)
			 model["Texture"]["normal"] = prefab->model.normal->name;
		 else
			 model["Texture"]["normal"] = 0;

		 if (prefab->model.metallicRoughness != nullptr)
			 model["Texture"]["metallic"] = prefab->model.metallicRoughness->name;
		 else
			 model["Texture"]["metallic"] = 0;

		 if (prefab->model.icon != nullptr)
			 model["Texture"]["icon"] = prefab->model.icon->name;
		 else
			 model["Texture"]["icon"] = 0;
	 }

	 if (prefab->signature & C_SIGNATURE::PHYSICS)
	 {
		 if (prefab->physics.physBody != nullptr)
		 {
			 json& rigid = js["Rigidbody"];
			 rigid["type"] = prefab->physics.physBody->getType();
			 rigid["angularDamping"] = prefab->physics.physBody->getAngularDamping();
			 rigid["linearDamping"] = prefab->physics.physBody->getLinearDamping();
			 rigid["mass"] = prefab->physics.physBody->getMass();
			 rigid["active"] = prefab->physics.physBody->isActive();
			 rigid["allowedToSleep"] = prefab->physics.physBody->isAllowedToSleep();
			 rigid["sleeping"] = prefab->physics.physBody->isSleeping();
			 rigid["gravityEnabled"] = prefab->physics.physBody->isGravityEnabled();
		 }
	 }
	 else
		 js["Rigidbody"] = "nullptr";

	 if (prefab->signature & C_SIGNATURE::PHYSICS)
	 {
		 if (prefab->physics.physColliders.size() != 0)
		 {
			 for (int i = 0; i < prefab->physics.physColliders.size(); i++)
			 {
				 ::reactphysics3d::Collider* actCollider = prefab->physics.physColliders[i];
				 json& collider = js["Colliders"][i];
				 if (actCollider->getCollisionShape()->getName() == ::reactphysics3d::CollisionShapeName::SPHERE)
				 {
					 float radius = static_cast<::reactphysics3d::SphereShape*>(actCollider->getCollisionShape())->getRadius();
					 collider["type"] = "Sphere";
					 collider["radius"] = radius;
				 }
				 else if (actCollider->getCollisionShape()->getName() == ::reactphysics3d::CollisionShapeName::CAPSULE)
				 {
					 ::reactphysics3d::CapsuleShape* capsule = static_cast<::reactphysics3d::CapsuleShape*>(actCollider->getCollisionShape());
					 collider["type"] = "Capsule";
					 collider["radius"] = capsule->getRadius();
					 collider["height"] = capsule->getHeight();
				 }
				 else if (actCollider->getCollisionShape()->getName() == ::reactphysics3d::CollisionShapeName::BOX)
				 {
					 ::reactphysics3d::Vector3 box = static_cast<::reactphysics3d::BoxShape*>(actCollider->getCollisionShape())->getHalfExtents();
					 collider["type"] = "Box";
					 collider["HalfExtents"] = { box.x, box.y, box.z };
				 }

				 ::reactphysics3d::Material& mat = actCollider->getMaterial();

				 ::reactphysics3d::Vector3		vec = actCollider->getLocalToBodyTransform().getPosition();
				 ::reactphysics3d::Quaternion	quat = actCollider->getLocalToBodyTransform().getOrientation();
				 collider["transform"]["pos"] = { vec.x, vec.y, vec.z };
				 collider["transform"]["quaternion"] = { quat.w, quat.x, quat.y, quat.z };
				 collider["bounciness"] = mat.getBounciness();
				 collider["frictionCoeff"] = mat.getFrictionCoefficient();
				 collider["density"] = mat.getMassDensity();
				 collider["rollingResistance"] = mat.getRollingResistance();
			 }
		 }
	 }
	 else
		 js["Colliders"] = "nullptr";

	 if (prefab->signature & C_SIGNATURE::GAMEPLAY)
	 {
		 const ComponentGameplay& gameplay = prefab->gameplay;
		 json& gp = js["Gameplay"];
		 gp["TeamName"] = gameplay.teamName;
		 gp["Type"] = gameplay.type;
		 gp["SignatureGameplay"] = gameplay.signatureGameplay;

		 json& cost = js["Gameplay"]["Cost"];
		 cost["CostPrimary"] = gameplay.cost.costPrimary;
		 cost["CostSecondary"] = gameplay.cost.costSecondary;
		 cost["CostSupply"] = gameplay.cost.costSupply;
		 cost["TimeToProduce"] = gameplay.cost.timeToProduce;

		 json& live = js["Gameplay"]["CGPLive"];
		 live["Life"] = gameplay.componentLive.lifeCurrent;
		 live["LifeMax"] = gameplay.componentLive.lifeMax;
		 live["Armor"] = gameplay.componentLive.armor;
		 live["PosLife"] = gameplay.componentLive.lifeBarOffset.e;

		 json& attack = js["Gameplay"]["CGPAttack"];
		 attack["NeedToAttack"] = gameplay.componentAttack.needToAttack;
		 attack["AttackDamage"] = gameplay.componentAttack.attackDamage;
		 attack["AttackSpeed"] = gameplay.componentAttack.attackSpeed;
		 attack["AttackCooldown"] = gameplay.componentAttack.attackCooldown;
		 attack["AttackRange"] = gameplay.componentAttack.attackRange;

		 json& move = js["Gameplay"]["CGPMove"];
		 move["MoveSpeed"] = gameplay.componentMove.moveSpeed;
		 move["isFlying"] = gameplay.componentMove.isFlying;

		 json& produ = js["Gameplay"]["CGPProducer"];
		 for (int i = 0; i < gameplay.componentProducer.possibleUnits.size(); i++)
			 produ["name"] += gameplay.componentProducer.possibleUnits[i]->name;

		 produ["TileSize"] = gameplay.componentProducer.tileSize.e;
		 produ["SupplyGiven"] = gameplay.componentProducer.supplyGiven;

		 json& worker = js["Gameplay"]["CGPWorker"];
		 for (int i = 0; i < gameplay.componentWorker.possibleBuildings.size(); i++)
			 worker["name"] += gameplay.componentWorker.possibleBuildings[i]->name;
	 }

	 if (prefab->signature & C_SIGNATURE::FX)
	 {
		 json& fx = js["FX"];
		 const ComponentGameplay& gameplay = prefab->gameplay;
		 if (gameplay.signatureGameplay & CGP_SIGNATURE::ATTACK)
		 {
			 fx["CGPAttack"]["sfxAttack"] = gameplay.componentAttack.sfxAttack ? gameplay.componentAttack.sfxAttack->name : "NONE";
			 fx["CGPAttack"]["vfxAttack"] = gameplay.componentAttack.vfxAttack ? gameplay.componentAttack.vfxAttack->name : "NONE";
		 }
		 if (gameplay.signatureGameplay & CGP_SIGNATURE::LIVE)
		 {
			 fx["CGPLive"]["sfxDeath"] = gameplay.componentLive.sfxDeath ? gameplay.componentLive.sfxDeath->name : "NONE";
			 fx["CGPLive"]["vfxDeath"] = gameplay.componentLive.vfxDeath ? gameplay.componentLive.vfxDeath->name : "NONE";

			 fx["CGPLive"]["sfxHit"] = gameplay.componentLive.sfxHit ? gameplay.componentLive.sfxHit->name : "NONE";
			 fx["CGPLive"]["vfxHit"] = gameplay.componentLive.vfxHit ? gameplay.componentLive.vfxHit->name : "NONE";
		 }
	 }

	 file << std::setw(4) << js << std::endl;
 }

void Cookie::Resources::Serialization::Save::SaveAllPrefabs(Cookie::Resources::ResourcesManager& resourcesManager)
 {
	 for (std::unordered_map<std::string, std::unique_ptr<Prefab>>::iterator prefab = resourcesManager.prefabs.begin(); prefab != resourcesManager.prefabs.end(); prefab++)
		 Resources::Serialization::Save::SavePrefab(prefab->second.get());
 }

void Cookie::Resources::Serialization::Save::SaveTexture(std::string& name, Cookie::Core::Math::Vec4& color)
 {
	 std::ofstream file("Assets/Textures/" + name + ".TAsset");

	 json js;

	 js["color"] = color.e;
	 js["name"] = name;

	 file << std::setw(4) << js << std::endl;
 }

void Cookie::Resources::Serialization::Save::SavePhysic(json& js, Cookie::ECS::ComponentPhysics& physics)
 {
	 std::vector<::reactphysics3d::Collider*>& colliders = physics.physColliders;
	 for (int j = 0; j < colliders.size(); j++)
	 {
		 ::reactphysics3d::Collider* actCollider = colliders[j];
		 json& colliders = js["Colliders"][j];
		 if (actCollider->getCollisionShape()->getName() == ::reactphysics3d::CollisionShapeName::SPHERE)
		 {
			 float radius = static_cast<::reactphysics3d::SphereShape*>(actCollider->getCollisionShape())->getRadius();
			 colliders["type"] = "Sphere";
			 colliders["radius"] = radius;
		 }
		 else if (actCollider->getCollisionShape()->getName() == ::reactphysics3d::CollisionShapeName::CAPSULE)
		 {
			 ::reactphysics3d::CapsuleShape* capsule = static_cast<::reactphysics3d::CapsuleShape*>(actCollider->getCollisionShape());
			 colliders["type"] = "Capsule";
			 colliders["radius"] = capsule->getRadius();
			 colliders["height"] = capsule->getHeight();
		 }
		 else if (actCollider->getCollisionShape()->getName() == ::reactphysics3d::CollisionShapeName::BOX)
		 {
			 const ::reactphysics3d::Vector3& box = static_cast<::reactphysics3d::BoxShape*>(actCollider->getCollisionShape())->getHalfExtents();
			 colliders["type"] = "Box";
			 colliders["HalfExtents"] = { box.x, box.y, box.z };
		 }

		 ::reactphysics3d::Material& mat = actCollider->getMaterial();

		 const ::reactphysics3d::Vector3& vec = actCollider->getLocalToBodyTransform().getPosition();
		 const ::reactphysics3d::Quaternion& quat = actCollider->getLocalToBodyTransform().getOrientation();
		 colliders["transform"]["pos"] = { vec.x, vec.y, vec.z };
		 colliders["transform"]["quaternion"] = { quat.w, quat.x, quat.y, quat.z };
		 colliders["bounciness"] = mat.getBounciness();
		 colliders["frictionCoeff"] = mat.getFrictionCoefficient();
		 colliders["density"] = mat.getMassDensity();
		 colliders["rollingResistance"] = mat.getRollingResistance();
	 }

	 ::reactphysics3d::RigidBody*& rigibody = physics.physBody;

	 json& rigidbody = js["Rigidbody"];
	 rigidbody["type"] = rigibody->getType();
	 rigidbody["angularDamping"] = rigibody->getAngularDamping();
	 rigidbody["linearDamping"] = rigibody->getLinearDamping();
	 rigidbody["mass"] = rigibody->getMass();
	 rigidbody["active"] = rigibody->isActive();
	 rigidbody["allowedToSleep"] = rigibody->isAllowedToSleep();
	 rigidbody["sleeping"] = rigibody->isSleeping();
	 rigidbody["gravityEnabled"] = rigibody->isGravityEnabled();

	 ::reactphysics3d::Transform& physTrans = physics.physTransform;
	 const ::reactphysics3d::Vector3& pos = physTrans.getPosition();
	 const ::reactphysics3d::Quaternion& quat = physTrans.getOrientation();

	 js["physicTRS"]["position"] = { pos.x, pos.y, pos.z };
	 js["physicTRS"]["quaternion"] = { quat.w, quat.x, quat.y, quat.z };
 }

void Cookie::Resources::Serialization::Save::SaveVolumAndModeMusic(Sound* const & sound)
{
	std::string filepath = sound->filepath;

	std::size_t pos = filepath.find(".mp3");
	std::size_t end = filepath.length();

	filepath.replace(pos, end, ".MAsset");

	std::ofstream file(filepath);

	json js;

	js["Volume"] = sound->vol;
	js["Mode"] = sound->mode;

	file << std::setw(4) << js << std::endl;
}

void Cookie::Resources::Serialization::Save::SaveParticles(Cookie::Resources::Particles::ParticlesPrefab& particles)
{
	std::string filePath("Assets/VFX/" + particles.name + ".PSAsset");
	std::ofstream file(filePath);

	json js;

	js["Name"] = particles.name;

	//Data
	{
		for (int i = 0; i < particles.data.size(); i++)
		{
			json& data = js["Data"][i];
			Cookie::Resources::Particles::data particlesData = particles.data[i];
			data["Mesh"] = particlesData.mesh != nullptr ? particlesData.mesh->name : "NO MESH";
			data["Texture"] = particlesData.texture != nullptr ? particlesData.texture->name : "NO TEXTURE";
			data["Size"] = particlesData.size;
			data["CountFrame"] = particlesData.countFrame;
			data["CountAlive"] = particlesData.countAlive;
			data["IsBillBoard"] = particlesData.isBillboard;
		}
	}

	//Emitter
	{
		for (int i = 0; i < particles.emitter.size(); i++)
		{
			ParticlesEmitter& particlesEmitter = particles.emitter[i];
			json& emitter = js["Emitter"][i];
			if (particlesEmitter.generators.size() == 0)
				emitter["Generators"] = 0;
			for (int j = 0; j < particlesEmitter.generators.size(); j++)
			{
				json& generator = emitter["Generators"][j];
				generator["Type"] = particlesEmitter.generators[j]->type;
				switch (particlesEmitter.generators[j]->type)
				{
				case (TYPEGEN::POINTPOSITIONGEN): {
					break;
				}
				case (TYPEGEN::BOXPOSITIONGEN): {
					break;
				}
				case (TYPEGEN::CIRCLEPOSITIONGEN): {
					break;
				}
				case (TYPEGEN::SCALECONSTGEN): {
					ScaleConstGenerate& scaleC = dynamic_cast<ScaleConstGenerate&>(*particlesEmitter.generators[j].get());
					generator["scale"] = scaleC.scale.e;
					break;
				}
				case (TYPEGEN::SCALERANDGEN): {
					ScaleRandGenerate& scaleR = dynamic_cast<ScaleRandGenerate&>(*particlesEmitter.generators[j].get());
					generator["scaleMin"] = scaleR.scaleMin.e;
					generator["scaleMax"] = scaleR.scaleMax.e;
					break;
				}
				case (TYPEGEN::ROTATERANDGEN): {
					RotateRandGenerate& rot = dynamic_cast<RotateRandGenerate&>(*particlesEmitter.generators[j].get());
					generator["rotMin"] = rot.rotMin.e;
					generator["rotMax"] = rot.rotMax.e;
					break;
				}
				case (TYPEGEN::VELCONSTGEN): {
					VelocityConstGenerate& velC = dynamic_cast<VelocityConstGenerate&>(*particlesEmitter.generators[j].get());
					generator["vel"] = velC.vel.e;
					break;
				}
				case (TYPEGEN::VELRANDGEN): {
					VelocityRandGenerate& velR = dynamic_cast<VelocityRandGenerate&>(*particlesEmitter.generators[j].get());
					generator["velMin"] = velR.velMin.e;
					generator["velMax"] = velR.velMax.e;
					break;
				}
				case (TYPEGEN::MASSCONSTGEN): {
					MassConstGenerate& mass = dynamic_cast<MassConstGenerate&>(*particlesEmitter.generators[j].get());
					generator["mass"] = mass.mass;
					break;
				}
				case (TYPEGEN::TIMECONSTGEN): {
					TimeConstGenerate& timeC = dynamic_cast<TimeConstGenerate&>(*particlesEmitter.generators[j].get());
					generator["time"] = timeC.time;
					break;
				}
				case (TYPEGEN::TIMERANDGEN): {
					TimeRandGenerate& timeR = dynamic_cast<TimeRandGenerate&>(*particlesEmitter.generators[j].get());
					generator["timeMin"] = timeR.timeMin;
					generator["timeMax"] = timeR.timeMax;
					break;
				}
				case (TYPEGEN::COLORCONSTGEN): {
					ColorConstGenerate& colorC = dynamic_cast<ColorConstGenerate&>(*particlesEmitter.generators[j].get());
					generator["color"] = colorC.col.e;
					break;
				}
				case (TYPEGEN::COLORRANDGEN): {
					ColorRandGenerate& colorR = dynamic_cast<ColorRandGenerate&>(*particlesEmitter.generators[j].get());
					generator["colorMin"] = colorR.minCol.e;
					generator["colorMax"] = colorR.maxCol.e;
					break;
				}
				case (TYPEGEN::INITVELWITHPOINT):
					break;
				}
			}

			if (particlesEmitter.updates.size() == 0)
				emitter["Updates"] = 0;
			for (int k = 0; k < particlesEmitter.updates.size(); k++)
			{
				json& update = emitter["Updates"][k];
				update["Type"] = particlesEmitter.updates[k]->type;
				switch (particlesEmitter.updates[k]->type)
				{
				case (TYPEUP::UPDATEVEL):
					break;
				case (TYPEUP::UPDATESCALE): {
					UpdateScale& upScale = dynamic_cast<UpdateScale&>(*particlesEmitter.updates[k]);
					update["scaleEnd"] = upScale.scaleEnd.e;
					break;
				}
				case (TYPEUP::UPDATEALPHA): {
					UpdateAlpha& upAlpha = dynamic_cast<UpdateAlpha&>(*particlesEmitter.updates[k]);
					update["alphaEnd"] = upAlpha.alphaEnd;
					break;
				}
				case (TYPEUP::COLOROVERLIFE): {
					ColorOverLife& upColor = dynamic_cast<ColorOverLife&>(*particlesEmitter.updates[k]);
					update["colorEnd"] = upColor.colorEnd.e;
					break;
				}
				case (TYPEUP::ENABLEGRAVITY): {
					EnabledGravity& upGravity = dynamic_cast<EnabledGravity&>(*particlesEmitter.updates[k]);
					update["gravity"] = upGravity.gravity;
					break;
				}
				case (TYPEUP::UPDATETIME):
					break;
				case (TYPEUP::LOOP):
					break;
				case (TYPEUP::COLLISIONWITHPLANE): {
					CollisionWithPlane& upCollision = dynamic_cast<CollisionWithPlane&>(*particlesEmitter.updates[k]);
					update["distance"] = upCollision.dis;
					update["normal"] = upCollision.n.e;
					update["namePrefab"] = upCollision.namePrefab;
					break;
				}
				case (TYPEUP::CREATEPARTICLES): {
					break;
				}
				case (TYPEUP::SHADOW): {
					break;
				}
				case (TYPEUP::SPAWNEND): {
					SpawnEnd& upCollision = dynamic_cast<SpawnEnd&>(*particlesEmitter.updates[k]);
					update["namePrefab"] = upCollision.namePrefab;
					break;
				}
				case (TYPEUP::STOPAT):
					break;
				}
			}
		}
	}

	//Emit
	{
		for (int i = 0; i < particles.emit.size(); i++)
		{
			for (int j = 0; j < particles.emit[i].size(); j++)
			{
				std::string name = particles.emit[i][j].name;
				json& generator = js["Emitter"][i]["Generators"];
				json& update = js["Emitter"][i]["Updates"];
				int sizeGenerator = generator.size();
				int sizeUpdate = update.size();
				if (name == "PointPositionGen")
				{
					generator[sizeGenerator]["Type"] = 0;
					generator[sizeGenerator]["pos"] = particles.emit[i][j].data[0].e;
				}
				else if (name == "Loop")
				{
					update[sizeUpdate]["Type"] = 6;
				}
				else if (name == "BoxPositionGen")
				{
					generator[sizeGenerator]["Type"] = 1;
					generator[sizeGenerator]["pos"] = particles.emit[i][j].data[0].e;
					generator[sizeGenerator]["sizeBox"] = particles.emit[i][j].data[0].e;
				}
				else if (name == "CirclePositionGen")
				{
					generator[sizeGenerator]["Type"] = 2;
					generator[sizeGenerator]["pos"] = particles.emit[i][j].data[0].e;
					generator[sizeGenerator]["radius"] = particles.emit[i][j].data[1].x;
				}
				else if (name == "CreateParticles")
				{
					update[sizeUpdate]["Type"] = 8;
					update[sizeUpdate]["index"] = particles.emit[i][j].data[0].x;

					update[sizeUpdate]["coeffScale"] = particles.emit[i][j].data[1].x;
					update[sizeUpdate]["coeffPos"] = particles.emit[i][j].data[1].y;
					update[sizeUpdate]["time"] = particles.emit[i][j].data[1].z;
				}
				else if (name == "Shadow")
				{
					update[sizeUpdate]["Type"] = 9;

					update[sizeUpdate]["index"] = particles.emit[i][j].data[0].x;
					update[sizeUpdate]["time"] = particles.emit[i][j].data[0].y;
				}
				else if (name == "SpawnEnd")
				{
					update[sizeUpdate]["Type"] = 10;

					update[sizeUpdate]["namePrefab"] = particles.emit[i][j].nameData;
				}
				else if (name == "InitVelWithPoint")
				{
					generator[sizeGenerator]["Type"] = 13;
				}
			}
		}
	}

	file << std::setw(4) << js << std::endl;
}

void Cookie::Resources::Serialization::Save::SaveAIBehavior(Cookie::Gameplay::AIBehavior& aiBehavior)
{
	std::ofstream file("Assets/AIBehaviors/" + aiBehavior.name + ".AIBAsset");

	json js;

	js["Name"] = aiBehavior.name;

	for (int i = 0; i < aiBehavior.steps.size(); i++)
	{
		json& steps = js["Steps"][i];
		AIStep& step = aiBehavior.steps[i];
		steps["NumberOfWorkers"] = step.nbOfWorker;
		steps["NumberOfUnits"] = step.nbOfUnits;
		for (int j = 0; j < step.listOfBuildings.size(); j++)
			steps["ListOfBuilding"] += step.listOfBuildings[j];
	}	

	file << std::setw(4) << js << std::endl;
}

 //------------------------------------------------------------------------------------------------------------------

void Cookie::Resources::Serialization::Load::FromJson(json& js, Cookie::ECS::EntityHandler& entity)
 {
	 for (int i = 0; i < entity.livingEntities; i++)
	 {
		 json& newEntity = js["EntityHandler"][i].at("entity");
		 entity.entities[i] = (Cookie::ECS::Entity(newEntity.at("id").get<int>(), newEntity.at("signature").get<int>(), 
								newEntity.at("name").get<std::string>(), newEntity.at("namePrefab").get<std::string>()));
	 }
 }

void Cookie::Resources::Serialization::Load::FromJson(json& js, const Cookie::ECS::EntityHandler& entity,
		Cookie::ECS::ComponentHandler& component, Cookie::Resources::ResourcesManager& resourcesManager)
 {
	 int indexOfPhysic = 0;
	 int indexOfGameplay = 0;
	 int indexOfFX = 0;
	 int indexOfTransform = 0;
	 int indexOfModel = 0;
	 for (int i = 0; i < entity.livingEntities; i++)
	 {
		 if (entity.entities[i].signature & C_SIGNATURE::TRANSFORM)
		 {
			 Cookie::ECS::ComponentTransform transform;
			 json TRS = js["ComponentHandler"]["Transform"][indexOfTransform].at("localTRS");
			 TRS.at("translate").get_to(transform.pos.e);
			 
			 if (TRS.contains("rotation"))
				 TRS.at("rotation").get_to(transform.rot.e);

			 if (TRS.contains("scale"))
				TRS.at("scale").get_to(transform.scale.e);
			 
			 transform.trsHasChanged = true;
			 component.GetComponentTransform(entity.entities[i].id) = transform;
			 indexOfTransform++;
		 }
		 if (entity.entities[i].signature & C_SIGNATURE::MODEL)
		 {
			 json model = js["ComponentHandler"]["Model"][indexOfModel];
			 if (model["model"].is_string())
			 {
				 if (model["texture"]["albedo"].get<std::string>() != "NO MESH")
					 component.GetComponentModel(entity.entities[i].id).mesh = resourcesManager.meshes[(model["model"].get<std::string>())].get();
			 }
			 if (model["texture"]["albedo"].is_string())
			 {
				 if (model["texture"]["albedo"].get<std::string>() != "NO ALBEDO")
					 component.GetComponentModel(entity.entities[i].id).albedo = resourcesManager.textures2D[(model["texture"]["albedo"].get<std::string>())].get();
			 }
			 if (model["texture"]["normal"].is_string())
			 {
				 if (model["texture"]["normal"].get<std::string>() != "NO NORMAL")
					 component.GetComponentModel(entity.entities[i].id).normal = resourcesManager.textures2D[(model["texture"]["normal"].get<std::string>())].get();
			 }
			 if (model["texture"]["metallic"].is_string())
			 {
				 if (model["texture"]["metallic"].get<std::string>() != "NO METALLIC")
					 component.GetComponentModel(entity.entities[i].id).metallicRoughness = resourcesManager.textures2D[(model["texture"]["metallic"].get<std::string>())].get();
			 }
			 if (model["texture"]["icon"].is_string())
			 {
				 if (model["texture"]["icon"].get<std::string>() != "NO ICON")
					 component.GetComponentModel(entity.entities[i].id).icon = resourcesManager.icons[(model["texture"]["icon"].get<std::string>())].get();
			 }
				 
			 indexOfModel++;
		 }
		 if (entity.entities[i].signature & C_SIGNATURE::PHYSICS)
		 {
			 json physic = js["PhysicHandler"][indexOfPhysic];

			 LoadPhysic(physic, component.GetComponentPhysics(entity.entities[i].id));
			 
			 indexOfPhysic += 1;
		 }
		 if (entity.entities[i].signature & C_SIGNATURE::GAMEPLAY)
		 {
			 json gameplay = js["Gameplay"][indexOfGameplay];

			 LoadGameplay(gameplay, component.GetComponentGameplay(entity.entities[i].id), resourcesManager, true);

			 indexOfGameplay += 1;
		 }
		 if (entity.entities[i].signature & C_SIGNATURE::FX)
		 {
			 json& fx = js["FX"][indexOfFX];

			 if (fx.contains("CGPAttack"))
			 {
				 Gameplay::CGPAttack& game = component.GetComponentGameplay(entity.entities[i].id).componentAttack;
				 std::string name = fx["CGPAttack"]["sfxAttack"].get<std::string>();
				 if (resourcesManager.sounds.find(name) != resourcesManager.sounds.end())
					 game.sfxAttack = resourcesManager.sounds[name].get();
				 name = fx["CGPAttack"]["vfxAttack"].get<std::string>();
				 if (resourcesManager.particles.find(name) != resourcesManager.particles.end())
					 game.vfxAttack = resourcesManager.particles[name].get();
			 }

			 if (fx.contains("CGPLive"))
			 {
				 Gameplay::CGPLive& game = component.GetComponentGameplay(entity.entities[i].id).componentLive;
				 std::string name = fx["CGPLive"]["sfxDeath"].get<std::string>();
				 if (resourcesManager.sounds.find(name) != resourcesManager.sounds.end())
					 game.sfxDeath = resourcesManager.sounds[name].get();
				 name = fx["CGPLive"]["vfxDeath"].get<std::string>();
				 if (resourcesManager.particles.find(name) != resourcesManager.particles.end())
					 game.vfxDeath = resourcesManager.particles[name].get();

				 name = fx["CGPLive"]["sfxHit"].get<std::string>();
				 if (resourcesManager.sounds.find(name) != resourcesManager.sounds.end())
					 game.sfxHit = resourcesManager.sounds[name].get();
				 name = fx["CGPLive"]["vfxHit"].get<std::string>();
				 if (resourcesManager.particles.find(name) != resourcesManager.particles.end())
					 game.vfxHit = resourcesManager.particles[name].get();
			 }
		 }
	 }
 }

void Cookie::Resources::Serialization::Load::LoadScene(const char* filepath, Game& game)
 {
	 std::unique_ptr<Resources::Scene> newScene = std::make_unique<Resources::Scene>();
	 std::ifstream file(filepath);

	 if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof())
	 {
		 std::cout << "DON'T FIND THE FILE\n";
		 newScene.get()->camera = std::make_unique<Render::GameCam>();
		 newScene.get()->map.trs.ComputeTRS();
		 newScene.get()->map.InitTiles();
		 game.scene = std::move(newScene);
		 return;
	 }

	 json js;
	 file >> js;
	 
	 if (js.contains("Name"))
		js["Name"].get_to(newScene->name);
	 
	 if (js.contains("SkyboxName"))
	 {
		 newScene->skyBox = game.resources.skyboxes[js["SkyboxName"].get<std::string>()].get();
	 }

	 if (js.contains("Camera"))
	 {
		 json& cam = js["Camera"];
		 newScene.get()->camera = std::make_unique<Render::GameCam>();
		 Cookie::Render::Camera* camera = newScene.get()->camera.get();

		 camera->camNear = cam["camNear"].get<float>();
		 camera->camFar = cam["camFar"].get<float>();
		 camera->fov = cam["fov"].get<float>();
		 camera->aspectRatio = cam["aspectRatio"].get<float>();

		 cam["windowOffset"].get_to(camera->windowOffset.e);
		 cam["pos"].get_to(camera->pos.e);
		 cam["rot"].get_to(camera->rot.e);
	 }

	 if (js.contains("Map"))
	 {
		 Cookie::Resources::Scene* scene = newScene.get();
		 Cookie::Resources::ResourcesManager& resources = game.resources;

		 js["Map"]["tilesNb"].get_to(scene->map.tilesNb.e);
		 js["Map"]["tilesSize"].get_to(scene->map.tilesSize.e);

		 js["Map"]["trs"]["pos"].get_to(scene->map.trs.pos.e);
		 js["Map"]["trs"]["rot"].get_to(scene->map.trs.rot.e);
		 js["Map"]["trs"]["scale"].get_to(scene->map.trs.scale.e);
		 scene->map.trs.ComputeTRS();

		 if (js["Map"]["model"]["texture"].contains("albedo"))
			 scene->map.model.albedo = resources.textures2D[js["Map"]["model"]["texture"]["albedo"].get<std::string>()].get();
		 if (js["Map"]["model"]["texture"].contains("normal"))
			 scene->map.model.normal = resources.textures2D[js["Map"]["model"]["texture"]["normal"].get<std::string>()].get();
		 if (js["Map"]["model"]["texture"].contains("metallic"))
			 scene->map.model.metallicRoughness = resources.textures2D[js["Map"]["model"]["texture"]["metallic"].get<std::string>()].get();

		 LoadPhysic(js["Map"]["physic"], scene->map.physic);
		 scene->map.InitTiles();
	 }

	 if (js.contains("EntityHandler"))
	 {
		 //entities
		 {
			 newScene->entityHandler.livingEntities = js["EntityHandler"].size();
			 Cookie::Resources::Serialization::Load::FromJson(js, newScene->entityHandler);
		 }

		 //component
		 {
			 Cookie::Resources::Serialization::Load::FromJson(js, newScene->entityHandler, newScene->componentHandler, game.resources);
		 }
	 }

	 if (js.contains("LightsArray"))
	 {
		 json light = js["LightsArray"];

		 newScene->lights.useDir = light["useDir"].get<bool>();
		 newScene->lights.usedPoints = light["usedPoints"].get<unsigned int>();

		 if (light.contains("DirLight"))
		 {
			 json dir = light["DirLight"];
			 dir["dir"].get_to(newScene->lights.dirLight.dir.e);
			 dir["color"].get_to(newScene->lights.dirLight.color.e);
			 newScene->lights.dirLight.castShadow = dir["castShadow"].get<bool>();
		 }

		 if (light.contains("PointLights"))
		 {
			 json point = light["PointLights"];
			 for (int i = 0; i < point.size(); i++)
			 {
				 point[i]["pos"].get_to(newScene->lights.pointLights[i].pos.e);
				 point[i]["color"].get_to(newScene->lights.pointLights[i].color.e);
				 newScene->lights.pointLights[i].radius = point[i]["radius"].get<float>();
			 }
		 }
	 }

	 newScene->filepath = filepath;

	 ArmyHandler& handler = newScene.get()->armyHandler;
	 //Army
	 if (js.contains("ArmyHandler"))
	 {
		 json& armyHandler = js["ArmyHandler"];
		 for (int i = 0; i < armyHandler.size(); i++)
		 {
			 Income& income = handler.armies[handler.livingArmies].income;
			 handler.AddArmy((E_ARMY_NAME)(armyHandler[i]["name"].get<int>()));
			 income.primary = armyHandler[i]["Income"]["incomePrimary"].get<float>();
			 income.secondary = armyHandler[i]["Income"]["incomeSecondary"].get<float>();
			 income.supplyCurrent = armyHandler[i]["Income"]["supplyCurrent"].get<float>();
			 income.supplyMax = armyHandler[i]["Income"]["supplyMax"].get<float>();
		 }
	 }

	 //ArmiesCoordinator
	 if (js.contains("ArmiesCoordinator"))
	 {
		 for (int i = 0; i < js["ArmiesCoordinator"].size(); i++)
		 {
			 json& armyCoordinator = js["ArmiesCoordinator"][i];
			 handler.AddArmyCoordinator((E_ARMY_NAME)(armyCoordinator["armyName"].get<int>()), 
				 game.resources.aiBehaviors[armyCoordinator["aiBehaviorName"].get<std::string>()].get());

			 ArmyCoordinator& coordinator = handler.armiesCoordinator[i];

			 coordinator.nbOfWorkerInProduction = armyCoordinator["nbOfWorkerInProduction"].get<int>();
			 coordinator.nbOfBuildingInProduction = armyCoordinator["nbOfBuildingInProduction"].get<int>();
			 coordinator.nbOfUnitInProduction = armyCoordinator["nbOfUnitInProduction"].get<int>();

			 coordinator.canAttack = armyCoordinator["canAttack"].get<bool>();
			 coordinator.canDefend = armyCoordinator["canDefend"].get<bool>();

			 coordinator.currentStepIndex = armyCoordinator["currentStepIndex"].get<int>();

			 //StepGoals
			 {
				 coordinator.stepGoals.nbOfWorker = armyCoordinator["nbOfWorker"].get<int>();
				 coordinator.stepGoals.nbOfUnits = armyCoordinator["nbOfUnits"].get<int>();
				 for (int j = 0; j < armyCoordinator["ListOfBuilding"].size(); j++)
					 coordinator.stepGoals.listOfBuildings.push_back(armyCoordinator["ListOfBuilding"][j].get<std::string>());
			 }
		 }
	 }

	 //Load With Prefab
	 int indexGameplay = 0;
	 Map& map = newScene.get()->map;
	 for (int i = 0; i < newScene.get()->entityHandler.livingEntities; i++)
	 {
		 Entity& current = newScene.get()->entityHandler.entities[i];

		 if (current.signature & C_SIGNATURE::TRANSFORM)
			 newScene.get()->componentHandler.GetComponentTransform(current.id).modelptr = &newScene.get()->componentHandler.GetComponentModel(current.id);


		 if (current.signature & C_SIGNATURE::GAMEPLAY)
		 {
			 ComponentGameplay& gameComp = newScene.get()->componentHandler.GetComponentGameplay(current.id);
			 ComponentTransform* trsPtr = &newScene.get()->componentHandler.GetComponentTransform(current.id);
			 gameComp.trs = trsPtr;
			 gameComp.componentLive.trs = trsPtr;
			 gameComp.componentAttack.trs = trsPtr;
			 gameComp.componentMove.trs = trsPtr;
			 gameComp.componentProducer.trs = trsPtr;

			 if (gameComp.teamName != E_ARMY_NAME::E_DEFAULT_NAME)
				handler.AddElementToArmy(&gameComp);

			 if (gameComp.signatureGameplay & CGP_SIGNATURE::PRODUCER)
			 {
				 json& temp = js["Gameplay"][indexGameplay]["CGPProducer"]["OccupiedTiles"];
				 for (int j = 0; j < temp.size(); j++)
				 {
					 gameComp.componentProducer.occupiedTiles.push_back(&(map.tiles[temp[j].get<int>()]));
					 gameComp.componentProducer.occupiedTiles[j]->isObstacle = true;
				 }
			 }

			 indexGameplay++; 
		 }
	 }

	 if (js.contains("UIScene"))
	 {
		 if (js["UIScene"].size() != 0)
		 {
			std::vector<Cookie::UI::UIscene::GameWindowInfo> list;
			for (int i = 0; i < js["UIScene"].size(); i++)
			{
				Cookie::UI::UIscene::GameWindowInfo info;
				json ui = js["UIScene"][i];
				info.ID = ui["ID"].get<int>();
				info.xPos = ui["xPos"].get<int>();
				info.yPos = ui["yPos"].get<int>();
				info.width = ui["width"].get<int>();
				info.height = ui["height"].get<int>();
				list.push_back(info);
			}
			newScene->uiScene.LoadLayout(list, game, *newScene);
		 }
	 }

	 game.scene = std::move(newScene);
 }

void Cookie::Resources::Serialization::Load::LoadAllPrefabs(Cookie::Resources::ResourcesManager& resourcesManager)
 {
	 std::vector<std::string> filesPath;
	 for (const fs::directory_entry& path : fs::directory_iterator("Assets/Prefabs"))
	 {
		 if (path.path().string().find(".PAsset") != std::string::npos)
			filesPath.push_back(path.path().string());
	 }

	 for (unsigned int i = 0; i < filesPath.size(); i++)
	 {
		 std::string& iFile = filesPath.at(i);
		 std::replace(iFile.begin(), iFile.end(), '\\', '/');
	 }

	 for (int i = 0; i < filesPath.size(); i++)
	 {
		 std::ifstream file(filesPath[i]);

		 if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof())
		 {
			 std::cout << "DON'T FIND THE FILE\n";
			 continue;
		 }

		 json js;
		 file >> js;

		 Cookie::Resources::Prefab newPrefab;

		 if (js.contains("Name"))
			 newPrefab.name = js["Name"];

		 if (js.contains("Model"))
		 {
			 if (js["Model"]["Mesh"].is_string())
			 {
				 if (resourcesManager.meshes.find(js["Model"]["Mesh"]) != resourcesManager.meshes.end())
					newPrefab.model.mesh = resourcesManager.meshes[js["Model"]["Mesh"]].get();
				 else
				 {
					 std::string name = js["Model"]["Mesh"];
					 CDebug.Error(std::string("Mesh " + name + " not found!").c_str());
				 }
				 
			 }
			 if (js["Model"]["Texture"]["albedo"].is_string())
			 {
				 if (resourcesManager.textures2D.find(js["Model"]["Texture"]["albedo"]) != resourcesManager.textures2D.end())
					newPrefab.model.albedo = resourcesManager.textures2D[js["Model"]["Texture"]["albedo"]].get();
				 else
				 {
					 std::string name = js["Model"]["Texture"]["albedo"];
					 CDebug.Error(std::string("Albedo " + name + " not found!").c_str());
				 }
			 }
			 if (js["Model"]["Texture"]["normal"].is_string())
			 {
				 if (resourcesManager.textures2D.find(js["Model"]["Texture"]["normal"]) != resourcesManager.textures2D.end())
					 newPrefab.model.normal = resourcesManager.textures2D[js["Model"]["Texture"]["normal"]].get();
				 else
				 {
					 std::string name = js["Model"]["Texture"]["normal"];
					 CDebug.Error(std::string("Normal " + name + " not found!").c_str());
				 }
			 }
			 if (js["Model"]["Texture"]["metallic"].is_string())
			 {
				 if (resourcesManager.textures2D.find(js["Model"]["Texture"]["metallic"]) != resourcesManager.textures2D.end())
					 newPrefab.model.metallicRoughness = resourcesManager.textures2D[js["Model"]["Texture"]["metallic"]].get();
				 else
				 {
					 std::string name = js["Model"]["Texture"]["metallic"];
					 CDebug.Error(std::string("Metallic " + name + " not found!").c_str());
				 }
			 }
			 if (js["Model"]["Texture"]["icon"].is_string())
			 {
				 if (resourcesManager.icons.find(js["Model"]["Texture"]["icon"]) != resourcesManager.icons.end())
					 newPrefab.model.icon = resourcesManager.icons[js["Model"]["Texture"]["icon"]].get();
				 else
				 {
					 std::string name = js["Model"]["Texture"]["icon"];
					 CDebug.Error(std::string("icon " + name + " not found!").c_str());
				 }
			 }
		 }

		 if (js.contains("Transform"))
		 {
			 if (js["Transform"]["Rotation"].is_array())
				 js["Transform"]["Rotation"].get_to(newPrefab.transform.rot.e); 
			 if (js["Transform"]["Scale"].is_array())
				 js["Transform"]["Scale"].get_to(newPrefab.transform.scale.e);
			 
			 newPrefab.transform.ComputeTRS();
		 }

		 if (js.contains("Signature"))
			 newPrefab.signature = js["Signature"];
		 
		 if (js.contains("physicTRS"))
		 {
			 json physic = js;

			 LoadPhysic(physic, newPrefab.physics);
		 }

		 if (js.contains("Gameplay"))
		 {
			 json gameplay = js["Gameplay"];

			 LoadGameplay(gameplay, newPrefab.gameplay, resourcesManager);
		 }
		 if (js.contains("FX"))
		 {
			 json& fx = js["FX"];

			 if (fx.contains("CGPAttack"))
			 {
				 Gameplay::CGPAttack& game = newPrefab.gameplay.componentAttack;
				 std::string name = fx["CGPAttack"]["sfxAttack"].get<std::string>();
				 if (resourcesManager.sounds.find(name) != resourcesManager.sounds.end())
					 game.sfxAttack = resourcesManager.sounds[name].get();
				 name = fx["CGPAttack"]["vfxAttack"].get<std::string>();
				 if (resourcesManager.particles.find(name) != resourcesManager.particles.end())
					 game.vfxAttack = resourcesManager.particles[name].get();
			 }

			 if (fx.contains("CGPLive"))
			 {
				 Gameplay::CGPLive& game = newPrefab.gameplay.componentLive;
				 std::string name = fx["CGPLive"]["sfxDeath"].get<std::string>();
				 if (resourcesManager.sounds.find(name) != resourcesManager.sounds.end())
					 game.sfxDeath = resourcesManager.sounds[name].get();
				 name = fx["CGPLive"]["vfxDeath"].get<std::string>();
				 if (resourcesManager.particles.find(name) != resourcesManager.particles.end())
					 game.vfxDeath = resourcesManager.particles[name].get();

				 name = fx["CGPLive"]["sfxHit"].get<std::string>();
				 if (resourcesManager.sounds.find(name) != resourcesManager.sounds.end())
					 game.sfxHit = resourcesManager.sounds[name].get();
				 name = fx["CGPLive"]["vfxHit"].get<std::string>();
				 if (resourcesManager.particles.find(name) != resourcesManager.particles.end())
					 game.vfxHit = resourcesManager.particles[name].get();
			 }
		 }

		 newPrefab.filepath = filesPath[i];

		 resourcesManager.prefabs[newPrefab.name] = std::make_unique<Prefab>(newPrefab);

		 file.close();
	 }

	 for (std::unordered_map<std::string, std::unique_ptr<Prefab>>::iterator pref =
		 resourcesManager.prefabs.begin(); pref != resourcesManager.prefabs.end(); ++pref)
	 {
		 Prefab* prefab = pref->second.get();
		 if (prefab->gameplay.signatureGameplay & CGP_SIGNATURE::PRODUCER)
		 {
			 for (int i = 0; i < prefab->gameplay.componentProducer.possibleUnitsAtLoad.size(); i++)
			 {
				 std::string name = prefab->gameplay.componentProducer.possibleUnitsAtLoad[i];
				 prefab->gameplay.componentProducer.possibleUnits.push_back(resourcesManager.prefabs[name].get());
			 }
		 }
		 if (prefab->gameplay.signatureGameplay & CGP_SIGNATURE::WORKER)
		 {
			 for (int i = 0; i < prefab->gameplay.componentWorker.possibleBuildingsAtLoad.size(); i++)
			 {
				 std::string name = prefab->gameplay.componentWorker.possibleBuildingsAtLoad[i];
				 prefab->gameplay.componentWorker.possibleBuildings.push_back(resourcesManager.prefabs[name].get());
			 }
		 }
	 }
 }

void Cookie::Resources::Serialization::Load::LoadAllTextures(Cookie::Resources::ResourcesManager& resourcesManager)
 {
	 std::vector<std::string> filesPath;
	 for (const fs::directory_entry& path : fs::directory_iterator("Assets/Textures"))
	 {
		 if (path.path().string().find(".TAsset") != std::string::npos)
			 filesPath.push_back(path.path().string());
	 }

	 for (unsigned int i = 0; i < filesPath.size(); i++)
	 {
		 std::string& iFile = filesPath.at(i);
		 std::replace(iFile.begin(), iFile.end(), '\\', '/');
	 }

	 for (int i = 0; i < filesPath.size(); i++)
	 {
		 std::ifstream file(filesPath[i]);

		 if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof())
		 {
			 std::cout << "DON'T FIND THE FILE\n";
			 continue;
		 }

		 json js;
		 file >> js;

		 Cookie::Core::Math::Vec4 color;
		 std::string name;
		 if (js.contains("color"))
			 js["color"].get_to(color.e);
		 else
			 CDebug.Error("No color defined");

		 if (js.contains("name"))
			js["name"].get_to(name);
		 else 
			 CDebug.Error("No name defined");

		 resourcesManager.textures2D[name] = std::make_unique<Texture>(name, color);
	 }
 }

void Cookie::Resources::Serialization::Load::LoadPhysic(json& physic, Cookie::ECS::ComponentPhysics& physicsComp)
{
	if (physic.contains("physicTRS"))
	{
		if (physic["physicTRS"].contains("position") && physic["physicTRS"].contains("quaternion"))
		{
			json& pTRS = physic["physicTRS"].at("position");
			json& qTRS = physic["physicTRS"].at("quaternion");

			::reactphysics3d::Vector3 vecTemp(pTRS[0].get<float>(), pTRS[1].get<float>(), pTRS[1].get<float>());
			::reactphysics3d::Quaternion quatTemp(qTRS[1].get<float>(), qTRS[2].get<float>(), qTRS[3].get<float>(), qTRS[0].get<float>());
			physicsComp.physTransform = ::reactphysics3d::Transform({ vecTemp, quatTemp });
		}
	}
	else
		CDebug.Error("No position and/or quaternion in physic");
	
	//Rigidbody
	if (physic.contains("Rigidbody"))
	{
		json& rigid = physic["Rigidbody"];
		::reactphysics3d::RigidBody* actRigidBody = physicsComp.physBody;

		if (rigid.contains("type"))
		{
			if (rigid["type"].get<int>() == 0)
				actRigidBody->setType(::reactphysics3d::BodyType::STATIC);
			else if (rigid["type"].get<int>() == 1)
				actRigidBody->setType(::reactphysics3d::BodyType::KINEMATIC);
			else if (rigid["type"].get<int>() == 2)
				actRigidBody->setType(::reactphysics3d::BodyType::DYNAMIC);
		}
		else
			CDebug.Error("No type define");

		if (rigid.contains("active"))
			actRigidBody->setIsActive(rigid["active"].get<bool>());
		else
			CDebug.Error("No type defined");

		if (rigid.contains("allowedToSleep"))
			actRigidBody->setIsAllowedToSleep(rigid["allowedToSleep"].get<bool>());
		else
			CDebug.Error("Don't know if allowed to sleep");

		if (rigid.contains("angularDamping"))
			actRigidBody->setAngularDamping(rigid["angularDamping"].get<float>());
		else
			CDebug.Error("No angular damping defined");

		if (rigid.contains("gravityEnabled"))
			actRigidBody->enableGravity(rigid["gravityEnabled"].get<bool>());
		else
			CDebug.Error("Don't know if gravity is enabled");

		if (rigid.contains("linearDamping"))
			actRigidBody->setLinearDamping(rigid["linearDamping"].get<float>());
		else
			CDebug.Error("No linear damping defined");

		if (rigid.contains("mass"))
			actRigidBody->setMass(rigid["mass"].get<float>());
		else
			CDebug.Error("No mass defined");
	}
	else
		CDebug.Error("No rigidbody");

	if (physic.contains("Colliders"))
	{
		for (int j = 0; j < physic["Colliders"].size(); j++)
		{
			json colliders = physic["Colliders"][j];

			Cookie::Core::Math::Vec3 pos(0, 0, 0);
			Cookie::Core::Math::Vec3 rot(0, 0, 0);
			if (colliders.contains("transform"))
			{
				if (colliders["transform"].contains("pos"))
					colliders["transform"]["pos"].get_to(pos.e);
				if (colliders["transform"].contains("quaternion"))
				{
					Cookie::Core::Math::Vec4 quat;
					colliders["transform"]["quaternion"].get_to(quat.e);
					rot = Cookie::Core::Math::Quat::ToEulerAngle({ quat.x, quat.y, quat.z, quat.w });
				}
			}
			else
				CDebug.Error("No transform defined");

			if (colliders.contains("type"))
			{
				if (colliders["type"].get<std::string>() == "Sphere")
				{
					float radius = colliders["radius"].get<float>();
					physicsComp.AddSphereCollider(radius, pos, rot);
				}
				else if (colliders["type"].get<std::string>() == "Capsule")
				{
					float radius = colliders["radius"].get<float>();
					float height = colliders["height"].get<float>();
					physicsComp.AddCapsuleCollider(Cookie::Core::Math::Vec2{ {radius, height } }, pos, rot);
				}
				else if (colliders["type"].get<std::string>() == "Box")
				{
					Cookie::Core::Math::Vec3 halfExtents;
					colliders["HalfExtents"].get_to(halfExtents.e);
					physicsComp.AddCubeCollider(halfExtents, pos, rot);
				}
			}

			::reactphysics3d::Material& mat = physicsComp.physColliders[j]->getMaterial();
			if (colliders.contains("bounciness"))
				mat.setBounciness(colliders["bounciness"].get<float>());
			else
				CDebug.Error("No bounciness defined");

			if (colliders.contains("density"))
				mat.setMassDensity(colliders["density"].get<float>());
			else
				CDebug.Error("No density defined");

			if (colliders.contains("frictionCoeff"))
				mat.setFrictionCoefficient(colliders["frictionCoeff"].get<float>());
			else 
				CDebug.Error("No frictionCoeff defined");

			if (colliders.contains("rollingResistance"))
				mat.setRollingResistance(colliders["rollingResistance"].get<float>());
			else 
				CDebug.Error("No rollingResistance defined");
		}
	}
}

void Cookie::Resources::Serialization::Load::LoadGameplay(json& gameplay, 
				Cookie::ECS::ComponentGameplay& GPComponent, Cookie::Resources::ResourcesManager& resourcesManager, bool allPrefabLoaded)
{
	GPComponent.teamName = gameplay["TeamName"];
	GPComponent.signatureGameplay = gameplay["SignatureGameplay"];
	GPComponent.type = gameplay["Type"];

	json temp = gameplay;
	if (gameplay.contains("Cost"))
	{
		temp = gameplay["Cost"];
		GPComponent.cost.costPrimary = temp["CostPrimary"].get<float>();
		GPComponent.cost.costSecondary = temp["CostSecondary"].get<float>();
		GPComponent.cost.costSupply = temp["CostSupply"].get<float>();
		GPComponent.cost.timeToProduce = temp["TimeToProduce"].get<float>();
	}

	if (gameplay.contains("CGPLive"))
	{
		temp = gameplay["CGPLive"];
		GPComponent.componentLive.lifeCurrent = temp["Life"].get<float>();
		GPComponent.componentLive.lifeMax = temp["LifeMax"].get<float>();
		GPComponent.componentLive.armor = temp["Armor"].get<float>();
		temp["PosLife"].get_to(GPComponent.componentLive.lifeBarOffset.e);
	}

	if (gameplay.contains("CGPAttack"))
	{
		temp = gameplay["CGPAttack"];
		GPComponent.componentAttack.needToAttack = temp["NeedToAttack"].get<bool>();
		GPComponent.componentAttack.attackDamage = temp["AttackDamage"].get<float>();
		GPComponent.componentAttack.attackSpeed = temp["AttackSpeed"].get<float>();
		GPComponent.componentAttack.attackRange = temp["AttackRange"].get<float>();
	}

	if (gameplay.contains("CGPMove"))
	{
		temp = gameplay["CGPMove"];
		GPComponent.componentMove.moveSpeed = temp["MoveSpeed"].get<float>();
		GPComponent.componentMove.isFlying = temp["isFlying"].get<bool>();
	}
	
	if (gameplay.contains("CGPProducer"))
	{
		temp = gameplay["CGPProducer"];
		temp["TileSize"].get_to(GPComponent.componentProducer.tileSize.e);
		GPComponent.componentProducer.supplyGiven = temp["SupplyGiven"].get<int>();

		if (temp.contains("name"))
		{
			for (int i = 0; i < temp["name"].size(); i++)
			{
				std::string name = temp["name"][i].get<std::string>();
				if (resourcesManager.prefabs.find(name) != resourcesManager.prefabs.end() && allPrefabLoaded)
					GPComponent.componentProducer.possibleUnits.push_back(resourcesManager.prefabs[name].get());
				else
					GPComponent.componentProducer.possibleUnitsAtLoad.push_back(name);
			}
		}
	}

	if (gameplay.contains("CGPWorker"))
	{
		if (gameplay["CGPWorker"].contains("name"))
		{
			temp = gameplay["CGPWorker"]["name"];
			for (int i = 0; i < temp.size(); i++)
			{
				std::string name = temp[i].get<std::string>();
				if (resourcesManager.prefabs.find(name) != resourcesManager.prefabs.end() && allPrefabLoaded)
					GPComponent.componentWorker.possibleBuildings.push_back(resourcesManager.prefabs[name].get());
				else
					GPComponent.componentWorker.possibleBuildingsAtLoad.push_back(name);
			}
		}
	}

	if (gameplay.contains("CGPResource"))
	{
		temp = gameplay["CGPResource"];
		GPComponent.componentResource.resourceReserve = temp["resourcesReserve"].get<int>();
		GPComponent.componentResource.isPrimary = temp["isPrimary"].get<bool>();
	}
}

void Cookie::Resources::Serialization::Load::LoadVolumAndModeMusic(std::string path, Sound* const& sound)
{
	std::ifstream file(path);

	if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof())
	{
		std::cout << "DON'T FIND THE FILE\n";
		return;
	}

	json js;
	file >> js;

	Cookie::Resources::SoundManager::SetVolume(sound, js["Volume"].get<float>());

	Cookie::Resources::SoundManager::SetMode(sound, js["Mode"].get<int>());
}

void Cookie::Resources::Serialization::Load::LoadAllParticles(Cookie::Resources::ResourcesManager& resourcesManager)
{
	std::vector<std::string> filesPath;
	for (const fs::directory_entry& path : fs::directory_iterator("Assets/VFX"))
	{
		if (path.path().string().find(".PSAsset") != std::string::npos)
			filesPath.push_back(path.path().string());
	}

	for (unsigned int i = 0; i < filesPath.size(); i++)
	{
		std::string& iFile = filesPath.at(i);
		std::replace(iFile.begin(), iFile.end(), '\\', '/');
	}

	for (int i = 0; i < filesPath.size(); i++)
	{
		std::ifstream file(filesPath[i]);

		if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof())
		{
			std::cout << "DON'T FIND THE FILE\n";
			continue;
		}

		json js;
		file >> js;

		std::unique_ptr<ParticlesPrefab> prefParticles = std::make_unique<ParticlesPrefab>();
		ParticlesPrefab& pref = *prefParticles.get();

		pref.name = js["Name"].get<std::string>();

		//Data
		if (js.contains("Data"))
		{
			pref.data.resize(js["Data"].size());
			for (int i = 0; i < js["Data"].size(); i++)
			{
				json data = js["Data"][i];
				std::string name = data["Mesh"].get<std::string>();
				if (name != "NO MESH")
				{
					if (resourcesManager.meshes.find(name) != resourcesManager.meshes.end())
						pref.data[i].mesh = resourcesManager.meshes[name].get();
				}
				else
					pref.data[i].mesh = nullptr;
				name = data["Texture"].get<std::string>();
				if (name != "NO TEXTURE")
				{
					if (resourcesManager.textures2D.find(name) != resourcesManager.textures2D.end())
						pref.data[i].texture = resourcesManager.textures2D[name].get();
				}
				else
					pref.data[i].texture = nullptr;
				pref.data[i].countAlive = data["CountAlive"].get<int>();
				pref.data[i].countFrame = data["CountFrame"].get<int>();
				pref.data[i].size = data["Size"].get<int>();
				pref.data[i].isBillboard = data["IsBillBoard"].get<bool>();
			}
		}

		//Emitter
		if (js.contains("Emitter"))
		{
			json emitterJson = js["Emitter"];
			pref.emitter.resize(emitterJson.size());
			pref.emit.resize(emitterJson.size());
			for (int i = 0; i < emitterJson.size(); i++)
			{
				if (emitterJson[i]["Generators"].is_array())
				{
					json gen = emitterJson[i]["Generators"];
					for (int j = 0; j < gen.size(); j++)
					{
						TYPEGEN typeGen = (TYPEGEN)gen[j]["Type"].get<int>();
						Particles::ParticlesEmitter& emitter = pref.emitter[i];
						switch (typeGen)
						{
						case (TYPEGEN::POINTPOSITIONGEN): {
							Particles::emit emit;
							emit.name = "PointPositionGen";
							gen[j]["pos"].get_to(emit.data[0].e);
							pref.emit[i].push_back(emit);
							emitter.componentAdd += COMPONENTADD::PPGEN;
							break;
						}
						case (TYPEGEN::BOXPOSITIONGEN): {
							Particles::emit emit;
							emit.name = "BoxPositionGen";
							gen[j]["pos"].get_to(emit.data[0].e);
							gen[j]["sizeBox"].get_to(emit.data[1].e);
							pref.emit[i].push_back(emit);
							emitter.componentAdd += COMPONENTADD::BPGEN;
							break;
						}
						case (TYPEGEN::CIRCLEPOSITIONGEN): {
							Particles::emit emit;
							emit.name = "CirclePositionGen";
							gen[j]["pos"].get_to(emit.data[0].e);
							emit.data[1].x = gen[j]["radius"].get<float>();
							pref.emit[i].push_back(emit);
							emitter.componentAdd += COMPONENTADD::CPGEN;
							break;
						}
						case (TYPEGEN::SCALECONSTGEN): {
							std::shared_ptr<ScaleConstGenerate> scaleC = std::make_unique<ScaleConstGenerate>();
							gen[j]["scale"].get_to(scaleC.get()->scale.e);
							emitter.generators.push_back(std::move(scaleC));
							emitter.componentAdd += COMPONENTADD::SCGEN;
							break;
						}
						case (TYPEGEN::SCALERANDGEN): {
							std::shared_ptr<ScaleRandGenerate> scaleR = std::make_unique<ScaleRandGenerate>();
							gen[j]["scaleMin"].get_to(scaleR.get()->scaleMin.e);
							gen[j]["scaleMax"].get_to(scaleR.get()->scaleMax.e);
							emitter.generators.push_back(std::move(scaleR));
							emitter.componentAdd += COMPONENTADD::SRGEN;
							break;
						}
						case (TYPEGEN::ROTATERANDGEN): {
							std::shared_ptr<RotateRandGenerate> rot = std::make_unique<RotateRandGenerate>();
							gen[j]["rotMin"].get_to(rot.get()->rotMin.e);
							gen[j]["rotMax"].get_to(rot.get()->rotMax.e);
							emitter.generators.push_back(std::move(rot));
							emitter.componentAdd += COMPONENTADD::RRGEN;
							break;
						}
						case (TYPEGEN::VELCONSTGEN): {
							std::shared_ptr<VelocityConstGenerate> velC = std::make_unique<VelocityConstGenerate>();
							gen[j]["vel"].get_to(velC.get()->vel.e);
							emitter.generators.push_back(std::move(velC));
							emitter.componentAdd += COMPONENTADD::VCGEN;
							break;
						}
						case (TYPEGEN::VELRANDGEN): {
							std::shared_ptr<VelocityRandGenerate> velR = std::make_unique<VelocityRandGenerate>();
							gen[j]["velMin"].get_to(velR.get()->velMin.e);
							gen[j]["velMax"].get_to(velR.get()->velMax.e);
							emitter.generators.push_back(std::move(velR));
							emitter.componentAdd += COMPONENTADD::VRGEN;
							break;
						}
						case (TYPEGEN::MASSCONSTGEN): {
							std::shared_ptr<MassConstGenerate> mass = std::make_unique<MassConstGenerate>();
							mass.get()->mass = gen[j]["mass"].get<float>();
							emitter.generators.push_back(std::move(mass));
							emitter.componentAdd += COMPONENTADD::MCGEN;
							break;
						}
						case (TYPEGEN::TIMECONSTGEN): {
							std::shared_ptr<TimeConstGenerate> timeC = std::make_unique<TimeConstGenerate>();
							timeC.get()->time = gen[j]["time"].get<float>();
							emitter.generators.push_back(std::move(timeC));
							emitter.componentAdd += COMPONENTADD::TCGEN;
							break;
						}
						case (TYPEGEN::TIMERANDGEN): {
							std::shared_ptr<TimeRandGenerate> timeR = std::make_unique<TimeRandGenerate>();
							timeR.get()->timeMin = gen[j]["timeMin"].get<float>();
							timeR.get()->timeMax = gen[j]["timeMax"].get<float>();
							emitter.generators.push_back(std::move(timeR));
							emitter.componentAdd += COMPONENTADD::TRGEN;
							break;
						}
						case (TYPEGEN::COLORCONSTGEN): {
							std::shared_ptr<ColorConstGenerate> colorC = std::make_unique<ColorConstGenerate>();
							gen[j]["color"].get_to(colorC.get()->col.e);
							emitter.generators.push_back(std::move(colorC));
							emitter.componentAdd += COMPONENTADD::CCGEN;
							break;
						}
						case (TYPEGEN::COLORRANDGEN): {
							std::shared_ptr<ColorRandGenerate> colorR = std::make_unique<ColorRandGenerate>();
							gen[j]["colorMin"].get_to(colorR.get()->minCol.e);
							gen[j]["colorMax"].get_to(colorR.get()->maxCol.e);
							emitter.generators.push_back(std::move(colorR));
							emitter.componentAdd += COMPONENTADD::CRGEN;
							break;
						}
						case (TYPEGEN::INITVELWITHPOINT): {
							Particles::emit emit;
							emit.name = "InitVelWithPoint";
							pref.emit[i].push_back(emit);
							emitter.componentAdd += COMPONENTADD::IVWP;
							break;
						}
						}
					}
				}

				if (emitterJson[i]["Updates"].is_array())
				{
					json up = emitterJson[i]["Updates"];
					for (int j = 0; j < up.size(); j++)
					{
						TYPEUP typeUp = (TYPEUP)up[j]["Type"].get<int>();
						Particles::ParticlesEmitter& emitter = pref.emitter[i];
						switch (typeUp)
						{
						case (TYPEUP::UPDATEVEL): {
							std::shared_ptr<UpdateVelocity> vel = std::make_unique<UpdateVelocity>();
							emitter.updates.push_back(std::move(vel));
							emitter.componentAdd += COMPONENTADD::UPVEL;
							break;
						}
						case (TYPEUP::UPDATESCALE): {
							std::shared_ptr<UpdateScale> upScale = std::make_unique<UpdateScale>();
							up[j]["scaleEnd"].get_to(upScale.get()->scaleEnd.e);
							emitter.updates.push_back(std::move(upScale));
							emitter.componentAdd += COMPONENTADD::UPSCALE;
							break;
						}
						case (TYPEUP::UPDATEALPHA): {
							std::shared_ptr<UpdateAlpha> upAlpha = std::make_unique<UpdateAlpha>();
							upAlpha.get()->alphaEnd = up[j]["alphaEnd"].get<float>();
							emitter.updates.push_back(std::move(upAlpha));
							emitter.componentAdd += COMPONENTADD::UPALPHA;
							break;
						}
						case (TYPEUP::COLOROVERLIFE): {
							std::shared_ptr<ColorOverLife> upColor = std::make_unique<ColorOverLife>();
							up[j]["colorEnd"].get_to(upColor.get()->colorEnd.e);
							emitter.updates.push_back(std::move(upColor));
							emitter.componentAdd += COMPONENTADD::COL;
							break;
						}
						case (TYPEUP::ENABLEGRAVITY): {
							std::shared_ptr<EnabledGravity> upGravity = std::make_unique<EnabledGravity>();
							upGravity.get()->gravity = up[j]["gravity"].get<float>();
							emitter.updates.push_back(std::move(upGravity));
							emitter.componentAdd += COMPONENTADD::EG;
							break;
						}
						case (TYPEUP::UPDATETIME): {
							std::shared_ptr<UpdateTime> time = std::make_unique<UpdateTime>();
							emitter.updates.push_back(std::move(time));
							emitter.componentAdd += COMPONENTADD::UPT;
							break;
						}
						case (TYPEUP::LOOP): {
							Particles::emit emit;
							emit.name = "Loop";
							pref.emit[i].push_back(emit);
							emitter.componentAdd += COMPONENTADD::LP;
							break;
						}
						case (TYPEUP::COLLISIONWITHPLANE): {
							std::shared_ptr<CollisionWithPlane> plane = std::make_unique<CollisionWithPlane>();
							plane.get()->dis = up[j]["distance"].get<float>();
							up[j]["normal"].get_to(plane.get()->n.e);
							plane.get()->namePrefab = up[j]["namePrefab"].get<std::string>();
							emitter.updates.push_back(std::move(plane));
							emitter.componentAdd += COMPONENTADD::CWP;
							break;
						}
						case (TYPEUP::CREATEPARTICLES): {
							Particles::emit emit;
							emit.name = "CreateParticles";
							emit.data[0].x = up[j]["index"].get<int>();
							emit.data[1].x = up[j]["coeffScale"].get<float>();
							emit.data[1].y = up[j]["coeffPos"].get<float>();
							emit.data[1].z = up[j]["time"].get<float>();
							pref.emit[i].push_back(emit);
							emitter.componentAdd += COMPONENTADD::CP;
							break;
						}
						case (TYPEUP::SHADOW): {
							Particles::emit emit;
							emit.name = "Shadow";
							emit.data[0].x = up[j]["index"].get<int>();
							emit.data[0].y = up[j]["time"].get<float>();
							pref.emit[i].push_back(emit);
							emitter.componentAdd += COMPONENTADD::SH;
							break;
						}
						case (TYPEUP::SPAWNEND):{
							Particles::emit emit;
							emit.name = "SpawnEnd";
							emit.nameData = up[j]["namePrefab"].get<std::string>();
							pref.emit[i].push_back(emit);
							emitter.componentAdd += COMPONENTADD::SP;
						}
						case (TYPEUP::STOPAT):{
							Particles::emit emit;
							emit.name = "StopAt";
							pref.emit[i].push_back(emit);
							emitter.componentAdd += COMPONENTADD::SA;
						}
						}
					}
				}

			}
		}

		resourcesManager.particles[pref.name] = std::move(prefParticles);
	}
}

void Cookie::Resources::Serialization::Load::LoadAllAIBehaviors(Cookie::Resources::ResourcesManager& resourcesManager)
{
	std::vector<std::string> filesPath;
	for (const fs::directory_entry& path : fs::directory_iterator("Assets/AIBehaviors"))
	{
		if (path.path().string().find(".AIBAsset") != std::string::npos)
			filesPath.push_back(path.path().string());
	}

	for (unsigned int i = 0; i < filesPath.size(); i++)
	{
		std::string& iFile = filesPath.at(i);
		std::replace(iFile.begin(), iFile.end(), '\\', '/');
	}

	for (int i = 0; i < filesPath.size(); i++)
	{
		std::ifstream file(filesPath[i]);

		if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof())
		{
			std::cout << "DON'T FIND THE FILE\n";
			continue;
		}

		json js;
		file >> js;

		std::unique_ptr<AIBehavior> aiBehavior = std::make_unique<AIBehavior>();
		AIBehavior* aiB = aiBehavior.get();
		
		aiB->name = js["Name"].get<std::string>();

		if (js.contains("Steps"))
		{
			for (int i = 0; i < js["Steps"].size(); i++)
			{
				json& currentStep = js["Steps"][i];
				aiBehavior->steps.push_back(AIStep());
				AIStep& step = aiBehavior->steps[i];
				step.nbOfWorker = currentStep["NumberOfWorkers"].get<int>();
				step.nbOfUnits = currentStep["NumberOfUnits"].get<int>();
				for (int j = 0; j < currentStep["ListOfBuilding"].size(); j++)
					step.listOfBuildings.push_back(currentStep["ListOfBuilding"][j].get<std::string>());
			}
		}

		resourcesManager.aiBehaviors[aiB->name] = std::move(aiBehavior);
	}
}
