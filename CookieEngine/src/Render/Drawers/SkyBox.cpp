#include "Render/D3D11Helper.hpp"
#include "Core/Primitives.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Render/Drawers/Skybox.hpp"
#include "Core/Math/Mat4.hpp"

using namespace Cookie::Render;
using namespace Cookie::Core::Math;

struct VS_CONSTANT_BUFFER
{
    Cookie::Core::Math::Mat4 proj = Cookie::Core::Math::Mat4::Identity();
    Cookie::Core::Math::Mat4 view = Cookie::Core::Math::Mat4::Identity();
};

/*==================== CONSTRUCTORS/DESTRUCTORS ====================*/

SkyBox::SkyBox():
	cube {Core::Primitives::CreateCube()}
{
    InitShader();
    InitRasterizer();
}

SkyBox::~SkyBox()
{
    if (rasterizerState)
        rasterizerState->Release();
    if (VShader)
        VShader->Release();
    if (PShader)
        PShader->Release();
    if (CBuffer)
        CBuffer->Release();
    if (ILayout)
        ILayout->Release();
    if (PSampler)
        PSampler->Release();
}

/*==================== INIT METHODS ====================*/

void SkyBox::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(struct VOut
    {
        float4 position : SV_POSITION;
        float3 pos : POSITION;
        float2 uv : UV;
    };

    cbuffer VS_CONSTANT_BUFFER : register(b0)
    {
        float4x4  proj;
        float4x4  view;
    };
    
    VOut main(float3 position : POSITION, float2 uv : UV, float3 normal : NORMAL)
    {
        VOut output;

        float3 pos      = mul(position,(float3x3)view);

        output.position = mul(float4(pos,1.0),proj);
        output.position.z = output.position.w;
        output.pos      = position;
        output.uv       = uv;
    
        return output;

    }
    )";

    Render::CompileVertex(source, &blob, &VShader);

    source = (const char*)R"(

    TextureCube skybox : register( t0 );

    SamplerState WrapSampler : register (s0);

    float4 main(float4 position : SV_POSITION, float3 pos : POSITION, float2 uv : UV) : SV_TARGET
    {
        return skybox.Sample(WrapSampler,pos);
    })";

    Render::CompilePixel(source, &PShader);

    struct Vertex
    {
        Core::Math::Vec3 position;
        Core::Math::Vec2 uv;
        Core::Math::Vec3 normal;
    };

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex,position), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    Render::CreateLayout(&blob, ied, 3, &ILayout);

    VS_CONSTANT_BUFFER buffer = {};

    Render::CreateBuffer(&buffer, sizeof(VS_CONSTANT_BUFFER), &CBuffer);

    // Create a sampler state
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

    blob->Release();
}

void SkyBox::InitRasterizer()
{
    D3D11_RASTERIZER_DESC rasterDesc = {};

    /* we want to draw the back face of cube */
    rasterDesc.CullMode = D3D11_CULL_FRONT;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = true;

    HRESULT result = RendererRemote::device->CreateRasterizerState(&rasterDesc, &rasterizerState);

    if (FAILED(result))
    {
        printf("Failed Creating Rasterizer State: %s\n", std::system_category().message(result).c_str());
    }
}

/*==================== REALTIME METHODS ====================*/

void SkyBox::Draw(const Core::Math::Mat4& proj, const Core::Math::Mat4& view)
{
    RendererRemote::context->RSSetState(rasterizerState);

    /* Shader */
    Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    Render::RendererRemote::context->PSSetShader(PShader, nullptr, 0);

    /* set texture sampler, constant buffer, and input layout */
    Render::RendererRemote::context->IASetInputLayout(ILayout);
    Render::RendererRemote::context->PSSetSamplers(0, 1, &PSampler);
    Render::RendererRemote::context->VSSetConstantBuffers(0, 1, &CBuffer);

    VS_CONSTANT_BUFFER buffer = { proj,view };

    Render::WriteBuffer(&buffer, sizeof(buffer), 0, &CBuffer);

    /* set texture */
	if (texture)
		texture->Set();
    else
    {
        ID3D11ShaderResourceView* tex = nullptr;
        RendererRemote::context->PSSetShaderResources(0, 1, &tex);
    }
	if (cube)
	{
        /* draw the cube! */
		cube->Set();
		cube->Draw();
	}
}