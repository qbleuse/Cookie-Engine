#include "D3D11Helper.hpp"
#include "Camera.hpp"
#include "ECS/Coordinator.hpp"
#include "ECS/ComponentHandler.hpp"
#include "Game.hpp"
#include "Scene.hpp"
#include "Resources/Mesh.hpp"
#include "DrawDataHandler.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::ECS;
using namespace Cookie::Render;

constexpr float cullEpsilon = -3.0f;

struct VS_CONSTANT_BUFFER
{
	Mat4 proj = Mat4::Identity();
	Mat4 view = Mat4::Identity();
};

/*======================= CONSTRUCTORS/DESTRUCTORS =======================*/

DrawDataHandler::DrawDataHandler()
{
}

DrawDataHandler::~DrawDataHandler()
{
}

/*========================= INIT METHODS =========================*/

void DrawDataHandler::Init(const Game& game)
{
	coordinator			= &game.coordinator;
	player				= &game.playerData;
}

void DrawDataHandler::SetScene(Resources::Scene* _scene)
{
	currentScene = _scene;
}

/*========================= DRAW DATA METHODS =========================*/

bool DrawData::operator==(const ECS::ComponentModel& model)
{
	return (model.mesh == mesh 
			&& model.albedo == albedo 
			&& model.normal == normalMap 
			&& model.metallicRoughness == matMap);
}

/*========================= FRUSTRUM METHODS =========================*/

void Frustrum::MakeFrustrum(const Camera& cam)
{
	Mat4 viewProj = cam.GetViewProj();

	//left plane
	planes[0].x = viewProj.c[3].e[0] + viewProj.c[0].e[0];
	planes[0].y = viewProj.c[3].e[1] + viewProj.c[0].e[1];
	planes[0].z = viewProj.c[3].e[2] + viewProj.c[0].e[2];
	planes[0].w = viewProj.c[3].e[3] + viewProj.c[0].e[3];

	//right plane
	planes[1].x = viewProj.c[3].e[0] - viewProj.c[0].e[0];
	planes[1].y = viewProj.c[3].e[1] - viewProj.c[0].e[1];
	planes[1].z = viewProj.c[3].e[2] - viewProj.c[0].e[2];
	planes[1].w = viewProj.c[3].e[3] - viewProj.c[0].e[3];

	//top plane
	planes[2].x = viewProj.c[3].e[0] - viewProj.c[1].e[0];
	planes[2].y = viewProj.c[3].e[1] - viewProj.c[1].e[1];
	planes[2].z = viewProj.c[3].e[2] - viewProj.c[1].e[2];
	planes[2].w = viewProj.c[3].e[3] - viewProj.c[1].e[3];

	//bottom plane
	planes[3].x = viewProj.c[3].e[0] + viewProj.c[1].e[0];
	planes[3].y = viewProj.c[3].e[1] + viewProj.c[1].e[1];
	planes[3].z = viewProj.c[3].e[2] + viewProj.c[1].e[2];
	planes[3].w = viewProj.c[3].e[3] + viewProj.c[1].e[3];

	//near plane
	planes[4].x = viewProj.c[2].e[0];
	planes[4].y = viewProj.c[2].e[1];
	planes[4].z = viewProj.c[2].e[2];
	planes[4].w = viewProj.c[2].e[3];

	//far plane
	planes[5].x = viewProj.c[3].e[0] - viewProj.c[2].e[0];
	planes[5].y = viewProj.c[3].e[1] - viewProj.c[2].e[1];
	planes[5].z = viewProj.c[3].e[2] - viewProj.c[2].e[2];
	planes[5].w = viewProj.c[3].e[3] - viewProj.c[2].e[3];

	for (int i = 0; i < planes.size(); i++)
	{
		planes[i] = planes[i].Normalize();
	}
}

/*========================= REALTIME METHODS =========================*/

void DrawDataHandler::SetDrawData(const Camera* cam)
{
	/* setup component for future renderpass and occlusion culling*/
	currentCam			= cam;
	frustrum.MakeFrustrum(*cam);

	/* set if the scene changed */
	if (currentScene)
	{
		mapDrawer.Set(currentScene->map);
		lights = &currentScene->lights;
	}

	/* get the different array of ECS */
	const Coordinator&			coord			= *coordinator;
	const ECS::EntityHandler&	entityHandler	= *coord.entityHandler;
	ECS::ComponentHandler&		components		= *coord.componentHandler;

	/* used to know if a model is culled or not */
	bool cull = false;

	for (int i = 0; i < entityHandler.livingEntities; ++i)
	{
		const Entity& iEntity = entityHandler.entities[i];

		/* we look up if the entity is displayable */
		if ((iEntity.signature & (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL)) == (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL))
		{
			/* seeing the signature it should be displayable but ... */
			ECS::ComponentModel& model = components.GetComponentModel(iEntity.id);

			/* we at least need a mesh to display the entity */
			if (model.mesh == nullptr)
			{
				continue;
			}

			/* occlusion culling process */
			Core::Math::Mat4& trs = components.GetComponentTransform(iEntity.id).TRS;
			cull = Cull(model, trs);

			/* at this point, we know if we should display it or not, we just separate it into two arrays:
			 * static and dynamic for a simpler drawing process, 
			 * only workers and those who have a move component should be able to move */
			ECS::ComponentGameplay& iGameplay = components.GetComponentGameplay(iEntity.id);
			if (iGameplay.signatureGameplay & (CGP_SIGNATURE::MOVE | CGP_SIGNATURE::WORKER))
			{
				PushDrawData(dynamicDrawData, model, trs, iGameplay, cull);
			}
			else
			{
				PushDrawData(staticDrawData, model, trs, iGameplay, cull);
			}
		}
	}

	for (int i = 0; i < coord.selectedEntities.size(); ++i)
	{
		Entity& iEntity = *coord.selectedEntities[i];
		if ((iEntity.signature & (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL)) == (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL))
		{
			ECS::ComponentModel& model = components.GetComponentModel(iEntity.id);

			if (model.mesh == nullptr)
			{
				continue;
			}

			/* there is not culling enabled for selected entity neither bactching */
			selectedDrawData.push_back({ model.mesh,model.albedo,model.normal,model.metallicRoughness });
			DrawData& draw = selectedDrawData[selectedDrawData.size() - 1];

			draw.matrices.push_back(components.GetComponentTransform(iEntity.id).TRS);
			draw.gameplays.push_back(&components.GetComponentGameplay(iEntity.id));
		}
	}
}

void DrawDataHandler::SetStaticDrawData(const Camera* cam)
{
	/* setup component for future renderpass and occlusion culling*/
	currentCam = cam;
	frustrum.MakeFrustrum(*cam);

	/* get the different array of ECS */
	const Coordinator& coord = *coordinator;
	const ECS::EntityHandler& entityHandler = *coord.entityHandler;
	ECS::ComponentHandler& components = *coord.componentHandler;

	/* used to know if a model is culled or not */
	bool cull = false;

	for (int i = 0; i < entityHandler.livingEntities; ++i)
	{
		const Entity& iEntity = entityHandler.entities[i];

		/* we look up if the entity is displayable */
		if ((iEntity.signature & (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL)) == (C_SIGNATURE::TRANSFORM + C_SIGNATURE::MODEL))
		{
			/* seeing the signature it should be displayable but ... */
			ECS::ComponentModel& model = components.GetComponentModel(iEntity.id);

			/* we at least need a mesh to display the entity */
			if (model.mesh == nullptr)
			{
				continue;
			}

			/* occlusion culling process */
			Core::Math::Mat4& trs = components.GetComponentTransform(iEntity.id).TRS;
			cull = Cull(model, trs);

			/* pushing if the entity is a static entity */
			ECS::ComponentGameplay& iGameplay = components.GetComponentGameplay(iEntity.id);
			if (!(iGameplay.signatureGameplay & (CGP_SIGNATURE::MOVE | CGP_SIGNATURE::WORKER)))
			{
				PushDrawData(staticDrawData, model, trs, iGameplay, cull);
			}
		}
	}
}

bool DrawDataHandler::Cull(ECS::ComponentModel& model, Core::Math::Mat4& trs)
{
	bool cull = false;

	/* change AABB to OBB */
	Vec4 modelMin = trs * Core::Math::Vec4(model.mesh->AABBMin, 1.0f);
	Vec4 modelMax = trs * Core::Math::Vec4(model.mesh->AABBMax, 1.0f);

	/* Check with each planes */
	for (int j = 0; j < frustrum.planes.size(); j++)
	{

		if ((frustrum.planes[j].Dot(modelMin) + frustrum.planes[j].w) < cullEpsilon && (frustrum.planes[j].Dot(modelMax) + frustrum.planes[j].w) < cullEpsilon)
		{
			cull = true;
			break;
		}

	}

	/* add to overall AABB if not culled */
	if (!cull)
	{
		AABB[0].x = std::min(modelMin.x, AABB[0].x);
		AABB[0].y = std::min(modelMin.y, AABB[0].y);
		AABB[0].z = std::min(modelMin.z, AABB[0].z);

		AABB[1].x = std::max(modelMax.x, AABB[1].x);
		AABB[1].y = std::max(modelMax.y, AABB[1].y);
		AABB[1].z = std::max(modelMax.z, AABB[1].z);
	}

	return cull;
}

void DrawDataHandler::PushDrawData(std::vector<DrawData>& drawDatas, const ECS::ComponentModel& model, const Core::Math::Mat4& trs, const ECS::ComponentGameplay& gameplay, bool culled)
{
	/* check in our drawDatas if ther is already a mesh like it */
	for (int i = 0; i < drawDatas.size(); i++)
	{
		DrawData& draw = drawDatas[i];
		if (draw == model)
		{
			draw.matrices.push_back(trs);
			draw.gameplays.push_back(&gameplay);

			if (!culled)
			{
				draw.visibleMatrices.push_back(trs);
				draw.visibleGameplays.push_back(&gameplay);
			}

			return;
		}
	}

	/* create another one otherwise */
	drawDatas.push_back({ model.mesh,model.albedo,model.normal,model.metallicRoughness });

	DrawData& draw = drawDatas[drawDatas.size() - 1];

	draw.matrices.push_back(trs);
	draw.gameplays.push_back(&gameplay);

	if (!culled)
	{
		draw.visibleMatrices.push_back(trs);
		draw.visibleGameplays.push_back(&gameplay);
	}
}

void DrawDataHandler::Draw(bool drawOccluded)
{
	modelDrawer.Draw(staticDrawData,drawOccluded);
	modelDrawer.Draw(dynamicDrawData,drawOccluded);
}

void DrawDataHandler::Clear()
{
	staticDrawData.clear();
	dynamicDrawData.clear();
	selectedDrawData.clear();
	AABB[0] = { std::numeric_limits<float>().max(),std::numeric_limits<float>().max() ,std::numeric_limits<float>().max() };
	AABB[1] = { -std::numeric_limits<float>().max(), -std::numeric_limits<float>().max() , -std::numeric_limits<float>().max() };
}