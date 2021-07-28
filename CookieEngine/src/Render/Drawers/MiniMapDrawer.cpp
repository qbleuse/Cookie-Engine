#include "Core/Math/Mat4.hpp"
#include "Render/D3D11Helper.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture.hpp"
#include "Core/Primitives.hpp"
#include "Resources/Map.hpp"
#include "Render/DrawDataHandler.hpp"
#include "Render/Drawers/MiniMapDrawer.hpp"
#include "Render/Camera.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

struct VS_CONSTANT_BUFFER
{
    Mat4 model;
    Vec4 tileNb;
};

/*======================= CONSTRUCTORS/DESTRUCTORS =======================*/

MiniMapDrawer::MiniMapDrawer():
    mapMesh{Core::Primitives::CreateCube()},
    quadColor{ std::make_unique<Resources::Texture>("White", Vec4(MINI_MAP_QUAD_COLOR)) }
{
    InitShader();

    /* creating a quad that works with line strips */
    std::vector<float> vertices = { -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                                     0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                                     0.5f, 0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
                                    -0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f };
    
    std::vector<unsigned int> indices = { 0 , 1, 2, 3 , 0 };


    quad = std::make_unique<Resources::Mesh>("lineQuad", vertices, indices, 5);
}

MiniMapDrawer::~MiniMapDrawer()
{
    if (VShader)
    {
        VShader->Release();
    }
    if (PShader)
    {
        PShader->Release();
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


/*======================= INIT METHODS =======================*/


void MiniMapDrawer::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 27
    struct VOut
    {
        float4 position : SV_POSITION;
        float2 uv       : UV; 
    };
    
    cbuffer MODEL_CONSTANT : register(b0)
    {
        float4x4  model;
        float2    tileNb;
    };

    cbuffer CAM_CONSTANT : register(b1)
    {
        float4x4  proj;
        float4x4  view;
    };
    
    VOut main(float3 position : POSITION, float2 uv : UV, float3 normal : NORMAL)
    {
        VOut output;
    
		output.position = mul(mul(mul(float4(position,1.0),model),view), proj);
        output.uv       = uv * tileNb;
    
        return output;
    }
    )";

    Render::CompileVertex(source, &blob, &VShader);
    source = (const char*)R"(#line 83

    Texture2D	albedoTex   : register(t0);

    SamplerState WrapSampler : register(s0);

    float4 main(float4 position : SV_POSITION, float2 uv : UV) : SV_TARGET
    {
        return  float4(albedoTex.Sample(WrapSampler,uv).rgb,1.0);
    })";

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

    Render::CompilePixel(source, &PShader);

    VS_CONSTANT_BUFFER vbuffer = {};

    Render::CreateBuffer(&vbuffer, sizeof(VS_CONSTANT_BUFFER), &VCBuffer);

    blob->Release();
}

/*======================= REALTIME METHODS =======================*/

void MiniMapDrawer::Set(const Camera& cam, const Resources::Map& map)
{
    mapAlbedo = map.model.albedo;

    mapTrs = map.trs.TRS;
    tileNb = map.tilesNb;

    Vec3 middle     = cam.ScreenPointToWorldDir({ { 0.0f,0.0f } });
    //Vec3 UpperRight = cam.ScreenPointToWorldDir({ { 1.0f,1.0f } });
    //Vec3 DownLeft   = cam.ScreenPointToWorldDir({ { -1.0f,-1.0f } });
    //
    float t = (-cam.pos.y) / middle.y;
    middle = cam.pos + middle * t;
    //t = (-cam.pos.y) / UpperRight.y;
    //UpperRight = cam.pos + UpperRight * t;
    //t = (-cam.pos.y) / DownLeft.y;
    //DownLeft = cam.pos + DownLeft * t;

    quadTrs = Mat4::Scale({ 10.0f,1.0f,10.0f }) * Mat4::Translate(middle);
}

void MiniMapDrawer::Draw()
{
    /* set shader */
    RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    RendererRemote::context->PSSetShader(PShader, nullptr, 0);
    RendererRemote::context->IASetInputLayout(ILayout);

    /* filling constant buffer with map info  */
    VS_CONSTANT_BUFFER vbuffer = {};
    vbuffer.model   = mapTrs;
    vbuffer.tileNb  = { tileNb.x,tileNb.y,0.0f,0.0f };
    RendererRemote::context->VSSetConstantBuffers(0, 1, &VCBuffer);
    WriteBuffer(&vbuffer, sizeof(vbuffer), 0, &VCBuffer);;


    /* map texture */
    if (mapAlbedo)
        mapAlbedo->Set(0);

    /* then draw the map */
    mapMesh->Set();
    mapMesh->Draw();

    /* drawing the quad of view of the cam */
    RendererRemote::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

    /* removing depth buffer */
    ID3D11RenderTargetView* rtv = nullptr;
    Render::RendererRemote::context->OMGetRenderTargets(1, &rtv, nullptr);
    Render::RendererRemote::context->OMSetRenderTargets(1, &rtv, nullptr);

    /* we can use the same shader as it is pretty close put matrix in vbuffer */
    vbuffer.model = quadTrs;
    WriteBuffer(&vbuffer, sizeof(vbuffer), 0, &VCBuffer);

    /* set a white texture and draw */
    quadColor->Set();
    quad->Set();
    quad->Draw();

    /* when you use Getter in dx11 it adds a ref in the object, 
     * so we release it*/
    rtv->Release();
    RendererRemote::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}