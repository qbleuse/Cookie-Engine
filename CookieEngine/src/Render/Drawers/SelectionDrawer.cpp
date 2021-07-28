#include <vector>
#include "D3D11Helper.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Prefab.hpp"
#include "Render/Camera.hpp"
#include "DrawDataHandler.hpp"
#include "ECS/ComponentGameplay.hpp"
#include "Render/Drawers/SelectionDrawer.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

constexpr float upEpsilon = 1.0f;

struct VS_CONSTANT_BUFFER
{
    Mat4 model;
    Mat4 viewProj;
};


/*=========================== CONSTRUCTORS/DESTRUCTORS ===========================*/

SelectionDrawer::SelectionDrawer()
{
    /* creating a 2D arrow  on x/z plane */
    std::vector<float> vertices = { -0.05f, upEpsilon, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                                     0.05f, upEpsilon, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                                     0.05f, upEpsilon,  0.45f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
                                    -0.05f, upEpsilon,  0.45f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
                                    -0.15f, upEpsilon, 0.45f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                                     0.15f, upEpsilon, 0.45f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
                                     0.0f, upEpsilon, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f};

    std::vector<unsigned int> indices = { 3, 0, 1, 3, 1, 2, 4 , 5 , 6 };


    arrowMesh = std::make_unique<Resources::Mesh>("Arrow", vertices, indices, 9);
}

SelectionDrawer::~SelectionDrawer()
{
}

/*=========================== INIT METHODS ===========================*/


/*=========================== REALTIME METHODS ===========================*/

void SelectionDrawer::Set(const DrawDataHandler& drawData)
{
    viewProj            = drawData.currentCam->GetViewProj();
    stencilBuffer       = drawData.depthStencilView;
    selectedDrawData    = &drawData.selectedDrawData;

    for (int i = 0; i < drawData.selectedDrawData.size(); i++)
    {
        const ECS::ComponentGameplay& iGameplay = *drawData.selectedDrawData[i].gameplays[0];
       
        if (iGameplay.signatureGameplay & ECS::CGP_SIGNATURE::PRODUCER)
        {
            /* get the infos for the selected models */
            Mat4 mat = drawData.selectedDrawData[i].matrices.at(0);
            Vec3 pos = Vec3(mat.e[3], mat.e[7], mat.e[11]);
            pos.y = 0.0f;
            Vec3 tan = (Vec3(iGameplay.componentProducer.newUnitDestination.x,0.0f,iGameplay.componentProducer.newUnitDestination.z) - pos);

            /* get the infos for the arrows*/
            Mat4 arrMat = Mat4::Scale({ 5.0f ,1.0f, tan.Length()});
            arrMat = arrMat * Mat4::LookAt({ 0.0f,0.0f,0.0f }, Vec3(tan.x,0.0f,-tan.z).Normalize(), {0.0f,1.0f,0.0f});
            arrMat = arrMat * Mat4::Translate(pos + (tan * 0.5f));

            arrowsMatrices.push_back(arrMat);
        }
    }
}

void SelectionDrawer::FillStencil(ID3D11Buffer* VCBuffer)
{
    VS_CONSTANT_BUFFER buffer = {};
    buffer.viewProj = viewProj;
    size_t bufferSize = sizeof(VS_CONSTANT_BUFFER);

    const std::vector<DrawData>& selectedDrawData_ = *selectedDrawData;

    /* fill the stencil buffer with the models */
    for (int i = 0; i < selectedDrawData_.size(); i++)
    {
        const DrawData& drawData = selectedDrawData_[i];

        if (drawData.mesh)
        {
            buffer.model = drawData.matrices.at(0);
            Render::WriteBuffer(&buffer, bufferSize, 0, &VCBuffer);

            drawData.mesh->Set();
            drawData.mesh->Draw();
        }
    }
}

void SelectionDrawer::Draw(ID3D11Buffer* VCBuffer, ID3D11Buffer* PCBuffer)
{
    ID3D11ShaderResourceView* tex[1] = { nullptr };
    Render::RendererRemote::context->PSSetShaderResources(0, 1, tex);

    VS_CONSTANT_BUFFER buffer = {};
    buffer.viewProj = viewProj;

    Vec4 color = OUTLINE_COLOR;
    WriteBuffer(&color, sizeof(Vec4), 0, &PCBuffer);

    size_t bufferSize = sizeof(VS_CONSTANT_BUFFER);

    /* drawing outLines*/
    const std::vector<DrawData>& selectedDrawData_ = *selectedDrawData;

    for (int i = 0; i < selectedDrawData_.size(); i++)
    {
        const DrawData& drawData = selectedDrawData_[i];

        if (drawData.mesh)
        {
            /* scale up the models */
            buffer.model = outLineSize * drawData.matrices.at(0);
            Render::WriteBuffer(&buffer, bufferSize, 0, &VCBuffer);

            drawData.mesh->Set();
            drawData.mesh->Draw();
        }
    }

    /* clear stencil */
    Render::RendererRemote::context->ClearDepthStencilView(stencilBuffer, D3D11_CLEAR_STENCIL, 1.0f, 0);

    /* drawing arrows */
    color = ARROW_COLOR;
    WriteBuffer(&color, sizeof(Vec4), 0, &PCBuffer);

    arrowMesh->Set();

    Vec3 src = { -1.0f,0.0f,0.0f };
    Vec3 dest = { 7.5f,0.0f,6.5f };

    for (int i = 0; i < arrowsMatrices.size(); i++)
    {
        buffer.model = arrowsMatrices[i];
        Render::WriteBuffer(&buffer, bufferSize, 0, &VCBuffer);
        arrowMesh->Draw();
    }
}

void SelectionDrawer::Clear()
{
    arrowsMatrices.clear();
}