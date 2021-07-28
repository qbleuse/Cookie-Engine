#include "D3D11Helper.hpp"
#include "Mat4.hpp"
#include "Light.hpp"
#include "ShadowBuffer.hpp"
#include "Drawers/DirLightDrawer.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Render;

struct PS_DIRLIGHT_BUFFER
{
    Vec3 dir;
    float padding       = 0.0f;
    Vec3 color          = { 1.0f,1.0f,1.0f };
    float padding2      = 0.0f;
    Mat4  lightViewProj;
};

/*======================= CONSTRUCTORS/DESTRUCTORS =======================*/

DirLightDrawer::DirLightDrawer()
{
	InitShader();
}

DirLightDrawer::~DirLightDrawer()
{
    if (VShader)
        VShader->Release();
    if (PShader)
        PShader->Release();
    if (CBuffer)
        CBuffer->Release();
    if (CSampler)
        CSampler->Release();
}

/*======================= INIT METHODS =======================*/

void DirLightDrawer::InitShader()
{
    ID3DBlob* blob = nullptr;

    std::string source = (const char*)R"(#line 30
    struct VOut
    {
        float4 position : SV_POSITION;
        float2 uv : UV;
    };
    
    VOut main(uint vI :SV_VertexId )
    {
        VOut output;
    
        float2 uv = float2((vI << 1) & 2, vI & 2);
        output.uv = float2(1 - uv.x, uv.y);
        output.position = float4(-uv.x * 2 + 1, -uv.y * 2 + 1, 0, 1);
    
        return output;

    }
    )";

    CompileVertex(source, &blob, &VShader);

    /* the idea is of an include, PBR is in Light.hpp */
    source = std::string(PBR) + std::string((const char*)R"(

    Texture2D	positionTex : register(t0);
    Texture2D	normalTex   : register(t1);
    Texture2D   albedoTex   : register(t2);
    Texture2D   shadowMap   : register(t3);

    static const float shadowTexelSize = 1.0/4096.0;

    cbuffer DirLight : register(b1)
    {
        float3      lightDir;
        float       castShadow;
        float3      lightColor;
        float       padding;
        float4x4    lightViewProj;
    };

    SamplerState ClampSampler : register(s0);
    SamplerComparisonState ShadowSampler : register(s1);

    float compute_shadow(float3 fragPos, float dot)
    {
        float4 lightFragPos = mul(float4(fragPos,1.0),lightViewProj);
        lightFragPos /= lightFragPos.w;

        if(lightFragPos.z > 1.0)
            return 1.0;

        lightFragPos.x = 0.5 + lightFragPos.x * 0.5;
        lightFragPos.y = 0.5 - lightFragPos.y * 0.5;

        float shadow = 1.0f;
        //float margin = max(0.05 * (1.0 - dot), 0.00);
        float margin = clamp(0.005 / acos(saturate(dot)),0.0,0.05); 

        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                shadow += shadowMap.SampleCmpLevelZero(ShadowSampler,lightFragPos.xy + float2(x,y)*shadowTexelSize,lightFragPos.z - margin);       
            }    
        }
        
        shadow /= 9.0;

        return shadow;

    }

    POut main(float4 position : SV_POSITION, float2 uv : UV)
    {
        POut output;
            
        float3  fragPos     = positionTex.Sample(ClampSampler,uv).xyz;
        float3  normal      = normalize(normalTex.Sample(ClampSampler,uv).xyz);
        float   metallic    = positionTex.Sample(ClampSampler,uv).w;
        float   roughness   = normalTex.Sample(ClampSampler,uv).w;
        float   ao          = albedoTex.Sample(ClampSampler,uv).w;

        float   shadow      = lerp(1.0,compute_shadow(fragPos,dot(normal, normalize(-lightDir))),castShadow);

        output          = compute_lighting(normal,fragPos,normalize(-lightDir),lightColor,metallic,roughness);
        output.diffuse  = output.diffuse * shadow + (0.03 * ao);
        output.diffuse  = pow(output.diffuse,0.45454545);
        output.specular = pow(output.specular * shadow,0.45454545);

        return output;
    })");

    CompilePixel(source, &PShader);

    /* create the buffer for the dir Lights */
    PS_DIRLIGHT_BUFFER buffer = {};
    CreateBuffer(&buffer, sizeof(PS_DIRLIGHT_BUFFER), &CBuffer);

    /* create the comparison sampler for sampling the shadow map */
    D3D11_SAMPLER_DESC samDesc = {};
    samDesc.Filter          = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
    samDesc.AddressU        = D3D11_TEXTURE_ADDRESS_BORDER;
    samDesc.AddressV        = D3D11_TEXTURE_ADDRESS_BORDER;
    samDesc.AddressW        = D3D11_TEXTURE_ADDRESS_BORDER;
    samDesc.ComparisonFunc  = D3D11_COMPARISON_LESS_EQUAL;
    samDesc.BorderColor[0]  = samDesc.BorderColor[1] = samDesc.BorderColor[2] = samDesc.BorderColor[3] = 1.0f;
    samDesc.MinLOD          = 0;
    samDesc.MaxLOD          = D3D11_FLOAT32_MAX;

    Render::CreateSampler(&samDesc, &CSampler);

    blob->Release();
}

/*======================= REALTIME METHODS =======================*/

void DirLightDrawer::Set(const DirLight& dirLight, const ShadowBuffer& shadowMap, ID3D11Buffer** lightCBuffer)
{
    /* set shaders */
    RendererRemote::context->VSSetShader(VShader, nullptr, 0);
    RendererRemote::context->PSSetShader(PShader, nullptr, 0);

    /* set the input layout to null to exploit the default behavior of dx11 */
    RendererRemote::context->IASetInputLayout(nullptr);

    /* set sampler and constant buffer */
    ID3D11Buffer* buffer[] = { *lightCBuffer , CBuffer };
    Render::RendererRemote::context->PSSetConstantBuffers(0, 2, buffer);
    Render::RendererRemote::context->PSSetSamplers(1, 1, &CSampler);

    /*set the shadow map */
    ID3D11ShaderResourceView* shaderResources[1] = { nullptr };
    if (dirLight.castShadow)
    {
        shaderResources[0] = shadowMap.shaderResource;
    }
    Render::RendererRemote::context->PSSetShaderResources(3, 1, shaderResources);

    /* write info in constant buffer */
    PS_DIRLIGHT_BUFFER dirLightBuffer = { dirLight.dir,static_cast<float>(dirLight.castShadow), dirLight.color,0.0f, dirLight.lightViewProj };
    Render::WriteBuffer(&dirLightBuffer, sizeof(PS_DIRLIGHT_BUFFER), 0, &CBuffer);
}