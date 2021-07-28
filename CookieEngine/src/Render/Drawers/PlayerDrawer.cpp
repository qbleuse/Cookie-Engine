#include <vector>
#include "D3D11Helper.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Prefab.hpp"
#include "Render/Camera.hpp"
#include "DrawDataHandler.hpp"
#include "PlayerData.hpp"
#include "Render/Drawers/PlayerDrawer.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

constexpr float upEpsilon = 0.1f;


struct VS_CONSTANT_BUFFER
{
    Mat4 model;
    Mat4 viewProj;
};


/*=========================== CONSTRUCTORS/DESTRUCTORS ===========================*/

PlayerDrawer::PlayerDrawer()
{
    /* creating a quad already rotated */
    std::vector<float> vertices = { -0.5f, upEpsilon, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                                     0.5f, upEpsilon, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                                     0.5f, upEpsilon,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
                                    -0.5f, upEpsilon,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f };

    std::vector<unsigned int> indices = { 3, 0, 1, 3, 1, 2 };


    quadMesh = std::make_unique<Resources::Mesh>("SelectQuad", vertices, indices, 6);
}

PlayerDrawer::~PlayerDrawer()
{
}

/*=========================== INIT METHODS ===========================*/


/*=========================== REALTIME METHODS ===========================*/

void PlayerDrawer::Set(const DrawDataHandler& drawData)
{
    const Gameplay::PlayerData& player = *drawData.player;

    viewProj = drawData.currentCam->GetViewProj();

    /* get the quad info and save it the way we need it */
    isMakingQuad = player.makingASelectionQuad;
    if (isMakingQuad)
    {
        Vec3 quadCenter = (player.mousePosInWorld + player.selectionQuadStart) * 0.5f;
        quadTrs = Mat4::Scale({(quadCenter.x - player.selectionQuadStart.x)*2.0f,1.0f,(quadCenter.z - player.selectionQuadStart.z)*2.0f }) * Mat4::Translate(quadCenter);
    }

    /* get the info of the building we need */
    isValid = player.isBuildingValid;
    if (player.workerWhoBuild)
    {
        if (player.workerWhoBuild->possibleBuildings[player.indexOfBuildingInWorker])
        {
            buildingMesh         = player.workerWhoBuild->possibleBuildings[player.indexOfBuildingInWorker]->model.mesh;
            buildingAlbedoTex    = player.workerWhoBuild->possibleBuildings[player.indexOfBuildingInWorker]->model.albedo;
            buildingTRS          = player.workerWhoBuild->possibleBuildings[player.indexOfBuildingInWorker]->transform.TRS * Mat4::Translate(player.buildingPos);
        }
    }
    else /* if not displayable for the moment disable it by making mesh null */
    {
        buildingMesh = nullptr;
    }
}

void PlayerDrawer::Draw(ID3D11Buffer* VCBuffer, ID3D11Buffer* PCBuffer)
{
    ID3D11ShaderResourceView* tex[1] = {nullptr};
    Render::RendererRemote::context->PSSetShaderResources(0, 1, tex);

    /* setup the cam first */
    VS_CONSTANT_BUFFER buffer   = {};
    size_t bufferSize           = sizeof(VS_CONSTANT_BUFFER);
    buffer.viewProj             = viewProj;

    /* setup the default color */
    Vec4 color          = validColor;
    size_t colorSize    = sizeof(Vec4);
    Render::WriteBuffer(&color, colorSize, 0, &PCBuffer);

    /* draw quad */
    if (isMakingQuad)
    {
        buffer.model = quadTrs;
        Render::WriteBuffer(&buffer, bufferSize, 0, &VCBuffer);

        quadMesh->Set();
        quadMesh->Draw();
    }

    /* draw building */
    if (buildingMesh)
    {
        buffer.model = buildingTRS;
        Render::WriteBuffer(&buffer, bufferSize, 0, &VCBuffer);

        if (!isValid)
        {
            /* write the invalid color */
            color = invalidColor;
            Render::WriteBuffer(&color, colorSize, 0, &PCBuffer);
        }

        if (buildingAlbedoTex)
        {
            /* setting texturr */
            tex[0] = buildingAlbedoTex->GetResourceView();
            Render::RendererRemote::context->PSSetShaderResources(0, 1, tex);
        }

        buildingMesh->Set();
        buildingMesh->Draw();
    }
}