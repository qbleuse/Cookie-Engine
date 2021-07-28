#include "Editor.hpp"
#include <reactphysics3d.h>
#include "InspectorWidget.hpp"

#include "Prefab.hpp"

#include "MapExplorerHelper.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "CustomImWidget.hpp"


using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::ECS;
using namespace Cookie::Resources;
using namespace Cookie::Gameplay;


void Inspector::WindowDisplay()
{
    if (selectedEntity.focusedEntity != recordedEntity || selectedEntity.componentHandler != coordinator.componentHandler)
    {
        recordedEntity = selectedEntity.focusedEntity;
        selectedCollider = nullptr;
    }

    TryBeginWindow()
    {
        if (selectedEntity.focusedEntity) EntityInspection();
    }
    
    ImGui::End();
}


void Inspector::EntityInspection()
{
    InputText("Entity name", &selectedEntity.focusedEntity->name);
    InputText("Entity tag",  &selectedEntity.focusedEntity->tag );

    ImGui::Separator();

    if (selectedEntity.focusedEntity->signature & C_SIGNATURE::TRANSFORM)    TransformInterface();
    if (selectedEntity.focusedEntity->signature & C_SIGNATURE::MODEL)        ModelInterface();
    if (selectedEntity.focusedEntity->signature & C_SIGNATURE::PHYSICS)      PhysicsInterface();
    if (selectedEntity.focusedEntity->signature & C_SIGNATURE::SCRIPT)       ScriptInterface();
    if (selectedEntity.focusedEntity->signature & C_SIGNATURE::GAMEPLAY)     GameplayInterface();
    if (selectedEntity.focusedEntity->signature & C_SIGNATURE::FX)           FXInterface();


    if (Button("Add component...")) OpenPopup("Add component popup");

    if (BeginPopup("Add component popup"))
    {
        if (selectedEntity.focusedEntity->signature & C_SIGNATURE::TRANSFORM)  TextDisabled("Component Transform already added");
        else if (Selectable("Add component Transform"))
            coordinator.componentHandler->AddComponent(*selectedEntity.focusedEntity, C_SIGNATURE::TRANSFORM);


        if (selectedEntity.focusedEntity->signature & C_SIGNATURE::MODEL)      TextDisabled("Component Model already added");
        else if (Selectable("Add component Model"))
            coordinator.componentHandler->AddComponent(*selectedEntity.focusedEntity, C_SIGNATURE::MODEL);


        if (selectedEntity.focusedEntity->signature & C_SIGNATURE::PHYSICS)    TextDisabled("Component Physics already added");
        else if (Selectable("Add component Physics"))
            coordinator.componentHandler->AddComponent(*selectedEntity.focusedEntity, C_SIGNATURE::PHYSICS);

        if (selectedEntity.focusedEntity->signature & C_SIGNATURE::SCRIPT)     TextDisabled("Component Script already added");
        else if (Selectable("Add component Script"))
            coordinator.componentHandler->AddComponent(*selectedEntity.focusedEntity, C_SIGNATURE::SCRIPT);


        if (selectedEntity.focusedEntity->signature & C_SIGNATURE::GAMEPLAY)   TextDisabled("Component Gameplay already added");
        else if (Selectable("Add component Gameplay"))
            coordinator.componentHandler->AddComponent(*selectedEntity.focusedEntity, C_SIGNATURE::GAMEPLAY);

        if (selectedEntity.focusedEntity->signature & C_SIGNATURE::FX)   TextDisabled("Component FX already added");
        else if (Selectable("Add component FX"))
            coordinator.componentHandler->AddComponent(*selectedEntity.focusedEntity, C_SIGNATURE::FX);

        EndPopup();
    }
}

void Inspector::TransformInterface()
{
    if (TreeNode("Transform"))
    {
        ComponentTransform& trsf = coordinator.componentHandler->GetComponentTransform(selectedEntity.focusedEntity->id);

        Text("Pos:"); SameLine(65.f); bool posHasChanged =  DragFloat3("##POS", trsf.pos.e);
        Text("Rot:"); SameLine(65.f); bool rotHasChanged =  DragFloat3("##ROT", trsf.rot.e);
        Text("Scl:"); SameLine(65.f); bool sclHasChanged =  DragFloat3("##SCL", trsf.scale.e);

        if (posHasChanged || rotHasChanged || sclHasChanged)
            trsf.trsHasChanged = true;
        
        NewLine();
        if (Selectable("Remove component##TRSF"))
        {
            coordinator.componentHandler->RemoveComponent(*selectedEntity.focusedEntity, C_SIGNATURE::TRANSFORM);
        }


        TreePop();
    }

    ImGui::Separator();
}

void Inspector::ModelInterface()
{
    if (TreeNode("Model"))
    {
        ComponentModel& modelComp = coordinator.componentHandler->GetComponentModel(selectedEntity.focusedEntity->id);

        static std::string researchString;

        //===== MESH =====//

        Text("Mesh:"); SameLine(100);

        ResourceMapExplorer<Mesh>("mesh", std::move("##MESHSELECT"), resources.meshes, modelComp.mesh);


//===== TEXTURES =====//
        
        Text("Albedo:"); SameLine(100);

        ResourceMapExplorer<Texture>("texture", std::move("##ALDEBOSELECT"), resources.textures2D, modelComp.albedo);


        Text("Normal:"); SameLine(100);

        ResourceMapExplorer<Texture>("normal texture", std::move("##NORMSELECT"), resources.textures2D, modelComp.normal);


        ImGui::Custom::TextSnip("metallic-Roughness", 9); SameLine(); Text(":"); SameLine(100);

        ResourceMapExplorer<Texture>("metallic-rough texture", std::move("##MRSELECT"), resources.textures2D, modelComp.metallicRoughness);


        ImGui::Custom::TextSnip("In-Game icon", 9); SameLine(); Text(":"); SameLine(100);

        ResourceMapExplorer<Texture>("In-Game icon", std::move("##IGICSELECTOR"), resources.icons, modelComp.icon);

//====================//

        NewLine();
        if (Selectable("Remove component##MODEL"))
        {
            coordinator.componentHandler->RemoveComponent(*selectedEntity.focusedEntity, C_SIGNATURE::MODEL);
        }

        TreePop();
    }

    ImGui::Separator();
}

void Inspector::PhysicsInterface()
{
    if (TreeNode("Physics"))
    {
        ComponentPhysics& physicComp = coordinator.componentHandler->GetComponentPhysics(selectedEntity.focusedEntity->id);

        // due to the nature of how we can access and set variables in react physic 3D, this variable are used numerous times to ease the manipulations with ImGui and save memory.
        static float multiUseFloat = 0; static bool multiUseBool = false;


        if (TreeNode("Colliders"))
        {
            Text("Active colliders:");
            Indent(15.f);

            std::vector<reactphysics3d::Collider*>& colliders = physicComp.physColliders;

            //====== COLLIDERS - beginning list of current colliders ======//

            for (size_t i = 0; i < colliders.size(); i++)
            {
                std::string nameTag;

                if (selectedCollider == colliders[i])
                    nameTag += '[';

                switch (colliders[i]->getCollisionShape()->getName())
                {
                case reactphysics3d::CollisionShapeName::SPHERE:    nameTag += "Sphere";    break;
                case reactphysics3d::CollisionShapeName::CAPSULE:   nameTag += "Capsule";   break;
                case reactphysics3d::CollisionShapeName::BOX:       nameTag += "Box";       break;

                default: break;
                }

                if (selectedCollider == colliders[i])
                    nameTag += ']';

                nameTag += "##";
                nameTag += std::to_string(i);


                if (Button(nameTag.c_str()))
                    selectedCollider = colliders[i];
            }

            Unindent(15.f);

            //====== COLLIDERS - sub-window for editing ======//

            if (BeginChild(windowName, { GetContentRegionAvail().x, 150 }, true) && (selectedCollider != nullptr))
            {
                multiUseBool = selectedCollider->getIsTrigger();
                if (Checkbox("Trigger", &multiUseBool))
                    selectedCollider->setIsTrigger(multiUseBool);


                if (TreeNode("Collider transform"))
                {
                    reactphysics3d::Vector3     pos = selectedCollider->getLocalToBodyTransform().getPosition();
                    reactphysics3d::Quaternion  quat = selectedCollider->getLocalToBodyTransform().getOrientation();
                    Core::Math::Vec3            rot = Core::Math::Quat::ToEulerAngle({ quat.w, quat.x, quat.y, quat.z });

                    Text("Pos:"); SameLine(60.f);
                    if (DragFloat3("##COLPOS", &pos[0], 0.25))
                        selectedCollider->setLocalToBodyTransform({ pos, quat });

                    Text("Rot:"); SameLine(60.f);
                    if (DragFloat3("##COLROT", rot.e))
                        selectedCollider->setLocalToBodyTransform({ pos, reactphysics3d::Quaternion::fromEulerAngles(Core::Math::ToRadians(rot.x), Core::Math::ToRadians(rot.y), Core::Math::ToRadians(rot.z)) });

                    NewLine();
                    Text("Characteristics:");

                    if (selectedCollider->getCollisionShape()->getName() == reactphysics3d::CollisionShapeName::SPHERE)
                    {
                        reactphysics3d::SphereShape* sphereCol = static_cast<reactphysics3d::SphereShape*>(selectedCollider->getCollisionShape());


                        multiUseFloat = sphereCol->getRadius();
                        if (DragFloat("##SRADIUS", &multiUseFloat, 0.25, 0, 0, "Radius: %.3f"))
                            sphereCol->setRadius(multiUseFloat > 0 ? multiUseFloat : 0.001);
                    }
                    else if (selectedCollider->getCollisionShape()->getName() == reactphysics3d::CollisionShapeName::CAPSULE)
                    {
                        reactphysics3d::CapsuleShape* capsuleCol = static_cast<reactphysics3d::CapsuleShape*>(selectedCollider->getCollisionShape());


                        multiUseFloat = capsuleCol->getHeight();
                        if (DragFloat("##CHEIGHT", &multiUseFloat, 0.25, 0, 0, "Height: %.3f"))
                            capsuleCol->setHeight(multiUseFloat > 0 ? multiUseFloat : 0.001);

                        multiUseFloat = capsuleCol->getRadius();
                        if (DragFloat("##CRADIUS", &multiUseFloat, 0.25, 0, 0, "Radius: %.3f"))
                            capsuleCol->setRadius(multiUseFloat > 0 ? multiUseFloat : 0.001);
                    }
                    else if (selectedCollider->getCollisionShape()->getName() == reactphysics3d::CollisionShapeName::BOX)
                    {
                        reactphysics3d::BoxShape* boxCol = static_cast<reactphysics3d::BoxShape*>(selectedCollider->getCollisionShape());


                        reactphysics3d::Vector3&& halfExtents = boxCol->getHalfExtents();
                        Text("Half extents:"); SameLine();
                        if (DragFloat3("##BSIZE", &halfExtents[0], 0.25))
                            boxCol->setHalfExtents(reactphysics3d::Vector3::max({ 0.001, 0.001, 0.001 }, halfExtents));
                    }

                    TreePop();
                }

                if (TreeNode("Collider material"))
                {
                    reactphysics3d::Material& colM = selectedCollider->getMaterial();

                    multiUseFloat = colM.getBounciness();
                    if (SliderFloat("##BOUNCINESS", &multiUseFloat, 0, 1, "Bounciness: %.5f"))
                        colM.setBounciness(multiUseFloat);

                    multiUseFloat = colM.getFrictionCoefficient();
                    if (DragFloat("##FRICTION", &multiUseFloat, 0.5, 0, 0, "Friction coef: %.3f"))
                        colM.setFrictionCoefficient(multiUseFloat >= 0 ? multiUseFloat : 0);

                    multiUseFloat = colM.getMassDensity();
                    if (DragFloat("##DENSITY", &multiUseFloat, 0.5, 0, 0, "Density: %.2f"))
                        colM.setMassDensity(multiUseFloat);

                    multiUseFloat = colM.getRollingResistance();
                    if (DragFloat("##ROLLRESIST", &multiUseFloat, 0.5, 0, 0, "Rolling resistance: %.5f"))
                        colM.setRollingResistance(multiUseFloat >= 0 ? multiUseFloat : 0);

                    TreePop();
                }
            }

            EndChild();

            //====== COLLIDERS - options to add new colliders ======//

            if (Button("Add a collider")) OpenPopup("Collider adder popup");
            if (BeginPopup("Collider adder popup"))
            {
                if (Button("Add a sphere collider"))
                {
                    physicComp.AddSphereCollider(1, { 0, 0, 0 }, { 0, 0, 0 });

                    selectedCollider = colliders.back();
                    CloseCurrentPopup();
                }

                if (Button("Add a box collider"))
                {
                    physicComp.AddCubeCollider({ 1, 1, 1 }, { 0, 0, 0 }, { 0, 0, 0 });

                    selectedCollider = colliders.back();
                    CloseCurrentPopup();
                }

                if (Button("Add a capsule collider"))
                {
                    physicComp.AddCapsuleCollider({ {1, 1} }, { 0, 0, 0 }, { 0, 0, 0 });

                    selectedCollider = colliders.back();
                    CloseCurrentPopup();
                }

                EndPopup();
            }


            TreePop();
        }


        if (TreeNode("Rigibody"))
        {
            reactphysics3d::RigidBody*& rigibod = physicComp.physBody;

            //====== RIGIBODY - all editable options ======//

            Text("Current entity dynamism:"); SameLine();

            switch (rigibod->getType())
            {
            case reactphysics3d::BodyType::STATIC:      TextColored({ 0.50, 0.50, 0.50, 1 }, "Static");     break;
            case reactphysics3d::BodyType::KINEMATIC:   TextColored({ 0.75, 0.75, 0.75, 1 }, "Kinematic");  break;
            case reactphysics3d::BodyType::DYNAMIC:     TextColored({ 1.00, 1.00, 1.00, 1 }, "Dynamic");    break;
            }

            Text("Change to:");

            SameLine();
            if (Button("Static") && rigibod->getType() != reactphysics3d::BodyType::STATIC)         rigibod->setType(reactphysics3d::BodyType::STATIC);
            SameLine();
            if (Button("Kinematic") && rigibod->getType() != reactphysics3d::BodyType::KINEMATIC)   rigibod->setType(reactphysics3d::BodyType::KINEMATIC);
            SameLine();
            if (Button("Dynamic") && rigibod->getType() != reactphysics3d::BodyType::DYNAMIC)       rigibod->setType(reactphysics3d::BodyType::DYNAMIC);

            NewLine();

            multiUseFloat = rigibod->getAngularDamping();
            if (DragFloat("##ANGULARDAMP", &multiUseFloat, 0.5, 0, 0, "Angular damping: %.3f"))
                rigibod->setAngularDamping(multiUseFloat >= 0 ? multiUseFloat : 0);

            multiUseFloat = rigibod->getLinearDamping();
            if (DragFloat("##LINEARDAMP", &multiUseFloat, 0.5, 0, 0, "Linear damping: %.3f"))
                rigibod->setLinearDamping(multiUseFloat >= 0 ? multiUseFloat : 0);

            multiUseFloat = rigibod->getMass();
            if (DragFloat("##MASS", &multiUseFloat, 1.0, 0, 0, "Mass: %.3f"))
                rigibod->setMass(multiUseFloat);


            multiUseBool = rigibod->isActive();
            if (Checkbox("Active", &multiUseBool))
                rigibod->setIsActive(multiUseBool);

            multiUseBool = rigibod->isAllowedToSleep();
            if (Checkbox("Allowed to sleep", &multiUseBool))
                rigibod->setIsAllowedToSleep(multiUseBool);

            multiUseBool = rigibod->isSleeping();
            Checkbox("Sleeping", &multiUseBool);

            multiUseBool = rigibod->isGravityEnabled();
            if (Checkbox("Gravity enabled", &multiUseBool))
                rigibod->enableGravity(multiUseBool);


            TreePop();
        }


        NewLine();
        if (Selectable("Remove component##COLLIDER"))
        {
            coordinator.componentHandler->RemoveComponent(*selectedEntity.focusedEntity, C_SIGNATURE::PHYSICS);
        }

        TreePop();
    }

    ImGui::Separator();
}

void Inspector::ScriptInterface()
{
    if (TreeNode("Script"))
    {
        ComponentScript& scriptComp = coordinator.componentHandler->GetComponentScript(selectedEntity.focusedEntity->id);
        
        for (size_t i = 0; i < scriptComp.scripts.size();)
        { 
            Text("%s", scriptComp.scripts[i].script->filename.c_str());
            Indent();
            
            if (Button( ("Remove##SCRIPT" + std::to_string(i)).c_str() ))
            {
                scriptComp.scripts.erase(scriptComp.scripts.begin() + i);
            }
            else ++i;

            NewLine();
            Unindent();
        }


        if (Button("Add a script")) OpenPopup("Script selector popup");
        
        if (BeginPopup("Script selector popup"))
        {
            for (std::unordered_map<std::string, std::unique_ptr<Script>>::iterator scrIt = resources.scripts.begin(); scrIt != resources.scripts.end(); scrIt++)
            {
                if (Button(scrIt->second->filename.c_str()))
                {
                    scriptComp.scripts.push_back(scrIt->second->CreateObject(std::to_string(selectedEntity.focusedEntity->id)));
                    CloseCurrentPopup();
                }
            }

            EndPopup();
        }


        NewLine();
        if (Selectable("Remove component##SCRIPT"))
        {
            coordinator.componentHandler->RemoveComponent(*selectedEntity.focusedEntity, C_SIGNATURE::SCRIPT);
        }

        TreePop();
    }

    ImGui::Separator();
}

void Inspector::GameplayInterface()
{
    if (TreeNode("Gameplay properties"))
    {
        ComponentGameplay& gameplayComp = coordinator.componentHandler->GetComponentGameplay(selectedEntity.focusedEntity->id);

        static const char* armyNames[] = { "Default", "Player", "AI1"};
        if (BeginCombo("##ARMY_NAME", armyNames[gameplayComp.teamName]))
        {
            if (Selectable(armyNames[0], gameplayComp.type == 0)) gameplayComp.teamName = Gameplay::E_ARMY_NAME::E_DEFAULT_NAME;
            if (Selectable(armyNames[1], gameplayComp.type == 1)) gameplayComp.teamName = Gameplay::E_ARMY_NAME::E_PLAYER;
            if (Selectable(armyNames[2], gameplayComp.type == 2)) gameplayComp.teamName = Gameplay::E_ARMY_NAME::E_AI1;

            EndCombo();
        }

        NewLine();
        DragFloat("##FIRSTCOST",  &gameplayComp.cost.costPrimary,   1.f, NULL, NULL, "Primary cost: %.0f");
        DragFloat("##SECONDCOST", &gameplayComp.cost.costSecondary, 1.f, NULL, NULL, "Secondary cost: %.0f");
        DragFloat("##SUPPLYCOST", &gameplayComp.cost.costSupply,    1.f, NULL, NULL, "Supply cost: %.0f" );

        DragFloat("##PRODUCTIONTIME", &gameplayComp.cost.timeToProduce, 0.25f, NULL, NULL, "Production time: %.2f");

        NewLine();

        static const char* _typesName[] = { "Default", "Worker", "Unit", "Building" };
        if (BeginCombo("##ENTITYTYPE", _typesName[gameplayComp.type]))
        {
            if (Selectable(_typesName[0], gameplayComp.type == 0)) gameplayComp.type = Gameplay::E_ARMY_TYPE::E_DEFAULT_TYPE;
            if (Selectable(_typesName[1], gameplayComp.type == 1)) gameplayComp.type = Gameplay::E_ARMY_TYPE::E_WORKER;
            if (Selectable(_typesName[2], gameplayComp.type == 2)) gameplayComp.type = Gameplay::E_ARMY_TYPE::E_UNIT;
            if (Selectable(_typesName[3], gameplayComp.type == 3)) gameplayComp.type = Gameplay::E_ARMY_TYPE::E_BUILDING;

            EndCombo();
        }

        NewLine();


        if (gameplayComp.signatureGameplay & CGP_SIGNATURE::LIVE)
        {
            if (TreeNode("Life/Armor properties"))
            {
                SameLine();
                if (SmallButton("Remove##LIVE"))
                    gameplayComp.RemoveComponent(CGP_SIGNATURE::LIVE);


                DragFloat("##LIFEMAX",  &gameplayComp.componentLive.lifeMax,     1.f, NULL, NULL,                               "Max life: %.0f");
                DragFloat("##LIFE",     &gameplayComp.componentLive.lifeCurrent, 1.f,    0, gameplayComp.componentLive.lifeMax, "Life: %.0f" );
                
                DragFloat("##ARMOR",    &gameplayComp.componentLive.armor,       1.f, NULL, NULL, "Armor: %.0f");

                DragFloat("##POS", &gameplayComp.componentLive.lifeBarOffset.y, 0.5f, -10, 10, "LifeBar Offset :  %.0f");

                TreePop();
                NewLine();
            }
        }
        else if (Selectable("Add life/Armor properties"))
            gameplayComp.AddComponent(CGP_SIGNATURE::LIVE);


        if (gameplayComp.signatureGameplay & CGP_SIGNATURE::MOVE)
        {
            if (TreeNode("Movement capacities"))
            {
                SameLine();
                if (SmallButton("Remove##MOVE"))
                    gameplayComp.RemoveComponent(CGP_SIGNATURE::MOVE);


                DragFloat("##SPEED", &gameplayComp.componentMove.moveSpeed, 0.25f, NULL, NULL, "Speed: %.2f");
                Text("Flying:"); SameLine(); Checkbox("##CANFLY", &gameplayComp.componentMove.isFlying);

                TreePop();
                NewLine();
            }
        }
        else if (Selectable("Add movement capacities"))
            gameplayComp.AddComponent(CGP_SIGNATURE::MOVE);


        if (gameplayComp.signatureGameplay & CGP_SIGNATURE::ATTACK)
        {
            if (TreeNode("Attack abilities"))
            {
                SameLine();
                if (SmallButton("Remove##ATTACK"))
                    gameplayComp.RemoveComponent(CGP_SIGNATURE::ATTACK);

                
                DragFloat("##POWER",  &gameplayComp.componentAttack.powerLevel,   1.00f, NULL, NULL, "Power Lv: %.2f");
                DragFloat("##DAMAGE", &gameplayComp.componentAttack.attackDamage, 0.25f, NULL, NULL, "Damage: %.2f"  );
                DragFloat("##RANGE",  &gameplayComp.componentAttack.attackRange,  0.25f, NULL, NULL, "Range: %.2f"   );
                DragFloat("##SPEED",  &gameplayComp.componentAttack.attackSpeed,  0.25f, NULL, NULL, "Speed: %.2f"   );

                TreePop();
                NewLine();
            }
        }
        else if (Selectable("Add attack abilities"))
            gameplayComp.AddComponent(CGP_SIGNATURE::ATTACK);


        if (gameplayComp.signatureGameplay & CGP_SIGNATURE::PRODUCER)
        {
            if (TreeNode("Production property"))
            {
                SameLine();
                if (SmallButton("Remove##PRODUCER"))
                    gameplayComp.RemoveComponent(CGP_SIGNATURE::PRODUCER);


                DragFloat2("Tile size (in x and z)", gameplayComp.componentProducer.tileSize.e, 0.5f, 0.5f, 100.f, "%.1f");
                DragInt("##SupplyGiven", &gameplayComp.componentProducer.supplyGiven, 1, 0, 10, "Supply given : %d");


                NewLine();
                Text("Can produce the following units:");

                int i = 0;
                for (auto it = gameplayComp.componentProducer.possibleUnits.begin(); it != gameplayComp.componentProducer.possibleUnits.end();)
                {
                    i++;

                    Text("%s", (*it)->name.c_str());
                    
                    SameLine();

                    std::string tinyDeleterTag = "X##" + std::to_string(i);
                    if (SmallButton(tinyDeleterTag.c_str()))
                    {
                        it = gameplayComp.componentProducer.possibleUnits.erase(it);
                    }
                    else it++;
                }
                {
                    Prefab* newProductable = ResourceMapSelector<Prefab>("prefab", "##PRDUNIT_SELECTOR", resources.prefabs);

                    if (newProductable != nullptr)
                        gameplayComp.componentProducer.possibleUnits.push_back(newProductable);
                }

                TreePop();
                NewLine();
            }
        }
        else if (Selectable("Make this entity a producer"))
            gameplayComp.AddComponent(CGP_SIGNATURE::PRODUCER);


        if (gameplayComp.signatureGameplay & CGP_SIGNATURE::WORKER)
        {
            if (TreeNode("Worker property"))
            {
                SameLine();
                if (SmallButton("Remove##WORKER"))
                    gameplayComp.RemoveComponent(CGP_SIGNATURE::WORKER);


                Text("Can produce the following buildings:");

                int i = 0;
                for (auto it = gameplayComp.componentWorker.possibleBuildings.begin(); it != gameplayComp.componentWorker.possibleBuildings.end();)
                {
                    i++;

                    Text("%s", (*it)->name.c_str());

                    SameLine();

                    std::string tinyDeleterTag = "X##" + std::to_string(i);
                    if (SmallButton(tinyDeleterTag.c_str()))
                    {
                        it = gameplayComp.componentWorker.possibleBuildings.erase(it);
                    }
                    else it++;
                }
                {
                    Prefab* newProductable = ResourceMapSelector<Prefab>("prefab", "##PRDUNIT_SELECTOR", resources.prefabs);

                    if (newProductable != nullptr)
                        gameplayComp.componentWorker.possibleBuildings.push_back(newProductable);
                }

                TreePop();
                NewLine();
            }
        }
        else if (Selectable("Make this entity a worker"))
            gameplayComp.AddComponent(CGP_SIGNATURE::WORKER);


        if (gameplayComp.signatureGameplay & CGP_SIGNATURE::RESOURCE)
        {
            if (TreeNode("Resource property"))
            {
                SameLine();
                if (SmallButton("Remove##RESOURCE"))
                    gameplayComp.RemoveComponent(CGP_SIGNATURE::RESOURCE);


                DragFloat("##RESERVE", &gameplayComp.componentResource.resourceReserve, 1.f, NULL, NULL, "Reserve: %.0f");
                Text("Is a primary resource:"); SameLine(); Checkbox("##ISPRIMARY", &gameplayComp.componentResource.isPrimary);

                TreePop();
            }
        }
        else if (Selectable("Transform this entity into game resources"))
            gameplayComp.AddComponent(CGP_SIGNATURE::RESOURCE);



        NewLine();
        if (Selectable("Remove component##GAMEPLAY"))
        {
            coordinator.componentHandler->RemoveComponent(*selectedEntity.focusedEntity, C_SIGNATURE::GAMEPLAY);
        }

        TreePop();
    }

    ImGui::Separator();
}

void Inspector::FXInterface()
{
    if (TreeNode("FX"))
    {
        ComponentGameplay& gameplay = coordinator.componentHandler->GetComponentGameplay(selectedEntity.focusedEntity->id);

        //Life
        if (gameplay.signatureGameplay & CGP_SIGNATURE::LIVE)
        {
            CGPLive& life = gameplay.componentLive;

            //Death
            {
                Text("On Death :");
                if (BeginCombo("##DEATH_SFX", (life.sfxDeath) ? life.sfxDeath->filepath.c_str() : "SFX missing"))
                {
                    for (std::unordered_map<std::string, std::unique_ptr<Sound>>::iterator soundPtr = resources.sounds.begin(); soundPtr != resources.sounds.end(); ++soundPtr)
                        if (Selectable(soundPtr->first.c_str()))
                            life.sfxDeath = soundPtr->second.get();

                    EndCombo();
                }
                if (BeginCombo("##DEATH_VFX", (life.vfxDeath) ? life.vfxDeath->name.c_str() : "VFX missing"))
                {
                    for (std::unordered_map<std::string, std::unique_ptr<Particles::ParticlesPrefab>>::iterator particlePtr = resources.particles.begin(); particlePtr != resources.particles.end(); ++particlePtr)
                        if (Selectable(particlePtr->first.c_str()))
                            life.vfxDeath = particlePtr->second.get();

                    EndCombo();
                }
            }

            //Hit
            {
                Text("On Hit :");
                if (BeginCombo("##HIT_SFX", (life.sfxHit) ? life.sfxHit->filepath.c_str() : "SFX missing"))
                {
                    for (std::unordered_map<std::string, std::unique_ptr<Sound>>::iterator soundPtr = resources.sounds.begin(); soundPtr != resources.sounds.end(); ++soundPtr)
                        if (Selectable(soundPtr->first.c_str()))
                            life.sfxHit = soundPtr->second.get();

                    EndCombo();
                }
                if (BeginCombo("##HIT_VFX", (life.vfxHit) ? life.vfxHit->name.c_str() : "VFX missing"))
                {
                    for (std::unordered_map<std::string, std::unique_ptr<Particles::ParticlesPrefab>>::iterator particlePtr = resources.particles.begin(); particlePtr != resources.particles.end(); ++particlePtr)
                        if (Selectable(particlePtr->first.c_str()))
                            life.vfxHit = particlePtr->second.get();

                    EndCombo();
                }
            }
        }
        else
            TextDisabled("Component Gameplay Life missing");
        NewLine();

        //Attack
        if (gameplay.signatureGameplay & CGP_SIGNATURE::ATTACK)
        {
            CGPAttack& attack = gameplay.componentAttack;

            //Attack
            {
                Text("On Attack :");
                if (BeginCombo("##ATTACK_SFX", (attack.sfxAttack) ? attack.sfxAttack->filepath.c_str() : "SFX missing"))
                {
                    for (std::unordered_map<std::string, std::unique_ptr<Sound>>::iterator soundPtr = resources.sounds.begin(); soundPtr != resources.sounds.end(); ++soundPtr)
                        if (Selectable(soundPtr->first.c_str()))
                            attack.sfxAttack = soundPtr->second.get();

                    EndCombo();
                }
                if (BeginCombo("##ATTACK_VFX", (attack.vfxAttack) ? attack.vfxAttack->name.c_str() : "VFX missing"))
                {
                    for (std::unordered_map<std::string, std::unique_ptr<Particles::ParticlesPrefab>>::iterator particlePtr = resources.particles.begin(); particlePtr != resources.particles.end(); ++particlePtr)
                        if (Selectable(particlePtr->first.c_str()))
                            attack.vfxAttack = particlePtr->second.get();

                    EndCombo();
                }
            }
        }
        else
            TextDisabled("Component Gameplay Attack missing");
        NewLine();


        if (Selectable("Remove component##FX"))
            coordinator.componentHandler->RemoveComponent(*selectedEntity.focusedEntity, C_SIGNATURE::FX);

        TreePop();
    }

    ImGui::Separator();
}
