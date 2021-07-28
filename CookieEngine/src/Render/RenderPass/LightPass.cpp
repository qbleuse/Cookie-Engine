#include "Render/D3D11Helper.hpp"
#include "Core/Math/Vec4.hpp"
#include "Light.hpp"
#include "RenderPass/LightPass.hpp"
#include "Render/DrawDataHandler.hpp"
#include "Camera.hpp"
#include "ShadowBuffer.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

/*======================= CONSTRUCTORS/DESTRUCTORS =======================*/

LightPass::LightPass(int width, int height) :
    diffuseFBO{ width, height, DXGI_FORMAT_R32G32B32A32_FLOAT },
    specularFBO {width, height, DXGI_FORMAT_R32G32B32A32_FLOAT}
{
    InitShader();
    InitState();
}

LightPass::~LightPass()
{
    if (lightCBuffer)
    {
        lightCBuffer->Release();
    }
    if (PSampler)
    {
        PSampler->Release();
    }
    if (depthStencilState)
    {
        depthStencilState->Release();
    }
    if (rasterizerState)
    {
        rasterizerState->Release();
    }
    if (blendState)
    {
        blendState->Release();
    }
    if (volumeRasterState)
    {
        volumeRasterState->Release();
    }
    if (volumeStencilState)
    {
        volumeStencilState->Release();
    }
}

/*======================= INIT METHODS =======================*/

void LightPass::InitShader()
{
    /* create sampler for picking geometry ans shadow info */
    D3D11_SAMPLER_DESC samDesc = {};
    samDesc.Filter          = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samDesc.AddressU        = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressV        = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.AddressW        = D3D11_TEXTURE_ADDRESS_CLAMP;
    samDesc.ComparisonFunc  = D3D11_COMPARISON_ALWAYS;

    Render::CreateSampler(&samDesc, &PSampler);

    /* create cam buffer */
    Vec4 cam = {};
    Render::CreateBuffer(&cam, sizeof(Vec4) * 2, &lightCBuffer);
}

void LightPass::InitState()
{
    /* the stencil is based on this technique https://dqlin.xyz/tech/2018/01/02/stencil/ */

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    /* setup depth test, for greater (will be the one used to draw) */
    depthStencilDesc.DepthEnable    = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_GREATER_EQUAL;

    /* we enable stencil read, not write */
    depthStencilDesc.StencilEnable      = true;
    depthStencilDesc.StencilReadMask    = 0xFF;
    depthStencilDesc.StencilWriteMask   = 0x00;

    /* basically for front and back face, we cull if stencil has culled it */ 
    depthStencilDesc.FrontFace.StencilFailOp        = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp   = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilPassOp        = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc          = D3D11_COMPARISON_NOT_EQUAL;
    depthStencilDesc.BackFace.StencilFailOp         = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp    = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilPassOp         = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc           = D3D11_COMPARISON_NOT_EQUAL;

    RendererRemote::device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    /* setup depth test, for less (will be the one used to draw) */
    depthStencilDesc.DepthEnable    = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS;

    /* we enable stencil read and write */
    depthStencilDesc.StencilEnable      = true;
    depthStencilDesc.StencilReadMask    = 0xFF;
    depthStencilDesc.StencilWriteMask   = 0xFF;

    /* basically we write on staencil if depth test is failed */
    depthStencilDesc.FrontFace.StencilFailOp        = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp   = D3D11_STENCIL_OP_REPLACE;
    depthStencilDesc.FrontFace.StencilPassOp        = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc          = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp         = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp    = D3D11_STENCIL_OP_REPLACE;
    depthStencilDesc.BackFace.StencilPassOp         = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc           = D3D11_COMPARISON_ALWAYS;

    RendererRemote::device->CreateDepthStencilState(&depthStencilDesc, &volumeStencilState);

    D3D11_RASTERIZER_DESC rasterDesc = {};

    /* back face culling ... */
    rasterDesc.CullMode                 = D3D11_CULL_BACK;
    rasterDesc.DepthClipEnable          = true;
    rasterDesc.FillMode                 = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise    = true;

    RendererRemote::device->CreateRasterizerState(&rasterDesc, &rasterizerState);

    /* ... and front face culling states */
    rasterDesc.CullMode = D3D11_CULL_FRONT;
    RendererRemote::device->CreateRasterizerState(&rasterDesc, &volumeRasterState);

    D3D11_BLEND_DESC blenDesc = {  };

    /* enable additive blending */
    blenDesc.RenderTarget[0].BlendEnable    = true;
    blenDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_ONE;
    blenDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_ONE;
    blenDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
    blenDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
    blenDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blenDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_MAX;
    blenDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    RendererRemote::device->CreateBlendState(&blenDesc, &blendState);
}

/*======================= REALTIME METHODS =======================*/

void LightPass::Set(FrameBuffer& posFBO, FrameBuffer& normalFBO, FrameBuffer& albedoFBO)
{
    // Now set the rasterizer state.
    Render::RendererRemote::context->RSSetState(rasterizerState);
    // Set the depth stencil state.
    Render::RendererRemote::context->OMSetDepthStencilState(depthStencilState, 1);
    // Set the Blend State.
    const float blendFactor[4] = { 1.0f,1.0f,1.0f,0.0f };
    Render::RendererRemote::context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    /* set the geometry pass info  */
    ID3D11ShaderResourceView* fbos[3] = { posFBO.shaderResource,normalFBO.shaderResource, albedoFBO.shaderResource};
    Render::RendererRemote::context->PSSetShaderResources(0, 3, fbos);
    Render::RendererRemote::context->PSSetSamplers(0, 1, &PSampler);
}

void LightPass::Draw(const ShadowBuffer& shadowMap, const DrawDataHandler& drawData)
{
    const Camera& cam           = *drawData.currentCam;
    const LightsArray& lights   = *drawData.lights;

    ID3D11RenderTargetView* rtvs[2] = { diffuseFBO.renderTargetView,specularFBO.renderTargetView };

    /* setup cam ans screen info */
    Vec4 camBuffer[2] = { { cam.pos.x,cam.pos.y ,cam.pos.z ,0.0f },{static_cast<float>(diffuseFBO.width),static_cast<float>(diffuseFBO.height),cam.camFar,0.0f} };
    Render::WriteBuffer(&camBuffer, sizeof(Vec4) * 2, 0, &lightCBuffer);

    if (lights.useDir)
    {
        /* we draw a full screen quad to emulate a directionnal lights */
        Render::RendererRemote::context->OMSetRenderTargets(2, rtvs, nullptr);
        dirDrawer.Set(lights.dirLight, shadowMap, &lightCBuffer);
        /*  we exploit the default behavior of 
         * having no vertex buffer of dx11 */
        Render::RendererRemote::context->Draw(3, 0);
    }
    if (lights.usedPoints > 0)
    {
        /* setting the information of point lights */
        ID3D11RenderTargetView* null[2] = { nullptr, nullptr };
        pointDrawer.Set(lights, drawData);

        /* Fill the Stencil buffer, to cull the first parts */
        Render::RendererRemote::context->RSSetState(volumeRasterState);
        Render::RendererRemote::context->OMSetRenderTargets(2, null, nullptr);
        Render::RendererRemote::context->OMSetRenderTargets(0, nullptr, drawData.depthStencilView);
        Render::RendererRemote::context->OMSetDepthStencilState(depthStencilState, 1);
        pointDrawer.FillStencil(lights.usedPoints);

        /* then draw with the stencil info you have */
        Render::RendererRemote::context->RSSetState(rasterizerState);
        Render::RendererRemote::context->OMSetRenderTargets(2, rtvs, drawData.depthStencilView);
        Render::RendererRemote::context->OMSetDepthStencilState(volumeStencilState, 1);
        pointDrawer.Draw(&lightCBuffer, lights.usedPoints);

        Render::RendererRemote::context->ClearDepthStencilView(drawData.depthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
    
}

void LightPass::Clear()
{
    Core::Math::Vec4 clearColor = { 0.0f,0.0f,0.0f,1.0f };

     Render::RendererRemote::context->ClearRenderTargetView(diffuseFBO.renderTargetView, clearColor.e);
     Render::RendererRemote::context->ClearRenderTargetView(specularFBO.renderTargetView, clearColor.e);
}