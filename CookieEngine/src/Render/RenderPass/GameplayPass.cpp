#include "D3D11Helper.hpp"
#include "DrawDataHandler.hpp"
#include "RenderPass/GameplayPass.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;


struct VS_CONSTANT_BUFFER
{
    Mat4 model;
    Mat4 viewProj;
};

/*=========================== CONSTRUCTORS/DESTRUCTORS ===========================*/

GameplayPass::GameplayPass()
{
    InitShader();
    InitState();
}

GameplayPass::~GameplayPass()
{
    if (rasterizerState)
    {
        rasterizerState->Release();
    }
    if (depthStencilState)
    {
        depthStencilState->Release();
    }
    if (outLineState)
    {
        outLineState->Release();
    }
    if (blendState)
    {
        blendState->Release();
    }
    if (PSampler)
    {
        PSampler->Release();
    }
    if (VShader)
        VShader->Release();
    if (PShader)
        PShader->Release();
    if (VCBuffer)
        VCBuffer->Release();
    if (PCBuffer)
        PCBuffer->Release();
}

/*=========================== INIT METHODS ===========================*/


void GameplayPass::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 31
    struct VOut
    {
        float4 position : SV_POSITION;
        float2 uv : UV;
    };

    cbuffer VS_CONSTANT_BUFFER : register(b0)
    {
        float4x4  model;
        float4x4  viewProj;
    };
    
    VOut main(float3 position : POSITION, float2 uv : UV, float3 normal : NORMAL)
    {
        VOut output;
    
        output.position = mul(mul(float4(position,1.0),model), viewProj);
        output.uv       = uv;
    
        return output;

    }
    )";

    Render::CompileVertex(source, &blob, &VShader);

    source = (const char*)R"(#line 31

    Texture2D	albedoTex : register(t0);
    
    SamplerState WrapSampler : register(s0);

    struct VOut
    {
        float4 position : SV_POSITION;
        float2 uv : UV;
    };

    cbuffer PS_CONSTANT_BUFFER : register(b0)
    {
        float4 color;
    };
    
    float4 main(VOut vertexOutput) : SV_TARGET
    {
        float3 texColor     = albedoTex.Sample(WrapSampler,vertexOutput.uv).rgb;

        float4 finalColor   = float4(lerp(texColor*color.rgb,color.rgb,step(0.0,dot(texColor,texColor))),color.a);
    
        return finalColor;

    }
    )";

    Render::CompilePixel(source, &PShader);

    VS_CONSTANT_BUFFER buffer = {};
    Render::CreateBuffer(&buffer, sizeof(VS_CONSTANT_BUFFER), &VCBuffer);

    Vec4 aColor;
    Render::CreateBuffer(&aColor, sizeof(Vec4), &PCBuffer);

    blob->Release();
}

void GameplayPass::InitState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    /* depth test enabled */
    depthStencilDesc.DepthEnable    = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS;

    /* depth test enabled, read, no write*/
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0;

    /* cull what was in stencil before */
    depthStencilDesc.FrontFace.StencilFailOp        = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp   = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilPassOp        = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc          = D3D11_COMPARISON_NOT_EQUAL;
    depthStencilDesc.BackFace.StencilFailOp         = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp    = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilPassOp         = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc           = D3D11_COMPARISON_NOT_EQUAL;

    RendererRemote::device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    /* keeps the depth test, enable write on stencil */
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    /* just write the entire thing that is being drawn in the stencil */
    depthStencilDesc.FrontFace.StencilFailOp        = D3D11_STENCIL_OP_REPLACE;
    depthStencilDesc.FrontFace.StencilDepthFailOp   = D3D11_STENCIL_OP_REPLACE;
    depthStencilDesc.FrontFace.StencilPassOp        = D3D11_STENCIL_OP_REPLACE;
    depthStencilDesc.FrontFace.StencilFunc          = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp         = D3D11_STENCIL_OP_REPLACE;
    depthStencilDesc.BackFace.StencilDepthFailOp    = D3D11_STENCIL_OP_REPLACE;
    depthStencilDesc.BackFace.StencilPassOp         = D3D11_STENCIL_OP_REPLACE;
    depthStencilDesc.BackFace.StencilFunc           = D3D11_COMPARISON_ALWAYS;

    RendererRemote::device->CreateDepthStencilState(&depthStencilDesc, &outLineState);

    D3D11_RASTERIZER_DESC rasterDesc = {};

    /* no culling cause it is easier that way (we display a lot of quad) */
    rasterDesc.CullMode                 = D3D11_CULL_NONE;
    rasterDesc.DepthClipEnable          = true;
    rasterDesc.FillMode                 = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise    = true;

    RendererRemote::device->CreateRasterizerState(&rasterDesc, &rasterizerState);

    D3D11_BLEND_DESC blenDesc = {  };

    /* turn on alpha blending */
    blenDesc.RenderTarget[0].BlendEnable            = true;
    blenDesc.RenderTarget[0].SrcBlend               = D3D11_BLEND_SRC_ALPHA;
    blenDesc.RenderTarget[0].DestBlend              = D3D11_BLEND_INV_SRC_ALPHA;
    blenDesc.RenderTarget[0].BlendOp                = D3D11_BLEND_OP_ADD;
    blenDesc.RenderTarget[0].SrcBlendAlpha          = D3D11_BLEND_SRC_ALPHA;
    blenDesc.RenderTarget[0].DestBlendAlpha         = D3D11_BLEND_INV_SRC_ALPHA;
    blenDesc.RenderTarget[0].BlendOpAlpha           = D3D11_BLEND_OP_ADD;
    blenDesc.RenderTarget[0].RenderTargetWriteMask  = D3D11_COLOR_WRITE_ENABLE_ALL;

    RendererRemote::device->CreateBlendState(&blenDesc, &blendState);

    D3D11_SAMPLER_DESC samDesc = {};

    /* sampler for textures */
    samDesc.Filter          = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressV        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressW        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.MaxAnisotropy   = 1;
    samDesc.ComparisonFunc  = D3D11_COMPARISON_ALWAYS;

    Render::CreateSampler(&samDesc, &PSampler);
}

/*=========================== REALTIME METHODS ===========================*/

void GameplayPass::Set()
{
    // Now set the rasterizer state.
    Render::RendererRemote::context->RSSetState(rasterizerState);
    // Set the depth stencil state.
    Render::RendererRemote::context->OMSetDepthStencilState(depthStencilState, 1);
    // Set the Blend State.
    const float blendFactor[4] = { 1.0f,1.0f,1.0f,0.0f };
    Render::RendererRemote::context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    Render::RendererRemote::context->PSSetSamplers(0, 1, &PSampler);

    /* set shader */
    Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);
    Render::RendererRemote::context->VSSetConstantBuffers(0, 1, &VCBuffer);
    Render::RendererRemote::context->PSSetConstantBuffers(0, 1, &PCBuffer);
}

void GameplayPass::Draw(const DrawDataHandler& drawData)
{
    ID3D11RenderTargetView* FBO = nullptr;
    Render::RendererRemote::context->OMGetRenderTargets(1, &FBO, nullptr);

    if (drawData.currentCam)
    {
        /* draw the selection quad and the building in construction */
        playerDrawer.Set(drawData);
        playerDrawer.Draw(VCBuffer, PCBuffer);

        /* set the shader */
        selectDrawer.Set(drawData);

        /* fill the stencil with the selectionned models */
        Render::RendererRemote::context->PSSetShader(nullptr, nullptr, 0);
        Render::RendererRemote::context->OMSetDepthStencilState(outLineState, 1);
        Render::RendererRemote::context->OMSetRenderTargets(0, nullptr, drawData.depthStencilView);
        selectDrawer.FillStencil(VCBuffer);

        /* draw outlines, and arrows */
        Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);
        Render::RendererRemote::context->OMSetRenderTargets(1, &FBO, drawData.depthStencilView);
        Render::RendererRemote::context->OMSetDepthStencilState(depthStencilState, 1);
        selectDrawer.Draw(VCBuffer, PCBuffer);

        selectDrawer.Clear();
    }

    FBO->Release();
}