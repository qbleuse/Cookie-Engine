#include "Render/D3D11Helper.hpp"
#include "Light.hpp"
#include "ECS/Coordinator.hpp"
#include "ECS/ComponentHandler.hpp"
#include "Resources/Mesh.hpp"
#include "Render/ShadowBuffer.hpp"
#include "Render/DrawDataHandler.hpp"
#include "Camera.hpp"
#include "RenderPass/ShadowPass.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

struct VS_CONSTANT_BUFFER
{
    Mat4 lightViewProj  = Mat4::Identity();
};

constexpr float shadowProjEpsilon = 100.0f;

/*======================= CONSTRUCTORS/DESTRUCTORS =======================*/

ShadowPass::ShadowPass():
    shadowViewport{0.0f,0.0f,SHADOW_MAP_RESOLUTION,SHADOW_MAP_RESOLUTION,0.0f,1.0f}
{
    InitShader();
    InitState();

    /* create a projection Matrix where we know model will be inside */
    proj = Mat4::Ortho(-shadowProjEpsilon, shadowProjEpsilon, -shadowProjEpsilon, shadowProjEpsilon, -shadowProjEpsilon, shadowProjEpsilon);
}

ShadowPass::~ShadowPass()
{
    if (VShader)
        VShader->Release();
    if (CBuffer)
        CBuffer->Release();
    if (depthStencilState)
        depthStencilState->Release();
    if (rasterizerState)
        rasterizerState->Release();
}

/*======================= INIT METHODS =======================*/

void ShadowPass::InitShader()
{

    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 35

    cbuffer CAM_CONSTANT : register(b0)
    {
        float4x4  lightViewProj;
    };
    
    float4 main(float3 position : POSITION, float2 uv : UV, float3 normal : NORMAL, float4x4 model : WORLD) : SV_POSITION
    {
        float4 pos;
    
        pos = mul(float4(position,1.0),model);
        pos = mul(pos, lightViewProj);

        return pos;

    }
    )";

    CompileVertex(source, &blob, &VShader);

    VS_CONSTANT_BUFFER buffer = {};

    Render::CreateBuffer(&buffer, sizeof(VS_CONSTANT_BUFFER), &CBuffer);

    blob->Release();
}

void ShadowPass::InitState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    /* depth test enabled, stencil disabled */
    depthStencilDesc.DepthEnable    = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable  = false;

    RendererRemote::device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    D3D11_RASTERIZER_DESC rasterDesc = {};

    /* cull is disabled because it looks better that way */
    rasterDesc.CullMode         = D3D11_CULL_NONE;
    rasterDesc.DepthClipEnable  = true;
    rasterDesc.FillMode         = D3D11_FILL_SOLID;

    RendererRemote::device->CreateRasterizerState(&rasterDesc, &rasterizerState);
}

/*======================= REALTIME METHODS =======================*/

void ShadowPass::Set()
{
    // Now set the rasterizer state.
    Render::RendererRemote::context->RSSetState(rasterizerState);
    // Set the depth stencil state.
    Render::RendererRemote::context->OMSetDepthStencilState(depthStencilState, 0);

    /* we only need the depth buffer */
    ID3D11RenderTargetView* rtvs[4] = { nullptr,nullptr ,nullptr ,nullptr };
    Render::RendererRemote::context->OMSetRenderTargets(4, rtvs, nullptr);

    /* clear up to disable the Pixel Shader */
    ID3D11ShaderResourceView* fbos[4] = { nullptr,nullptr,nullptr, nullptr};
    Render::RendererRemote::context->PSSetShaderResources(0, 4, fbos);

    /* set the shader */
    RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    RendererRemote::context->PSSetShader(nullptr, nullptr, 0);
}

void ShadowPass::Draw(DrawDataHandler& drawData)
{
   

    if (drawData.lights->useDir && drawData.lights->dirLight.castShadow)
    {
        /* set constant and viewport */
        Render::RendererRemote::context->RSSetViewports(1, &shadowViewport);
        Render::RendererRemote::context->VSSetConstantBuffers(0, 1, &CBuffer);

        /* set the shadow map */
        Render::RendererRemote::context->OMSetRenderTargets(0, nullptr, shadowMap.depthStencilView);

        DirLight& dirLight = drawData.lights->dirLight;

        /* set the proj */
        Vec3 jDir   = dirLight.dir.Normalize();
        /* we put the center of the camera at the center of the overall AABB of the visibles models */
        Vec3 pos    = (drawData.AABB[0] + drawData.AABB[1]) * 0.5f;
        Mat4 view   = Mat4::LookAt({ 0.0f,0.0f,0.0f }, jDir, { 0.0f,1.0f,0.0f });
        /* the camera is inverse of transform so not SRT but (T)^-1 * (R)^-1 */
        view = Mat4::Translate(-(pos - jDir * shadowProjEpsilon * 0.5f)) * view;

        /* set the camera */
        VS_CONSTANT_BUFFER buffer   = {};
        dirLight.lightViewProj      = view * proj;
        buffer.lightViewProj        = dirLight.lightViewProj;
        Render::WriteBuffer(&buffer, sizeof(buffer), 0, &CBuffer);

        /* draw all models, without frustrum culling */
        drawData.Draw(true);
    }
}

void ShadowPass::Clear()
{
    shadowMap.Clear();
}