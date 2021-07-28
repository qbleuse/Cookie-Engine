#include "RenderPass/ParticlesPass.hpp"
#include "Core/Math/Mat4.hpp"
#include "Render/RendererRemote.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Camera.hpp"

using namespace Cookie;
using namespace Cookie::Render;

struct VS_CONSTANT_BUFFER
{
    Cookie::Core::Math::Mat4 proj = Cookie::Core::Math::Mat4::Identity();
    Cookie::Core::Math::Mat4 view = Cookie::Core::Math::Mat4::Identity();
    Cookie::Core::Math::Vec3 pos;
    bool isBillboard;
};


ParticlesPass::ParticlesPass()
{
}

ParticlesPass::ParticlesPass(const ParticlesPass& pass) : ILayout(pass.ILayout), VShader(pass.VShader), PShader(pass.PShader), CBuffer(pass.CBuffer),
                InstanceBuffer(pass.InstanceBuffer), blendState(pass.blendState), PSampler(pass.PSampler), rasterizerState(pass.rasterizerState), 
                depthStencilState(pass.depthStencilState), mInstancedData(pass.mInstancedData)
{
    VShader->AddRef();
    PShader->AddRef();
    CBuffer->AddRef();
    ILayout->AddRef();
    InstanceBuffer->AddRef();
    blendState->AddRef();
    PSampler->AddRef();
    rasterizerState->AddRef();
    depthStencilState->AddRef();
}

ParticlesPass::~ParticlesPass()
{
}

void ParticlesPass::Destroy()
{
    if (VShader != nullptr)
        VShader->Release();
    if (PShader != nullptr)
        PShader->Release();
    if (CBuffer != nullptr)
        CBuffer->Release();
    if (ILayout != nullptr)
        ILayout->Release();
    if (InstanceBuffer != nullptr)
        InstanceBuffer->Release();
    if (blendState != nullptr)
        blendState->Release();
    if (PSampler != nullptr)
        PSampler->Release();
    if (rasterizerState != nullptr)
        rasterizerState->Release();
    if (depthStencilState != nullptr)
        depthStencilState->Release();
}

void ParticlesPass::Clean()
{
    Render::RendererRemote::context->VSSetShader(nullptr, nullptr, 0);
}

void ParticlesPass::InitShader()
{
	ID3DBlob* blob = nullptr;

	std::string source = (const char*)R"(#line 28
    cbuffer MatrixBuffer : register(b0)
    {
        float4x4 gProj;
	    float4x4 gView;
        float3 gCamPos;
        float padding;
    };

    struct VertexInputType
    {
        float3 PosL     : POSITION;
	    float2 Tex      : TEXCOORD;
        float3 NormalL  : NORMAL;
        float4x4 World  : WORLD;
	    float4 Color    : COLOR;
        uint isBillBoard : ISBILLBOARD;
    };
    
    struct PixelInputType
    {
        float4 PosH    : SV_POSITION;
        float3 PosW    : POSITION;
        float3 NormalW : NORMAL;
	    float2 Tex     : TEXCOORD;
	    float4 Color   : COLOR;
    };
    
    PixelInputType main(VertexInputType vin)
    {
    	PixelInputType vout;

        float4 temp = mul(float4(vin.PosL, 1.0f), vin.World);

        if (vin.isBillBoard == 1)
        {
            float3 z = normalize(gCamPos - temp);
            float3 y = float3(0, 1, 0);
            float3 x = normalize(cross(z, y));
            y = normalize(cross(x, z));


           float4x4 rot = float4x4(float4(x.x, x.y, x.z, 0),
                                    float4(y.x, y.y, y.z, 0),
                                    float4(-z.x, -z.y, -z.z, 0),
                                    float4(0.0, 0.0, 0.0, 1));

            temp = mul(float4(vin.PosL, 1.0), mul(rot, vin.World));
        }

	    vout.PosW    = temp.xyz;
	    vout.NormalW = mul(vin.NormalL, (float3x3)vin.World);
        
        float4x4 mat = mul(gView, gProj);

	    vout.PosH = mul(temp, mat);
	    vout.Tex   = vin.Tex;
	    vout.Color = vin.Color;

	    return vout;
    }
	)";

	Render::CompileVertex(source, &blob, &VShader);

	source = (const char*)R"(#line 53

    Texture2D	text : register(t0);    
    SamplerState WrapSampler : register(s0);

    struct PixelInputType
    {
        float4 PosH    : SV_POSITION;
        float3 PosW    : POSITION;
        float3 NormalW : NORMAL;
	    float2 Tex     : TEXCOORD;
	    float4 Color   : COLOR;
    };

    float4 main(PixelInputType input) : SV_TARGET
    {
        float4 finalColor = input.Color.rgba * text.Sample(WrapSampler, input.Tex);
        return finalColor; 
    }
	)";

	Render::CompilePixel(source, &PShader);

    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "ISBILLBOARD", 0, DXGI_FORMAT_R32_UINT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
    };
     
    Render::CreateLayout(&blob, ied, 9, &ILayout);

    mInstancedData.resize(1);

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(InstancedData) * mInstancedData.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    Render::RendererRemote::device->CreateBuffer(&vbd, 0, &InstanceBuffer);

    VS_CONSTANT_BUFFER buffer = {};
    buffer.pos = Cookie::Core::Math::Vec3();
    buffer.proj = Cookie::Core::Math::Mat4::Identity();
    buffer.view = Cookie::Core::Math::Mat4::Identity();

    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
    vbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = &buffer;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    Render::RendererRemote::device->CreateBuffer(&vbd, &InitData, &CBuffer);

    D3D11_BLEND_DESC blenDesc = {  };

    blenDesc.AlphaToCoverageEnable = false;
    blenDesc.IndependentBlendEnable = false;
    blenDesc.RenderTarget[0].BlendEnable = true;
    blenDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blenDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blenDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blenDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    blenDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blenDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blenDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    RendererRemote::device->CreateBlendState(&blenDesc, &blendState);

    D3D11_SAMPLER_DESC samDesc = {};
    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.MipLODBias = 0.0f;
    samDesc.MaxAnisotropy = 1;
    samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samDesc.BorderColor[0] = samDesc.BorderColor[1] = samDesc.BorderColor[2] = samDesc.BorderColor[3] = 0;
    samDesc.MinLOD = 0;
    samDesc.MaxLOD = 0;

    Render::CreateSampler(&samDesc, &PSampler);

    // Initialize the description of the stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = false;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    RendererRemote::device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    D3D11_RASTERIZER_DESC rasterDesc = {};

    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = true;
    rasterDesc.CullMode = D3D11_CULL_FRONT;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = false;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    HRESULT result = RendererRemote::device->CreateRasterizerState(&rasterDesc, &rasterizerState);

    if (FAILED(result))
    {
        printf("Failed Creating Rasterizer State: %s\n", std::system_category().message(result).c_str());
    }


    blob->Release();
}

/*=========================== REALTIME METHODS ===========================*/

void ParticlesPass::AllocateMoreSpace(int newSpace)
{
    if (InstanceBuffer != nullptr)
        InstanceBuffer->Release();
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(InstancedData) * newSpace;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    Render::RendererRemote::device->CreateBuffer(&vbd, 0, &InstanceBuffer);
    mInstancedData.resize(newSpace);
}

void ParticlesPass::Draw(const Cookie::Render::Camera& cam, Resources::Mesh* mesh, Resources::Texture* texture, std::vector<InstancedData> data)
{
    if (mesh == nullptr)
        return;
    Render::RendererRemote::context->OMSetDepthStencilState(depthStencilState, 0);
    Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);

    const float blendFactor[4] = { 1, 1, 1, 1 };
    Render::RendererRemote::context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    Render::RendererRemote::context->RSSetState(rasterizerState);

    if (mInstancedData.size() < data.size())
        AllocateMoreSpace(data.size());

    mInstancedData = data;

    ID3D11Buffer* vbs[2] = { mesh->VBuffer, InstanceBuffer };

    UINT stride[2] = { sizeof(Cookie::Core::Math::Vec3) + sizeof(Cookie::Core::Math::Vec2) + sizeof(Cookie::Core::Math::Vec3), sizeof(InstancedData) };
    UINT offset[2] = { 0, 0 };

    Render::RendererRemote::context->IASetInputLayout(ILayout);
    Render::RendererRemote::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Render::RendererRemote::context->VSSetConstantBuffers(0, 1, &CBuffer);
    Render::RendererRemote::context->PSSetSamplers(0, 1, &PSampler);

    if (texture != nullptr)
        texture->Set(0);

    VS_CONSTANT_BUFFER buffer = {};
    buffer.proj = cam.GetProj();
    buffer.view = cam.GetView();
    buffer.pos = cam.pos;
    buffer.isBillboard = true;
    Render::WriteBuffer(&buffer, sizeof(buffer), 0, &CBuffer);
 
    Render::RendererRemote::context->IASetVertexBuffers(0, 2, vbs, stride, offset);
    Render::RendererRemote::context->IASetIndexBuffer(mesh->IBuffer, DXGI_FORMAT_R32_UINT, 0);

    D3D11_MAPPED_SUBRESOURCE ms;
    Render::RendererRemote::context->Map(InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

    memcpy(ms.pData, mInstancedData.data(), sizeof(Render::InstancedData) * mInstancedData.size());
    Render::RendererRemote::context->Unmap(InstanceBuffer, 0);

    Render::RendererRemote::context->DrawIndexedInstanced(mesh->INb, mInstancedData.size(), 0, 0, 0);
}