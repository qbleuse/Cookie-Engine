#include <D3D11Helper.hpp>
#include "Render/RendererRemote.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Resources/Map.hpp"
#include "Primitives.hpp"
#include "Render/Drawers/MapDrawer.hpp"

using namespace Cookie::Render;
using namespace Cookie::Core::Math;

struct VS_CONSTANT_BUFFER
{
    Mat4 model;
    Vec4 tileNb;
};

/*============================= CONSTRUCTORS/DESTRUCTORS =============================*/

MapDrawer::MapDrawer():
    mapMesh{ Core::Primitives::CreateCube() }
{
	InitShader();
}

MapDrawer::~MapDrawer()
{
    if (VShader)
    {
        VShader->Release();
    }
    if (VCBuffer)
    {
        VCBuffer->Release();
    }
    if (ILayout)
    {
        ILayout->Release();
    }
}

/*============================= INIT METHODS =============================*/

void MapDrawer::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 27
    struct VOut
    {
        float4 position : SV_POSITION;
        float2 uv       : UV; 
        float3 normal   : NORMAL;
        float3 fragPos  : FRAGPOS;
        float4 view     : VIEW;
    };
    
    cbuffer MODEL_CONSTANT : register(b0)
    {
        float4x4  model;
        float2    tileSize;
    };

    cbuffer CAM_CONSTANT : register(b1)
    {
        float4x4  proj;
        float4x4  view;
    };

    
    VOut main(float3 position : POSITION, float2 uv : UV, float3 normal : NORMAL)
    {
        VOut output;
    
        output.fragPos  = mul(float4(position,1.0),model).xyz;
        output.view     = mul(float4(output.fragPos,1.0),view);
        output.position = mul(output.view, proj);
        output.uv       = uv * tileSize;
        output.normal   = normalize(mul(normal,(float3x3)model));
    
        return output;
    }
    )";

    CompileVertex(source, &blob, &VShader);

    struct Vertex
    {
        Core::Math::Vec3 position;
        Core::Math::Vec2 uv;
        Core::Math::Vec3 normal;
    };

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     offsetof(Vertex,position),  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV",       0, DXGI_FORMAT_R32G32_FLOAT,    0,     offsetof(Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     offsetof(Vertex, normal),   D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    Render::CreateLayout(&blob, ied, 3, &ILayout);

    VS_CONSTANT_BUFFER vbuffer = {};

    Render::CreateBuffer(&vbuffer, sizeof(VS_CONSTANT_BUFFER), &VCBuffer);

    blob->Release();
}

/*============================= REALTIME METHODS =============================*/

void MapDrawer::Set(const Resources::Map& map)
{
    albedoTex = map.model.albedo;
    normalTex = map.model.normal;
    matTex = map.model.metallicRoughness;

    model = map.trs.TRS;
    tileNb = map.tilesNb;
}

void MapDrawer::Draw()
{
    /* set shader*/
    Render::RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    Render::RendererRemote::context->IASetInputLayout(ILayout);

    /* set map info */
    VS_CONSTANT_BUFFER vbuffer   = {};
    vbuffer.model                = model;
    vbuffer.tileNb             = { tileNb.x,tileNb.y,0.0f,0.0f };

    Render::RendererRemote::context->VSSetConstantBuffers(0, 1, &VCBuffer);
    Render::WriteBuffer(&vbuffer, sizeof(vbuffer), 0, &VCBuffer);

    /* set texture */
    if (albedoTex)
        albedoTex->Set(0);
    if (normalTex)
        normalTex->Set(1);
    if (matTex)
        matTex->Set(2);

    /* and Draw! */
    mapMesh->Set();
    mapMesh->Draw();
}