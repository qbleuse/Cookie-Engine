#include "Editor.hpp" 
#include "UIcore.hpp"
#include "UIeditor_AllIn.hpp"
#include "Serialization.hpp"
#include "Primitives.hpp"
#include "Physics/PhysicsHandle.hpp"
#include "Resources/Scene.hpp"
#include "CGPMove.hpp"
#include "CGPProducer.hpp"
//#include "SoundManager.hpp"

using namespace Cookie;
using namespace Cookie::Core;
using namespace Cookie::Core::Math;
using namespace Cookie::ECS;
using namespace Cookie::Gameplay;
using namespace Cookie::Resources;
using namespace rp3d;

Editor::Editor()
    : editorFBO{game.renderer.window.width,game.renderer.window.height}
{
    cam.SetProj(60.f, game.renderer.viewport.Width, game.renderer.viewport.Height, CAMERA_INITIAL_NEAR, CAMERA_INITIAL_FAR);
    cam.pos = { 0.f , 20.0f,30.0f };
    cam.rot = { Core::Math::ToRadians(30.0f) ,0.0f,0.0f };
    cam.ResetPreviousMousePos();
    cam.Update();
    cam.Deactivate();
    game.scene->InitCoordinator(game.coordinator);
    
    //Load default Scene
    Resources::Serialization::Load::LoadScene("Assets/Save/Default.CAsset", game);
    game.SetScene();

    game.uiMenu.GiveQuitFunction([&editor=*this]{editor.isPlaying = false;});

    editorUI.AddItem(new UIwidget::SaveButton(game.scene, game.resources), 0);
    editorUI.AddWItem(new UIwidget::ExitPannel(game.renderer.window.window), 0);
    
    editorUI.AddWItem(new UIwidget::TextureEditor(game.resources), 1);
    editorUI.AddWItem(new UIwidget::AIBehaviorEditor(game.resources), 1);
    editorUI.AddWItem(new UIwidget::GameUIeditor(game), 1);
    //editorUI.AddWItem(new UIwidget::SoundOrchestrator(game.resources), 1);
    editorUI.AddWItem(new UIwidget::ParticleEditor(game.resources), 1);
    
    editorUI.AddWItem(new UIwidget::Inspector(selectedEntity, game.resources, game.coordinator), 2);
    editorUI.AddWItem(new UIwidget::Hierarchy(game.resources, game.scene, game.coordinator, selectedEntity), 2);
    editorUI.AddWItem(new UIwidget::WorldSettingsWidget(game.scene, game.scene.get()->lights, game.renderer.skyBox, game.resources), 2);
    editorUI.AddWItem(new UIwidget::Console(CDebug, game.resources), 2);
    editorUI.AddWItem(new UIwidget::FileExplorer(game), 2);

    editorUI.AddWItem(new UIwidget::DemoWindow, 3);


    UIwidget::Toolbar* toolbar = new UIwidget::Toolbar(game.resources, isPlaying, game.isPaused);
    editorUI.AddWindow(new UIwidget::Viewport(toolbar, game.renderer.window.window, editorFBO, &cam, game.coordinator, selectedEntity));
    editorUI.AddWindow(new UIwidget::GamePort(isPlaying, game));

    InitEditComp();

    Physics::PhysicsHandle::editWorld->setIsDebugRenderingEnabled(false);
}

Editor::~Editor()
{
    //Save all prefabs in folder Prefabs
   // Resources::Serialization::Save::SaveAllPrefabs(game.resources);
    //Resources::SoundManager::Release();
}


void Editor::InitEditComp()
{
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        int j = game.coordinator.entityHandler->entities[i].id;
        editingComponent[j].InitComponent(game.coordinator.componentHandler->GetComponentTransform(j));
    }
}

void Editor::ModifyEditComp()
{
    int max = livingEntitiesNb > game.coordinator.entityHandler->livingEntities ? livingEntitiesNb : game.coordinator.entityHandler->livingEntities;
    for (int i = 0; i < max; i++)
    {
        int j = game.coordinator.entityHandler->entities[i].id;
        ComponentEditor& jComponent = editingComponent[j];
        jComponent.editTrs = &game.coordinator.componentHandler->GetComponentTransform(j);
        bool hasMesh = (game.coordinator.entityHandler->entities[i].signature & C_SIGNATURE::MODEL) && game.coordinator.componentHandler->GetComponentModel(j).mesh != nullptr;

        /* if it has mesh and alive, change collider */
        if (hasMesh)
        {
            jComponent.AABBMin = game.coordinator.componentHandler->GetComponentModel(j).mesh->AABBMin;
            jComponent.AABBMax = game.coordinator.componentHandler->GetComponentModel(j).mesh->AABBMax;
            jComponent.MakeCollider();
        }
        
        /* if entity deleted, remove collider */
        if (!hasMesh && jComponent.collider)
        {
            if (jComponent.body)
            {
                if (jComponent.collider)
                {
                    jComponent.body->removeCollider(editingComponent[j].collider);
                    jComponent.collider = nullptr;
                }
            }
        }

        jComponent.Update();
    }
}

void Editor::ChooseDrawBuffer()
{
    static Render::FrameBuffer* fbo = &editorFBO;

    if (ImGui::GetIO().KeysDownDuration[GLFW_KEY_F1] >= 0.0f)
    {
        fbo = &editorFBO;
    }
    if (ImGui::GetIO().KeysDownDuration[GLFW_KEY_F2] >= 0.0f)
    {
        fbo = &game.renderer.geomPass.posFBO;
    }
    else if (ImGui::GetIO().KeysDownDuration[GLFW_KEY_F3] >= 0.0f)
    {
        fbo = &game.renderer.geomPass.normalFBO;
    }
    else if (ImGui::GetIO().KeysDownDuration[GLFW_KEY_F4] >= 0.0f)
    {
        fbo = &game.renderer.geomPass.albedoFBO;
    }
    else if (ImGui::GetIO().KeysDownDuration[GLFW_KEY_F5] >= 0.0f)
    {
        fbo = &game.renderer.lightPass.diffuseFBO;
    }
    else if (ImGui::GetIO().KeysDownDuration[GLFW_KEY_F6] >= 0.0f)
    {
        fbo = &game.renderer.lightPass.specularFBO;
    }

    if (fbo == &editorFBO)
        return;

    game.renderer.shadPass.Set();
    Render::RendererRemote::context->OMSetRenderTargets(1, &editorFBO.renderTargetView, nullptr);
    game.renderer.DrawFrameBuffer(*fbo);
}

void Editor::HandleEditorInput()
{
    game.coordinator.ApplyComputeTrs();

    cam.Update();

    if (currentScene != game.scene.get() || game.coordinator.entityHandler->livingEntities != livingEntitiesNb)
    {
        selectedEntity = {};
        selectedEntity.componentHandler = game.coordinator.componentHandler;
        ModifyEditComp();
        currentScene = game.scene.get();
        livingEntitiesNb = game.coordinator.entityHandler->livingEntities;
    }

    if (!ImGui::GetIO().MouseDownDuration[0])
    {
        Core::Math::Vec3 fwdRay = cam.pos + cam.MouseToWorldDirClamp() * cam.camFar;
        rp3d::Ray ray({ cam.pos.x,cam.pos.y,cam.pos.z }, { fwdRay.x,fwdRay.y,fwdRay.z });
        Physics::PhysicsHandle::editWorld->raycast(ray, this);
    }

    if (selectedEntity.toChangeEntityIndex >= 0)
    {
        PopulateFocusedEntity();
    }
    if (selectedEntity.focusedEntity && (selectedEntity.focusedEntity->signature & C_SIGNATURE::PHYSICS))
    {
        selectedEntity.componentHandler->GetComponentPhysics(selectedEntity.focusedEntity->id).Set(selectedEntity.componentHandler->GetComponentTransform(selectedEntity.focusedEntity->id));
    }
}

void Editor::Loop()
{    
    bool previewIsPlaying = isPlaying;

    while (!glfwWindowShouldClose(game.renderer.window.window))
    {
        TryResizeWindow();

        // Present frame

        if (isPlaying && previewIsPlaying)
        {
            game.Update();
        }
        else if (isPlaying && !previewIsPlaying)
        {
            previewIsPlaying = isPlaying;
            game.Start();
            Resources::Serialization::Save::SaveScene(*game.scene.get(), game.resources);
        }
        else if (!isPlaying && previewIsPlaying)
        {
            previewIsPlaying = isPlaying;
            
            game.playerData.makingASelectionQuad = false;
            
            Resources::Serialization::Load::LoadScene(game.scene.get()->filepath.c_str(), game);
            game.SetScene();
            game.coordinator.selectedEntities.clear();
            game.particlesHandler.Clear();
        }
        else
        {
            glfwPollEvents();
        }

        HandleEditorInput();

        //Draw
        game.renderer.Clear();
        game.renderer.ClearFrameBuffer(editorFBO);
        game.renderer.Draw(&cam,editorFBO);
        ChooseDrawBuffer();
		game.particlesHandler.Draw(cam);

        //dbgRenderer.Draw(cam.GetViewProj());

        game.renderer.SetBackBuffer();
        UIcore::BeginFrame();
        editorUI.UpdateUI();
        UIcore::EndFrame();
        game.renderer.Render();
    }

    if (game.scene)
        game.scene->skyBox = game.renderer.skyBox.texture;

    Particles::ParticlesHandler::shader.Destroy();
}

void Editor::TryResizeWindow()
{
    int width = 0;
    int height = 0;

    glfwGetWindowSize(game.renderer.window.window, &width, &height);

    if (width <= 0 || height <= 0)
        return;

    if (game.renderer.window.width != width || game.renderer.window.height != height)
    {
        //Core::DebugMessageHandler::Summon().Log((std::to_string(width) + ' ' + std::to_string(height)).c_str());
        //printf("%d, %d\n", width, height);
        game.renderer.window.width = width;
        game.renderer.window.height = height;

        game.renderer.ResizeBuffer(width, height);
        game.frameBuffer.Resize(width, height);
        editorFBO.Resize(width, height);

        //scene->camera->SetProj(Core::Math::ToRadians(60.f), width, height, CAMERA_INITIAL_NEAR, CAMERA_INITIAL_FAR);
    }
}
