#include "Core/Primitives.hpp"
#include "Physics/PhysicsHandle.hpp"
#include "Resources/Scene.hpp"
#include "Game.hpp"
#include "ECS/ComponentGameplay.hpp"
#include "Serialization.hpp"

//#include "SoundManager.hpp"

#include "UIcore.hpp"

using namespace Cookie;
using namespace Cookie::Core::Math;
using namespace Cookie::ECS;
using namespace Cookie::Gameplay;
using namespace Cookie::Resources::Particles;
using namespace rp3d;

constexpr int miniMapResolution = 512;

/*================== CONSTRUCTORS/DESTRUCTORS ==================*/

Game::Game(bool windowed):
    renderer{windowed},
    frameBuffer{renderer.window.width,renderer.window.height },
    miniMapBuffer{miniMapResolution, miniMapResolution},
    uiMenu{resources}
{
    Physics::PhysicsHandle::Init();
    Core::UIcore::FinishInit(renderer);
    renderer.drawData.Init(*this);

    resources.Load(renderer);

    //Resources::SoundManager::InitSystem();
    //Resources::SoundManager::LoadAllMusic(resources);

    //Load all Textures we have create in texture editor
    Resources::Serialization::Load::LoadAllTextures(resources);

    Resources::Serialization::Load::LoadAllParticles(resources);

    //Load all prefabs in folder Prefabs
    Resources::Serialization::Load::LoadAllPrefabs(resources);

    particlesHandler.particlesPrefab = &resources.particles;

    Resources::Serialization::Load::LoadAllAIBehaviors(resources);


    renderer.miniMapPass.CreateDepth(miniMapResolution, miniMapResolution);

    uiMenu.GiveQuitFunction([&game=*this]{glfwSetWindowShouldClose(game.renderer.window.window, true);});
}

Game::~Game()
{
    Physics::PhysicsHandle::Terminate();
}


void Game::Start()
{
    uiMenu.SetMenuOpened();
}

/*================== LOOP ==================*/

void Game::Loop()
{
    Start();

    Resources::Serialization::Load::LoadScene("Assets/Save/Default.CAsset", *this);
    SetScene();

    while (!glfwWindowShouldClose(renderer.window.window))
    {
        TryResizeWindow();

        Update();

        renderer.Render();
    }
}

void Game::Update()
{
    // Present frame
    glfwPollEvents();

    //physSim.Update();
    //coordinator.ApplySystemPhysics(physSim.factor);

    if (!uiMenu.menuState)
    {
        renderer.Clear();
        renderer.ClearFrameBuffer(frameBuffer);
        renderer.ClearFrameBuffer(miniMapBuffer);

        if (!isPaused)
        {
            scene->camera->Update();
            particlesHandler.Update();
            coordinator.ApplyComputeTrs();

            CalculateMousePosInWorld();
            HandleGameplayInputs();
            ECSCalls();
        }

        renderer.Draw(scene->camera.get(), frameBuffer);
        particlesHandler.Draw(*scene->camera.get());
        DisplayLife();
        renderer.DrawMiniMap(miniMapBuffer);
    }
    
    //Resources::SoundManager::UpdateFMODFor3DMusic(*scene->camera.get());

    renderer.SetBackBuffer();
}

void Game::CalculateMousePosInWorld()
{
    const Render::Camera& cam = *scene->camera;
    Core::Math::Vec3 fwdRay = cam.pos + cam.MouseToWorldDir() * cam.camFar;
    rp3d::Ray ray({ cam.pos.x,cam.pos.y,cam.pos.z }, { fwdRay.x,fwdRay.y,fwdRay.z });
    RaycastInfo raycastInfo;

    //if raycast hit
    if (scene->map.physic.physBody->raycast(ray, raycastInfo))
        playerData.mousePosInWorld = {raycastInfo.worldPoint.x, raycastInfo.worldPoint.y + 0.1f, raycastInfo.worldPoint.z};
}
void Game::HandleGameplayInputs()
{

    if (!ImGui::GetIO().KeysDownDuration[GLFW_KEY_N])
    {
        ECS::Entity* newEntity = coordinator.AddEntity(resources.prefabs["Base"].get(), E_ARMY_NAME::E_PLAYER);

        ComponentTransform& trs = coordinator.componentHandler->GetComponentTransform(newEntity->id);
        CGPProducer& producer = coordinator.componentHandler->GetComponentGameplay(newEntity->id).componentProducer;

        float unscaledOffsetFromMap = std::abs(coordinator.componentHandler->GetComponentModel(newEntity->id).mesh->AABBMin.y);
        trs.pos = scene->map.GetCenterOfBuilding(playerData.mousePosInWorld, producer.tileSize) + Vec3{ 0, trs.scale.y * unscaledOffsetFromMap, 0 };	
        
        Vec3 posTopLeft = trs.pos - trs.scale / 2;
        scene->map.FillOccupiedTiles(scene->map.GetTileIndex(posTopLeft), producer.tileSize, producer.occupiedTiles);
    }
    if (!ImGui::GetIO().KeysDownDuration[GLFW_KEY_B])
    {
        ECS::Entity* newEntity = coordinator.AddEntity(resources.prefabs["Base"].get(), E_ARMY_NAME::E_AI1);

        ComponentTransform& trs = coordinator.componentHandler->GetComponentTransform(newEntity->id);
        CGPProducer& producer = coordinator.componentHandler->GetComponentGameplay(newEntity->id).componentProducer;

        float unscaledOffsetFromMap = std::abs(coordinator.componentHandler->GetComponentModel(newEntity->id).mesh->AABBMin.y);
        trs.pos = scene->map.GetCenterOfBuilding(playerData.mousePosInWorld, producer.tileSize) + Vec3{ 0, trs.scale.y * unscaledOffsetFromMap, 0 };

        Vec3 posTopLeft = trs.pos - trs.scale / 2;
        scene->map.FillOccupiedTiles(scene->map.GetTileIndex(posTopLeft), producer.tileSize, producer.occupiedTiles);
    }
    if (!ImGui::GetIO().KeysDownDuration[GLFW_KEY_V])
    {
        ECS::Entity* newEntity = coordinator.AddEntity(resources.prefabs["ResourcePrimary"].get(), E_ARMY_NAME::E_DEFAULT_NAME);

        ComponentTransform& trs = coordinator.componentHandler->GetComponentTransform(newEntity->id);

        float unscaledOffsetFromMap = std::abs(coordinator.componentHandler->GetComponentModel(newEntity->id).mesh->AABBMin.y);
        Vec2 tileSize {{1, 1}};
        trs.pos = scene->map.GetCenterOfBuilding(playerData.mousePosInWorld, tileSize) + Vec3{ 0, trs.scale.y * unscaledOffsetFromMap, 0 };
    }
    if (!ImGui::GetIO().KeysDownDuration[GLFW_KEY_C])
    {
        ECS::Entity* newEntity = coordinator.AddEntity(resources.prefabs["ResourceSecondary"].get(), E_ARMY_NAME::E_DEFAULT_NAME);

        ComponentTransform& trs = coordinator.componentHandler->GetComponentTransform(newEntity->id);

        float unscaledOffsetFromMap = std::abs(coordinator.componentHandler->GetComponentModel(newEntity->id).mesh->AABBMin.y);
        Vec2 tileSize{ {2, 2} };
        trs.pos = scene->map.GetCenterOfBuilding(playerData.mousePosInWorld, tileSize) + Vec3{ 0, trs.scale.y * unscaledOffsetFromMap, 0 };
    }

    if (ImGui::GetIO().KeysDown[GLFW_KEY_SPACE])
        InputFocusCam();

    if (playerData.buildingToBuild)
    {
        CheckIfBuildingValid();

        if (!ImGui::GetIO().MouseDownDuration[0] && playerData.isBuildingValid)
            InputValidateBuilding();
        if (!ImGui::GetIO().MouseDownDuration[1])
            InputCancelBuilding();
    }
    else
    {
        if (ImGui::GetIO().MouseClicked[0] && !scene->uiScene.IsHovered())
            InputStartSelectionQuad();

        if (ImGui::GetIO().MouseReleased[0] && playerData.makingASelectionQuad)
            InputEndSelectionQuad();

        if (!ImGui::GetIO().MouseDownDuration[1])
        {
            ParticlesHandler::CreateParticlesWithPrefab(playerData.mousePosInWorld, resources.particles["Target_Mouse_Green"].get());

            //CGPMove
            InputMoveSelected();
            
            //CGPProducer
            InputSetNewEntityDestination();

            //CGPWorker
            InputSetResourceToWorkers();
        }


    }
}

void Game::InputFocusCam()
{
    if (coordinator.selectedEntities.empty())
        scene->camera->pos = { 0, 0, 0 };
    else
        scene->camera->pos = coordinator.componentHandler->GetComponentTransform(coordinator.selectedEntities[0]->id).pos;

    scene->camera->pos += {0, 20, 10};
    scene->camera->ForceUpdate();
}
void Game::CheckIfBuildingValid()
{
    playerData.buildingPos = scene->map.GetCenterOfBuilding(playerData.mousePosInWorld, playerData.buildingToBuild->tileSize);
    playerData.buildingPos.y += playerData.workerWhoBuild->possibleBuildings[playerData.indexOfBuildingInWorker]->transform.scale.y * std::abs(playerData.workerWhoBuild->possibleBuildings[playerData.indexOfBuildingInWorker]->model.mesh->AABBMin.y);

    Vec2 posTopLeft = {{playerData.buildingPos.x - playerData.buildingToBuild->tileSize.x * scene->map.tilesSize.x / 2,
                        playerData.buildingPos.z - playerData.buildingToBuild->tileSize.y * scene->map.tilesSize.y / 2}};

    playerData.isBuildingValid = scene->map.isBuildingValid(scene->map.GetTileIndex(posTopLeft), playerData.buildingToBuild->tileSize);
}
void Game::InputCancelBuilding()
{
    playerData.buildingToBuild = nullptr;
    playerData.workerWhoBuild = nullptr;
    playerData.indexOfBuildingInWorker = 0;
    playerData.isBuildingValid = false;
}
void Game::InputValidateBuilding()
{
    playerData.workerWhoBuild->StartBuilding(scene->map, playerData.buildingPos, playerData.indexOfBuildingInWorker);

    playerData.buildingToBuild = nullptr;
    playerData.workerWhoBuild = nullptr;
    playerData.indexOfBuildingInWorker = 0;
}
void Game::InputStartSelectionQuad()
{
    playerData.makingASelectionQuad = true;
    playerData.selectionQuadStart = playerData.mousePosInWorld;
}
void Game::InputEndSelectionQuad()
{
    playerData.makingASelectionQuad = false;

    //if selection quad is small try select closest Entity else select all entities with trs.pos inside selection quad
    if ((playerData.selectionQuadStart - playerData.mousePosInWorld).Length() < MINIMUM_SELECTION_QUAD_LENGTH)
    {
        coordinator.selectedEntities.clear();
        ECS::Entity* possibleSelectedEntity = coordinator.GetClosestSelectableEntity(playerData.selectionQuadStart, E_ARMY_NAME::E_PLAYER);

        if (possibleSelectedEntity)
            coordinator.selectedEntities.push_back(possibleSelectedEntity);
    }
    else
        coordinator.SelectEntities(playerData.selectionQuadStart, playerData.mousePosInWorld);
}
void Game::InputMoveSelected()
{
    if (scene->map.GetTile(playerData.mousePosInWorld).isObstacle)
        return;

    //set MovableEntities
    std::vector<ECS::Entity*> movableEntities;
    for (int i = 0; i < coordinator.selectedEntities.size(); ++i)
    {
        float selectedEntityId = coordinator.selectedEntities[i]->id;
        ComponentGameplay& gameplay = coordinator.componentHandler->GetComponentGameplay(selectedEntityId);

        if (gameplay.signatureGameplay & CGP_SIGNATURE::MOVE)
            movableEntities.push_back(coordinator.selectedEntities[i]);
    }

    //Calculate centroid
    Vec3 centroid{ 0, 0, 0 };
    for (int i = 0; i < movableEntities.size(); ++i)
    {
        //divide by movableEntities.size in for loop, so we're sure we can't divide by 0 if we don't use a if (movableEntities.size())
        centroid += coordinator.componentHandler->GetComponentTransform(movableEntities[i]->id).pos / movableEntities.size();
    }

    //Apply pathfind to each movableEntities
    for (int i = 0; i < movableEntities.size(); ++i)
    {
        float movableEntitiesId = movableEntities[i]->id;
        ComponentGameplay& gameplay = coordinator.componentHandler->GetComponentGameplay(movableEntitiesId);
        ComponentTransform& trs = coordinator.componentHandler->GetComponentTransform(movableEntitiesId);

        Vec3 offsetFromCentroid = trs.pos - centroid;
        Vec3 finalPos = playerData.mousePosInWorld + offsetFromCentroid;
        //pathfind to mousePos + offset
        if (offsetFromCentroid.Length() < OFFSET_MAX_FROM_CENTROID && scene->map.ApplyPathfinding(scene->map.GetTile(trs.pos), scene->map.GetTile(finalPos)))
            gameplay.componentMove.SetPath(scene->map.GetTile(finalPos));
        //pathfind to mousePos
        else if (scene->map.ApplyPathfinding(scene->map.GetTile(trs.pos), scene->map.GetTile(playerData.mousePosInWorld)))
            gameplay.componentMove.SetPath(scene->map.GetTile(playerData.mousePosInWorld));
        else
            std::cout << "No Path Find\n";
    }

}
void Game::InputSetNewEntityDestination()
{
    for (int i = 0; i < coordinator.selectedEntities.size(); ++i)
    {
        float selectedEntityId = coordinator.selectedEntities[i]->id;
        ComponentGameplay& gameplay = coordinator.componentHandler->GetComponentGameplay(selectedEntityId);

        if (gameplay.signatureGameplay & CGP_SIGNATURE::PRODUCER)
            gameplay.componentProducer.newUnitDestination = playerData.mousePosInWorld;
    }
}
void Game::InputSetResourceToWorkers()
{
    ECS::Entity* resource = coordinator.GetClosestSelectableEntity(playerData.mousePosInWorld, E_ARMY_NAME::E_DEFAULT_NAME, CGP_SIGNATURE::RESOURCE);

    //if we don't select an entity with CGPResource in the first place, quit
    if (!resource)
        return;

    for (int i = 0; i < coordinator.selectedEntities.size(); ++i)
    {
        if (coordinator.componentHandler->GetComponentGameplay(resource->id).componentResource.nbOfWorkerOnIt >= MAX_WORKER_PER_RESOURCE)
        {
            resource = coordinator.GetClosestFreeResourceEntity(playerData.mousePosInWorld);
            //if there is no more free resources available
            if (!resource)
                return;
        }

        ComponentGameplay& gameplay = coordinator.componentHandler->GetComponentGameplay(coordinator.selectedEntities[i]->id);

        if (gameplay.signatureGameplay & CGP_SIGNATURE::WORKER)
            gameplay.componentWorker.SetResource(coordinator.componentHandler->GetComponentTransform(resource->id).pos, coordinator.componentHandler->GetComponentGameplay(resource->id).componentResource);

    }

}
void Game::InputStartBuilding(int index)
{
    for (int i = 0; i < coordinator.selectedEntities.size(); ++i)
    {
        ComponentGameplay& gameplay = coordinator.componentHandler->GetComponentGameplay(coordinator.selectedEntities[i]->id);

        if (gameplay.signatureGameplay & CGP_SIGNATURE::WORKER && !gameplay.componentWorker.BuildingInConstruction)
        {
            playerData.buildingToBuild = &gameplay.componentWorker.possibleBuildings[0]->gameplay.componentProducer;
            playerData.workerWhoBuild = &gameplay.componentWorker;
            playerData.indexOfBuildingInWorker = index;
            break;
        }
    }
}
void Game::InputAddUnit(int index)
{
    for (int i = 0; i < coordinator.selectedEntities.size(); ++i)
    {
        ComponentGameplay& gameplay = coordinator.componentHandler->GetComponentGameplay(coordinator.selectedEntities[i]->id);

        if (gameplay.signatureGameplay & CGP_SIGNATURE::PRODUCER)
            gameplay.componentProducer.AddUnitToQueue(index);
    }
}

void Game::ECSCalls()
{
    coordinator.armyHandler->UpdateArmyCoordinators(scene->map);
    resources.UpdateScriptsContent();
    coordinator.ApplyScriptUpdate();
    coordinator.ApplyGameplay(scene->map);
    coordinator.ApplyRemoveUnnecessaryEntities();
}

void Game::DisplayLife()
{
    std::vector<Cookie::Render::InstancedData> data;
    float angle = Cookie::Core::Math::ToRadians(180) - scene.get()->camera.get()->rot.x;
    static float lifeCurrent;
    static float lifeMax;
    static Mat4 trs;
    static Vec3 posLife;
    for (int i = 0; i < coordinator.entityHandler->livingEntities; i++)
    {
        //Check first if the entity has a life
        if (!coordinator.CheckSignature(coordinator.entityHandler->entities[i].signature, C_SIGNATURE::GAMEPLAY))
            continue;

        ComponentGameplay gameplay = coordinator.componentHandler->GetComponentGameplay(coordinator.entityHandler->entities[i].id);
        if (!coordinator.CheckSignature(gameplay.signatureGameplay, CGP_SIGNATURE::LIVE))
            continue;

        lifeCurrent = gameplay.componentLive.lifeCurrent;
        lifeMax = gameplay.componentLive.lifeMax;
        trs = Cookie::Core::Math::Mat4::Translate(gameplay.trs->pos);
        posLife = gameplay.componentLive.lifeBarOffset;
        Cookie::Render::InstancedData newData;
        Vec3 scaleBlack = Vec3(2 * lifeMax / 10, 0.25, 1);
        Vec3 scaleGreen = Vec3(2 * lifeCurrent / 10, 0.25, 1);
        Vec3 posGreen = Vec3((lifeMax - lifeCurrent) / 10, 0, 0);
        if (coordinator.CheckSignature(coordinator.entityHandler->entities[i].signature, C_SIGNATURE::MODEL) && 
                coordinator.CheckSignature(coordinator.entityHandler->entities[i].signature, C_SIGNATURE::TRANSFORM))
        {
            //Calcul the scale and the pos to clamp with the model
            ComponentTransform& transform = coordinator.componentHandler->GetComponentTransform(coordinator.entityHandler->entities[i].id);
            ComponentModel& model = coordinator.componentHandler->GetComponentModel(coordinator.entityHandler->entities[i].id);
            if (model.mesh)
            {
                Core::Math::Vec3 AABBMax = model.mesh->AABBMax;
                scaleBlack.x = lifeMax * (AABBMax.x * transform.scale.x * 2) / lifeMax;
                scaleGreen.x = lifeCurrent * (AABBMax.x * transform.scale.x * 2) / lifeMax;
                posGreen.x = (scaleBlack.x - scaleGreen.x) / 2;
            }
        }
        
        newData.World = Cookie::Core::Math::Mat4::TRS(posLife, Vec3(angle, 0, 0), scaleBlack) * trs;
        newData.Color = Vec4(0, 0, 0, 1);
        newData.isBillboard = false;
        data.push_back(newData);

        newData.World = Cookie::Core::Math::Mat4::TRS(posLife - posGreen, Vec3(angle, 0, 0), scaleGreen) * trs;
        newData.Color = Vec4((lifeMax - lifeCurrent) / lifeMax, lifeCurrent / lifeMax, 0, 1);
        data.push_back(newData);
    }

    //Use the particles shader withe the instancing
    if (data.size() > 0)
        ::ParticlesHandler::shader.Draw(*scene.get()->camera.get(), resources.meshes["Quad"].get(), resources.textures2D["White"].get(), data);
}


/*================== SETTER/GETTER ==================*/

void Game::SetScene()
{
    scene->InitCoordinator(coordinator);

    scene->camera->SetProj(scene->camera.get()->fov, renderer.window.width, renderer.window.height, CAMERA_INITIAL_NEAR, CAMERA_INITIAL_FAR);
    scene->camera->ResetPreviousMousePos();
    //scene->camera->ForceUpdate();
    SetCamClampFromMap();
    scene->camera->ZoomClamp = { {2.0f, 20.0f} };
    scene->camera->Deactivate();

    renderer.drawData.SetScene(scene.get());
    renderer.skyBox.texture = scene->skyBox;
}

void Game::SetCamClampFromMap()
{
    Vec3 middle = scene->camera->ScreenPointToWorldDir({ { 0.0f,0.0f } });
    Vec3 UpperRight = scene->camera->ScreenPointToWorldDir({ { 1.0f,1.0f } });
    Vec3 DownLeft = scene->camera->ScreenPointToWorldDir({ { -1.0f,-1.0f } });

    float t = (scene->camera->pos.y) / middle.y;
    middle = scene->camera->pos - middle * t;
    t = (scene->camera->pos.y) / UpperRight.y;
    UpperRight = scene->camera->pos - UpperRight * t;
    t = (scene->camera->pos.y) / DownLeft.y;
    DownLeft = scene->camera->pos - DownLeft * t;

    float width = UpperRight.x - DownLeft.x;
    if (width > scene->map.trs.scale.x)
        width = scene->map.trs.scale.x;

    /* going forward is in the negative z */
    float depth = DownLeft.z - UpperRight.z;
    if (depth > scene->map.trs.scale.z)
        depth = scene->map.trs.scale.z;

    scene->camera->mapClampX = { { -scene->map.trs.scale.x * 0.5f ,scene->map.trs.scale.x * 0.5f } };
    scene->camera->mapClampZ = { { -scene->map.trs.scale.z * 0.5f , scene->map.trs.scale.z * 0.5f} };
}

void Game::TryResizeWindow()
{
    int width = 0;
    int height = 0;

    glfwGetWindowSize(renderer.window.window, &width, &height);

    if (width <= 0 || height <= 0)
        return;

    if (renderer.window.width != width || renderer.window.height != height)
    {
        Core::DebugMessageHandler::Summon().Log((std::to_string(width) + ' ' + std::to_string(height)).c_str());
        printf("%d, %d\n", width, height);
        renderer.window.width = width;
        renderer.window.height = height;

        renderer.ResizeBuffer(width, height);
        frameBuffer.Resize(width, height);
        //scene->camera->SetProj(Core::Math::ToRadians(60.f), width, height, CAMERA_INITIAL_NEAR, CAMERA_INITIAL_FAR);
    }
}


