#include "D3D11Helper.hpp"
#include "Core/Primitives.hpp"
#include "Mat4.hpp"
#include "Light.hpp"
#include "Mesh.hpp"
#include "DrawDataHandler.hpp"
#include "Drawers/PointLightDrawer.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

struct INSTANCE_POINT_LIGHT
{
    Vec3 position;
    float radius = 0.0f;
    Vec3 color = { 1.0f,1.0f,1.0f };
};

/*======================= CONSTRUCTORS/DESTRUCTORS =======================*/

PointLightDrawer::PointLightDrawer()
{
    sphereMesh = Core::Primitives::CreateIcoSphere();
    InitShader();
}

PointLightDrawer::~PointLightDrawer()
{
    if (VShader)
        VShader->Release();
    if (PShader)
        PShader->Release();
    if (IBuffer)
        IBuffer->Release();
    if (ILayout)
        ILayout->Release();
}

/*======================= INIT METHODS =======================*/

void PointLightDrawer::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 30
    struct VOut
    {
        float4  position    : SV_POSITION;
        float3  lightPos    : LIGHTPOS;
        float   radius      : RADIUS;
        float3  color       : COLOR;

    };

    struct VIn
    {
        float3 position     : POSITION;
        float2 uv           : UV;
        float3 normal       : NORMAL;
        float3 lightPos     : LIGHTPOS;
        float  radius       : LIGHTRADIUS;
        float3 lightColor   : LIGHTCOLOR;
    };

    cbuffer CAM_CONSTANT : register(b0)
    {
        float4x4  proj;
        float4x4  view;
    };
    
    VOut main(VIn input)
    {
        VOut output;
    
        float4x4 model = 0;
        model[0][0] = input.radius * 2.0;
        model[0][3] = input.lightPos.x;
        model[1][1] = input.radius * 2.0;
        model[1][3] = input.lightPos.y;
        model[2][2] = input.radius * 2.0;
        model[2][3] = input.lightPos.z;
        model[3][3] = 1.0;

        output.position = mul(mul(mul(model,float4(input.position,1.0)),view),proj);
        output.lightPos = input.lightPos;
        output.radius   = input.radius;
        output.color    = input.lightColor;
    
        return output;

    }
    )";

    CompileVertex(source, &blob, &VShader);

    source = std::string(PBR) + std::string((const char*)R"(

    Texture2D	positionTex : register(t0);
    Texture2D	normalTex   : register(t1);
    Texture2D   albedoTex   : register(t2);

    struct VOut
    {
        float4 position     : SV_POSITION;
        float3 lightPos     : LIGHTPOS;
        float radius        : RADIUS;
        float3 color        : COLOR;

    };

    SamplerState ClampSampler : register(s0);
    static const float clipEpsilon = 0.001;

    //from https://gamedev.net/forums/topic/664177-quadratic-attenuation-for-radius/5200635/
    float Falloff(float distance, float radius ) 
    {
    	float distOverRadius = distance / radius;
    	float distOverRadius2 = distOverRadius * distOverRadius;
    	float distOverRadius4 = distOverRadius2 * distOverRadius2;
    	float falloff = saturate( 1.0 - distOverRadius4 );
        falloff *= falloff;
    	falloff /=  (distance *distance) + 1.0;
        
        clip(falloff - clipEpsilon);

    	return falloff;
    }

    POut main(VOut vertexOutput)
    {
        POut output;
        
        float2 uv = vertexOutput.position.xy;
        uv.x = (uv.x/screenSize.x);
        uv.y = (uv.y/screenSize.y);

        float3  fragPos     = positionTex.Sample(ClampSampler,uv).xyz;
        float3  normal      = normalize(normalTex.Sample(ClampSampler,uv).xyz);
        float   metallic    = positionTex.Sample(ClampSampler,uv).w;
        float   roughness   = normalTex.Sample(ClampSampler,uv).w;
        float3  lightDir    = (vertexOutput.lightPos - fragPos);
        float   dist        = length(lightDir);

        output          = compute_lighting(normal,fragPos,normalize(lightDir),vertexOutput.color,metallic,roughness);
        output.diffuse  = output.diffuse * Falloff(dist,vertexOutput.radius);
        output.diffuse  = pow(output.diffuse,0.45454545);
        output.specular = pow(output.specular * Falloff(dist,vertexOutput.radius),0.45454545);//

        return output;
    })");

    CompilePixel(source, &PShader);

    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        /* mesh info */
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "UV",         0, DXGI_FORMAT_R32G32_FLOAT,    0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 },

        /* point light instance info */
        { "LIGHTPOS",       0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,                                       D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "LIGHTRADIUS",    0, DXGI_FORMAT_R32_FLOAT,       1, offsetof(INSTANCE_POINT_LIGHT,radius),   D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "LIGHTCOLOR",     0, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof(INSTANCE_POINT_LIGHT,color),    D3D11_INPUT_PER_INSTANCE_DATA, 1 }
    };

    Render::CreateLayout(&blob, ied, 6, &ILayout);

    /* create the instance buffer to fill the point light info */
    D3D11_BUFFER_DESC instanceBufferDesc;
    instanceBufferDesc.Usage                = D3D11_USAGE_DYNAMIC;
    instanceBufferDesc.ByteWidth            = sizeof(INSTANCE_POINT_LIGHT) * POINT_LIGHT_MAX_NB;
    instanceBufferDesc.BindFlags            = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags       = D3D11_CPU_ACCESS_WRITE;
    instanceBufferDesc.MiscFlags            = 0;
    instanceBufferDesc.StructureByteStride  = 0;

    Render::RendererRemote::device->CreateBuffer(&instanceBufferDesc, 0, &IBuffer);

    blob->Release();
}

/*======================= REALTIME METHODS =======================*/

void PointLightDrawer::Set(const LightsArray& lights, const DrawDataHandler& drawData)
{
    /* set the vertex shader and constant buffers */
    RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    RendererRemote::context->IASetInputLayout(ILayout);
    Render::RendererRemote::context->VSSetConstantBuffers(0, 1, &drawData.CamCBuffer);

    /* write the instance buffer with the point lights info */
    WriteBuffer(lights.pointLights.data(), sizeof(INSTANCE_POINT_LIGHT) * lights.usedPoints, 0, &IBuffer);

    /* set the vertex, index and instance buffer */
    UINT stride[2]              = { ((2 * sizeof(Core::Math::Vec3)) + sizeof(Core::Math::Vec2)), sizeof(INSTANCE_POINT_LIGHT) };
    UINT offset[2]              = { 0 , 0 };
    ID3D11Buffer* VBuffers[2]   = { sphereMesh->VBuffer, IBuffer };
    RendererRemote::context->IASetIndexBuffer(sphereMesh->IBuffer, DXGI_FORMAT_R32_UINT, 0);
    RendererRemote::context->IASetVertexBuffers(0, 2, VBuffers, stride, offset);
}

void PointLightDrawer::FillStencil(const unsigned int instanceNb)
{
    /* to fill stencil, no need  of pxiel buffer */
    RendererRemote::context->PSSetShader(nullptr, nullptr, 0);
    RendererRemote::context->DrawIndexedInstanced(sphereMesh->INb, instanceNb, 0, 0, 0);
}

void PointLightDrawer::Draw(ID3D11Buffer** lightCBuffer, const unsigned int instanceNb)
{
    /* set all things we need in pixel */
    RendererRemote::context->PSSetShader(PShader, nullptr, 0);
    Render::RendererRemote::context->PSSetConstantBuffers(0, 1, lightCBuffer);
    RendererRemote::context->DrawIndexedInstanced(sphereMesh->INb, instanceNb, 0, 0, 0);
}